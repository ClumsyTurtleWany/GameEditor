#include "pch.h"
#include "DeadLockProfiler.h"

void DeadLockProfiler::LockPush(const char* name)
{
	LockGuard guard(_lock);

	//���̵� ã�ų� �߱�
	int32 lockID = 0;

	auto findIt = _nameToID.find(name);
	if (findIt == _nameToID.end())
	{
		lockID = static_cast<int32>(_nameToID.size());
		_nameToID[name] = lockID;
		_idToName[lockID] = name;
	}
	else
	{
		lockID = findIt->second;
	}

	if (LlockStack.empty() == false)
	{
		//������ �߰ߵ��� ���� ���̽�(����)��� ����� ���� �ٽ� Ȯ��.
		const int32 prevID = LlockStack.top();
		if (lockID != prevID)
		{
			set<int32>& history = _lockHistory[prevID];
			if (history.find(lockID) == history.end())
			{
				history.insert(lockID);
				CheckCycle();
			}
		}
	}

	LlockStack.push(lockID);
}
void DeadLockProfiler::CheckCycle()
{
	//-1 = �湮�ȵ�

	const int32 lockCount = static_cast<int32>(_nameToID.size());
	_discoveredOrder = vector<int32>(lockCount, -1);
	_discoveredCount = 0;
	_finished = vector<bool>(lockCount, false);
	_parent = vector<int32>(lockCount, -1);
	for (int32 lockID = 0; lockID < lockCount; lockID++)
		Dfs(lockID);

	_discoveredOrder.clear();
	_finished.clear();
	_parent.clear();
}

void DeadLockProfiler::Dfs(int32 here)
{
	if (_discoveredOrder[here] != -1)
		return;

	_discoveredOrder[here] = _discoveredCount++;

	auto findIt = _lockHistory.find(here); // &map<here, set<int32>> _lockHistory
	if (findIt == _lockHistory.end())
	{
		_finished[here] = true;
		return;
	}
	set<int32>& nextSet = findIt->second;
	for (int32 there : nextSet)
	{
		//�湮���� ������ �湮
		if (_discoveredOrder[there] == -1)
		{
			_parent[there] = here;
			Dfs(there);
			continue;
		}
		//here�� there���� ���� �߰ߵǸ� there�� here�� �ļ�(������ ����)
		if (_discoveredOrder[here] < _discoveredOrder[there])
			continue;
		//�������� �ƴϰ�, Dfs(there)�� ���� �������� �ʾҴٸ�, there�� here�� �����̴�.(������ ����)
		if (_finished[there] == false)
		{
			printf("%s ->%s\n", _idToName[here], _idToName[there]);
			
			int32 now = here;
			while (1)
			{
				printf("%s -> %s\n", _idToName[_parent[now]], _idToName[now]);
				now = _parent[now];
				if (now == there)
					break;
			}
			CRASH("DEADLOCK_DETECTED");
		}
	}
	_finished[here] = true;
}

void DeadLockProfiler::LockPop(const char* name)
{
	LockGuard guard(_lock);

	if (LlockStack.empty()) CRASH("MULTIPLE_UNLOCK");
	
	int32 lockID = _nameToID[name];
	if (LlockStack.top() != lockID) CRASH("INVALID_UNLOCK");

	LlockStack.pop();
}
