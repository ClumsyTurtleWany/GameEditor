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
	//memory�� ���ԵǴ� ���� nullptr�� �ƴϸ� 
	while (MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header)))
		::_aligned_free(memory);

	/*while (_queue.empty() == false)
	{
		MemoryHeader* header = _queue.front();
		_queue.pop();
		free(header);
	}*/
}

//�޸� Ǯ�� �ִ°Ŵϱ� ��ǻ� �޸� �ݳ�
void MemoryPool::Push(MemoryHeader* ptr)
{
	ptr->allocSize = 0;

	::InterlockedPushEntrySList(&_header, static_cast<PSLIST_ENTRY>(ptr));

	_useCount.fetch_sub(1);
	_reserveCount.fetch_add(1);

	//WRITE_LOCK;
	//ptr->allocSize = 0;
	////���� ���� ũ�⸦ �־�� �ϴϱ� �ݳ��Ҷ��� 0���� �ʱ�ȭ?
	//
	////Pool�� �޸� �ݳ�
	//_queue.push(ptr);
	//
	//_useCount.fetch_sub(1);
}

//�޸�Ǯ���� ���°Ŵϱ� ��ǻ� �޸� �Ҵ� 
MemoryHeader* MemoryPool::Pop()
{
	MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header));

	// ������ ���� �����
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
	//	//pool�� ������ �ִ���?
	//	if (_queue.empty() == false)
	//	{
	//		//������ �ϳ� ������
	//		header = _queue.front();
	//		_queue.pop();
	//	}
	//}
	//
	////������ ���� ���� 
	//if (header == nullptr)
	//{
	//	header = reinterpret_cast<MemoryHeader*>(malloc(_allocSize));
	//
	//	//Ȥ�� �����ڿ��� ���� ���� ����
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
