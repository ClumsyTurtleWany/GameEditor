#pragma once

//threadmanager를 싱글톤 같은것을 메인에서 사용하기에는 좀 그럴때

extern class ThreadManager*		GThreadManager;
extern class DeadLockProfiler*	GDeadLockProfiler;
extern class Memory*			GMemory;
extern class SendBufferManager* GSendBufferManager;
