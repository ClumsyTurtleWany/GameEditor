#pragma once
#include"Allocator.h"

class MemoryPool;

/*-------------
	Memory
---------------*/
//https://inflearn.com/questions/359369
//https://www.inflearn.com/questions/357795

class Memory
{
	enum
	{
		//				  32	 +		8		+		8		= 48
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256), 
		MAX_ALLOC_SIZE = 4096,
	};

	vector<MemoryPool*> _pools;

	// 메모리 크기 <-> 메모리 풀 동기화
	// O(1) 빠르게 찾기 위한 테이블
	MemoryPool* _poolTable[MAX_ALLOC_SIZE + 1];
	//0~4096 바이트 안에 해당하면 그 바이트에 맞는 테이블에서 주소값을 찾기

public:
	Memory();
	~Memory();
public:
	void* Allocate(int32 size);
	void Release(void* ptr);
};

template<typename Type, typename... Args>
Type* xnew(Args&&... args)
{
	//이거까지만 하면 할당만 해주고 생성자 호출을 안시켜줌
	//Type* memory = static_cast<Type*>(BaseAllocator::Alloc(sizeof(Type)));
	//Type* memory = static_cast<Type*>(xAlloc(sizeof(Type)));
	Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));
	
	//operator new = 객체의 생성자를 호출
	//탬플릿으로 받으니까 어떤 생성자를 호출할지 몰라서 이것저것 다 받는다 라는 느낌
	new(memory)Type(forward<Args>(args)...);
	//new (buf) int(3); buf에 int타입 3을 넣는다
	//forward : 받은값이 L-Value인지 R-Value인지 확인
	//new (address) type (initializer) 

	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type();

	//BaseAllocator::Release(obj);
	//xRelease(obj);
	PoolAllocator::Release(obj);
}

//template<typename Type>
//shared_ptr<Type> MakeShared()
//{
//	return shared_ptr<Type>{xnew<Type>(), xdelete<Type>};
//}
template<typename Type, typename... Args>
shared_ptr<Type> MakeShared(Args&&... args)
{
	return shared_ptr<Type>{xnew<Type>(forward<Args>(args)...), xdelete<Type>};
}