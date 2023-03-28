#pragma once
#include"Types.h"
/*----------------
    RW SpinLock
    Reader-Writer
    서로 잠시 참조했다가 풀어주는 방식을 자주 사용 하기 떄문에 스핀락
-----------------*/

/*--------------------------------------------
* 1바이트[********]
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
    //writelock이 걸려있는 상태에서 또 락을 걸때 크래시가 나는게 아니라 한번 더 락을 잡게 해줄려고
   
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