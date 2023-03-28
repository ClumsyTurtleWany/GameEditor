#pragma once

/*--------------------
	BaseAllocator
---------------------*/

class BaseAllocator
{
public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);
};

/*-------------------
	StompAllocator
-------------------*/

class StompAllocator
{
	enum { PAGE_SIZE = 0x1000 };
public:
	static void*	Alloc(int32 size);
	static void		Release(void* ptr);
};

/*-------------------
	PoolAllocator
-------------------*/

class PoolAllocator
{
public:
	static void* Alloc(int32 size);
	static void		Release(void* ptr);
};

/*-------------------
	STL Allocator
-------------------*/
template<typename T>
class StlAllocator
{
public:
	using value_type = T;
	StlAllocator() {}

	template<typename Other>
	StlAllocator(const StlAllocator<Other>&) {} //삭제가 될때 따른 타입을 받아서 카피할려고 함(이런 인터페이스를 제공해야 에러 사라짐)

	T* allocate(size_t count) //실질적 데이터를 할당
	{
		const int32 size = static_cast<int32>(count * sizeof(T));
		//return static_cast<T*>(xAlloc(size));
		return static_cast<T*>(PoolAllocator::Alloc(size));
	}

	void deallocate(T* ptr, size_t count) //데이터 해제
	{
		//xRelease(ptr);
		PoolAllocator::Release(ptr);
	}
};

