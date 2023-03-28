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

//8����Ʈ�� �Ҵ��Ѵ� �ص� 4096����Ʈ�� ����� �Ҵ�ȴٴ� ����
//��� ���׸� ����شٴ� ū ���� ������ ��
//�޸� �����÷ο� ������ ������� ���ϰ�����->���ʿ� �Ҵ��Ҷ� �̳� ũ�� �Ҵ����
//���� ����� �κ� �̿ܿ� ����ϸ� �ȵ����� ����ص� ������ ���� ����
//(���ʿ� ũ�� �Ҵ�޾����� ���α׷������δ� ������ �ƴ�).
//���� ���� ����� �޸� ���� ��ġ�� �Ҵ���� �޸��� �޺κп� ��ġ �ϸ� �����÷ο� ������ �ذ� 
//�ٴ� ����÷ο� ������ �߻� ���ɼ��� ������ ��κ��� ������ �����÷ο� ������ ����÷ο� ������ ����
//�Ͼ�� ����

void* StompAllocator::Alloc(int32 size)
{
	//const int64 pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	//return ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	// 
	//�޸� �����÷ο� ������ ������� ���ϰ�����->���ʿ� �Ҵ��Ҷ� �̳� ũ�� �Ҵ����
	//���� ����� �κ� �̿ܿ� ����ϸ� �ȵ����� ����ص� ������ ���� ����
	//(���ʿ� ũ�� �Ҵ�޾����� ���α׷������δ� ������ �ƴ�).
	//���� ���� ����� �޸� ���� ��ġ�� �Ҵ���� �޸��� �޺κп� ��ġ �ϸ� �����÷ο� ������ �ذ� 
	//�ٴ� ����÷ο� ������ �߻� ���ɼ��� ������ ��κ��� ������ �����÷ο� ������ ����÷ο� ������ ����
	//�Ͼ�� ����
	//�ݿø� ���� �ذ��� ����
	const int64 pageCount = (size + (PAGE_SIZE - 1)) / PAGE_SIZE;
	//ex) (4 + 4096 - 1 )/4096 =1.xxxxx�ε� ���⼭ int -> �Ҽ� ����
	const int64 dataOffset = pageCount * PAGE_SIZE - size;//�����÷ο� ������ �ذ��ϱ����� �ǵ����� ������ġ ����
	void* baseAddress = ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	return static_cast<void*>(static_cast<int8*>(baseAddress)+dataOffset);
}

void StompAllocator::Release(void* ptr)
{
	//VirtualFree(reinterpret_cast<void*>(ptr), 0, MEM_RELEASE);
	//�����÷ο� ������ ���� �� �����͸� �ڷ� �о�����
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
