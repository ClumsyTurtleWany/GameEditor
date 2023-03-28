#include "pch.h"
#include "Allocator.h"
#include "Memory.h"

/*-------------------
	BaseAllocator
-------------------*/

void* BaseAllocator::Alloc(int32 size)
{
	return malloc(size);
}

void BaseAllocator::Release(void* ptr)
{
	free(ptr);
}

/*-------------------
	StompAllocator
-------------------*/

//8바이트만 할당한다 해도 4096바이트의 배수가 할당된다는 단점
//대신 버그를 잡아준다는 큰 장점 때문에 씀
//메모리 오버플로우 문제는 잡아주지 못하고있음->에초에 할당할때 겁나 크게 할당받음
//실제 사용할 부분 이외에 사용하면 안되지만 사용해도 에러는 뜨지 않음
//(에초에 크게 할당받았으니 프로그램적으로는 에러가 아님).
//따라서 실제 사용할 메모리 시작 위치를 할당받은 메모리의 뒷부분에 배치 하면 오버플로우 문제는 해결 
//근대 언더플로우 문제는 발생 가능성이 있으나 대부분의 문제는 오버플로우 문제지 언더플로우 문제는 거의
//일어나지 않음

void* StompAllocator::Alloc(int32 size)
{
	//const int64 pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	//return ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	// 
	//메모리 오버플로우 문제는 잡아주지 못하고있음->에초에 할당할때 겁나 크게 할당받음
	//실제 사용할 부분 이외에 사용하면 안되지만 사용해도 에러는 뜨지 않음
	//(에초에 크게 할당받았으니 프로그램적으로는 에러가 아님).
	//따라서 실제 사용할 메모리 시작 위치를 할당받은 메모리의 뒷부분에 배치 하면 오버플로우 문제는 해결 
	//근대 언더플로우 문제는 발생 가능성이 있으나 대부분의 문제는 오버플로우 문제지 언더플로우 문제는 거의
	//일어나지 않음
	//반올림 문제 해결을 위함
	const int64 pageCount = (size + (PAGE_SIZE - 1)) / PAGE_SIZE;
	//ex) (4 + 4096 - 1 )/4096 =1.xxxxx인데 여기서 int -> 소수 삭제
	const int64 dataOffset = pageCount * PAGE_SIZE - size;//오버플로우 문제를 해결하기위해 실데이터 시작위치 셋팅
	void* baseAddress = ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	return static_cast<void*>(static_cast<int8*>(baseAddress)+dataOffset);
}

void StompAllocator::Release(void* ptr)
{
	//VirtualFree(reinterpret_cast<void*>(ptr), 0, MEM_RELEASE);
	//오버플로우 방지를 위해 실 데이터를 뒤로 밀었을때
	const int64 address = reinterpret_cast<int64>(ptr);
	//https://inflearn.com/questions/497841
	const int64 baseAddress = address - (address % PAGE_SIZE);
	VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
}

/*-------------------
	PoolAllocator
-------------------*/

void* PoolAllocator::Alloc(int32 size)
{
	return GMemory->Allocate(size);
}

void PoolAllocator::Release(void* ptr)
{
	GMemory->Release(ptr);
}
