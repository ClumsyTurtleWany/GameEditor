#pragma once

#include<thread>
#include<functional>

class ThreadManager
{
	Mutex _lock;
	vector<thread> _threads;

public:
	ThreadManager();
	~ThreadManager();
	
	void Launch(function<void(void)> callback);
	void Join();

	static void InitTLS();
	static void DestroyTLS();
public:
	bool eraseThreadVector();
};

