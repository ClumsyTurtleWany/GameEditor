#include "pch.h"
#include "MemoryPool.h"

/*-----------------
	MemoryPool
------------------*/

MemoryPool::MemoryPool(int32 allocSize) : _allocSize(allocSize)
{
	InitializeSListHead(&_header);
}

MemoryPool::~MemoryPool()
{
	//memory에 대입되는 값이 nullptr이 아니면 
	while (MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header)))
		::_aligned_free(memory);

	/*while (_queue.empty() == false)
	{
		MemoryHeader* header = _queue.front();
		_queue.pop();
		free(header);
	}*/
}

//메모리 풀에 넣는거니까 사실상 메모리 반납
void MemoryPool::Push(MemoryHeader* ptr)
{
	ptr->allocSize = 0;

	::InterlockedPushEntrySList(&_header, static_cast<PSLIST_ENTRY>(ptr));

	_useCount.fetch_sub(1);
	_reserveCount.fetch_add(1);

	//WRITE_LOCK;
	//ptr->allocSize = 0;
	////실제 쓰는 크기를 넣어야 하니까 반납할때는 0으로 초기화?
	//
	////Pool에 메모리 반납
	//_queue.push(ptr);
	//
	//_useCount.fetch_sub(1);
}

//메모리풀에서 빼는거니까 사실상 메모리 할당 
MemoryHeader* MemoryPool::Pop()
{
	MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header));

	// 없으면 새로 만들다
	if (memory == nullptr)
	{
		memory = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(_allocSize, SLIST_ALIGNMENT));
	}
	else
	{
		ASSERT_CRASH(memory->allocSize == 0);
		_reserveCount.fetch_sub(1);
	}

	_useCount.fetch_add(1);

	return memory;

	//MemoryHeader* header = nullptr;
	//
	//{
	//	WRITE_LOCK;
	//	//pool에 여분이 있는지?
	//	if (_queue.empty() == false)
	//	{
	//		//있으면 하나 꺼내옴
	//		header = _queue.front();
	//		_queue.pop();
	//	}
	//}
	//
	////없으면 새로 만듬 
	//if (header == nullptr)
	//{
	//	header = reinterpret_cast<MemoryHeader*>(malloc(_allocSize));
	//
	//	//혹은 생성자에서 만들 수도 있음
	//}
	//else
	//{
	//	ASSERT_CRASH(header->allocSize == 0);
	//}
	//
	//_useCount.fetch_add(1);
	//
	//return header;
}
