#pragma once

#include<stack>
#include<map>
#include<vector>



class DeadLockProfiler
{
private:
	unordered_map<const char*, int32>	_nameToID;
	unordered_map<int32, const char*>	_idToName;
	//stack<int32>						_lockStack; //��Ƽ������ ȯ�濡�� ������ ���̴� ������ �߰�, TLS�� �M 
	map<int32, set<int32>>				_lockHistory;
	mutex								_lock;

private: //����Ŭ �������� üũ�ϴ� ������
	vector<int32>	_discoveredOrder;	//��尡 �߰ߵ� ������ ����ϴ� �迭
	int32			_discoveredCount;	//��尡 �߰ߵ� ����
	vector<bool>	_finished;			//Dfs(i)�� ����Ǿ����� ����
	vector<int32>	_parent;			//�߰ߵ� ����� �θ�

public:
	void LockPush(const char* name);
	void LockPop(const char* name);
	void CheckCycle();

private:
	void Dfs(int32 here);	//Depth First Search ���� �켱 Ž��
};

