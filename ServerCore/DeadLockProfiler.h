#pragma once

#include<stack>
#include<map>
#include<vector>



class DeadLockProfiler
{
private:
	unordered_map<const char*, int32>	_nameToID;
	unordered_map<int32, const char*>	_idToName;
	//stack<int32>						_lockStack; //멀티쓰레드 환경에서 스텍이 꼬이는 현상이 발견, TLS로 뻄 
	map<int32, set<int32>>				_lockHistory;
	mutex								_lock;

private: //사이클 돌때마다 체크하는 변수들
	vector<int32>	_discoveredOrder;	//노드가 발견된 순서를 기록하는 배열
	int32			_discoveredCount;	//노드가 발견된 순서
	vector<bool>	_finished;			//Dfs(i)가 종료되었는지 여부
	vector<int32>	_parent;			//발견된 노드의 부모

public:
	void LockPush(const char* name);
	void LockPop(const char* name);
	void CheckCycle();

private:
	void Dfs(int32 here);	//Depth First Search 깊이 우선 탐색
};

