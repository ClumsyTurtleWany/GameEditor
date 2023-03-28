#pragma once

/*---------------------------------------
				LOCK
----------------------------------------*/

#define USE_MANY_LOCKS(count)	Lock _locks[count];
#define USE_LOCK				USE_MANY_LOCKS(1);
#define READ_LOCK_IDX(idx)		ReadLockGuard readLockGuard_##idx(_locks[idx],typeid(this).name());
#define READ_LOCK				READ_LOCK_IDX(0);
#define WRITE_LOCK_IDX(idx)		WriteLockGuard writeLockGuard_##idx(_locks[idx],typeid(this).name());
#define WRITE_LOCK				WRITE_LOCK_IDX(0);

/*---------------------------------------
				LOCK
----------------------------------------*/

//object pool / memory pool 을 사용하면 stomp Allocator을 사용하지 못함 → 사용할 수있게 수정
//pool Allocator 내부에서 메모리 풀링 을 적용시킬지 stomp allocator을 사용하게 할지 분기 나눠보자

//#ifdef _DEBUG
////							stompAllocator에서 변경
//#define xAlloc(size)	PoolAllocator::Alloc(size) 
//#define xRelease(ptr)	PoolAllocator::Release(ptr)
//#else //나중에 변경 예정
//#define xalloc(size)	BaseAllocator::Alloc(size)
//#define xrelease(ptr)	BaseAllocator::Release(ptr)
//#endif // _DEBUG



/*---------------------------------------
				CRASH
----------------------------------------*/

//인위적인 크래시를 내고싶을때
#define CRASH(cause)						\
{											\
	uint32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

//조건이 false일때 크래시
#define ASSERT_CRASH(expr)			\
{									\
	if (!(expr))					\
	{								\
		CRASH("ASSERT_CRASH");		\
		__analysis_assume(expr);	\
	}								\
}

//근대 이럴꺼면 assert사용하면안대나?
