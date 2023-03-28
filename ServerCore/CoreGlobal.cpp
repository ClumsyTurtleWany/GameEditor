#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"
#include "Memory.h"
#include"SocketUtils.h"
#include"SendBuffer.h"

ThreadManager*		GThreadManager = nullptr;
DeadLockProfiler*	GDeadLockProfiler = nullptr;
Memory*				GMemory = nullptr;
SendBufferManager* GSendBufferManager = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager =	new ThreadManager();
		GDeadLockProfiler = new DeadLockProfiler();
		GMemory =			new Memory();
		GSendBufferManager = new SendBufferManager();
		SocketUtils::Init();
	}
	~CoreGlobal()
	{
		delete GThreadManager;
		delete GDeadLockProfiler;
		delete GMemory;
		delete GSendBufferManager;
		SocketUtils::Clear();
	}
};

CoreGlobal gCoreGlobal;