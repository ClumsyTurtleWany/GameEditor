#include "pch.h"
#include "Lock.h"
#include"CoreTLS.h"
#include"DeadLockProfiler.h"
void Lock::WriteLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->LockPush(name);
#endif // _DEBUG
	//������ �����尡 �����ϰ� �ִٸ� ������ ����.
	const uint32 lockThreadID = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadID)
	{
		_writeCount++;
		return;
	}

	//�ƹ��� ���� �� ���� �ϰ����� ���� ���, ������ �������� �������� ����
	const int64 beginTick = GetTickCount64(); //�ð� üũ
	
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);
	while (1)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = EMPTY_FLAG;
			if (_lockFlag.compare_exchange_strong(expected, desired))
			{
				_writeCount++;
				return;
			}
		}
		if (GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		this_thread::yield();
	}
}

void Lock::WritUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->LockPop(name);
#endif // _DEBUG
	//ReadLock �� Ǯ�� ������ WriteUnlock�Ұ���
	if ((_lockFlag.load() & READ_COUNT_MASK) != 0)
		CRASH("INVALID_UNLOCK_ORDER");

	const int32 lockCount = --_writeCount;
	if (lockCount == 0)
		_lockFlag.store(EMPTY_FLAG);
}

void Lock::ReadLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->LockPush(name);
#endif // _DEBUG
	//������ �����尡 �ҿ��ϰ� �ִٸ� ������ ����
	const uint32 lockThreadID = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadID)
	{
		_lockFlag.fetch_add(1);
		return;
	}

	//�ƹ��� �����ϰ� ���������� �����ؼ� ���� ī��Ʈ �ø�
	const int64 beginTick = GetTickCount64();

	while (1)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = (_lockFlag.load() & READ_COUNT_MASK);
			if (_lockFlag.compare_exchange_strong(expected, expected + 1))
				return;
		}
		if (GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		this_thread::yield();
	}
}

void Lock::ReadUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->LockPop(name);
#endif // _DEBUG
	if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
		CRASH("MULTIPLE_UNLOCK");
}
