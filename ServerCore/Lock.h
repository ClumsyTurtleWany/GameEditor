#pragma once
#include"Types.h"
/*----------------
    RW SpinLock
    Reader-Writer
    ���� ��� �����ߴٰ� Ǯ���ִ� ����� ���� ��� �ϱ� ������ ���ɶ�
-----------------*/

/*--------------------------------------------
* 1����Ʈ[********]
[WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
W : WriteFlag (Exclusive Lock Owner ThreadId)
R : ReadFlag (Shared Lock Count)
---------------------------------------------*/

// W -> W (0)
// W -> R (0)
// R -> W (X)

class Lock
{
    enum :uint32
    {
        ACQUIRE_TIMEOUT_TICK = 10000,
        MAX_SPIN_COUNT = 5000,

        WRITE_THREAD_MASK = 0xFFFF'0000,
        READ_COUNT_MASK = 0x0000'FFFF,
        EMPTY_FLAG = 0x0000'0000
    };

    Atomic<uint32> _lockFlag = EMPTY_FLAG;
    uint16 _writeCount = 0;
    //writelock�� �ɷ��ִ� ���¿��� �� ���� �ɶ� ũ���ð� ���°� �ƴ϶� �ѹ� �� ���� ��� ���ٷ���
   
public:
    void WriteLock(const char* name);
    void WritUnlock(const char* name);
    void ReadLock(const char* name);
    void ReadUnlock(const char* name);
};


///////////////////LockGuards/////////////////////////

class ReadLockGuard
{
    Lock& _lock;
    const char* _name;
public:
    ReadLockGuard(Lock& lock,const char* name) : _lock(lock),_name(name) { _lock.ReadLock(name); }
    ~ReadLockGuard() { _lock.ReadUnlock(_name); }
};

class WriteLockGuard
{
    Lock& _lock;
    const char* _name;
public:
    WriteLockGuard(Lock& lock, const char* name) : _lock(lock), _name(name) { _lock.WriteLock(name); }
    ~WriteLockGuard() { _lock.WritUnlock(_name); }
};