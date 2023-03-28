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

	// �޸� ũ�� <-> �޸� Ǯ ����ȭ
	// O(1) ������ ã�� ���� ���̺�
	MemoryPool* _poolTable[MAX_ALLOC_SIZE + 1];
	//0~4096 ����Ʈ �ȿ� �ش��ϸ� �� ����Ʈ�� �´� ���̺��� �ּҰ��� ã��

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
	//�̰ű����� �ϸ� �Ҵ縸 ���ְ� ������ ȣ���� �Ƚ�����
	//Type* memory = static_cast<Type*>(BaseAllocator::Alloc(sizeof(Type)));
	//Type* memory = static_cast<Type*>(xAlloc(sizeof(Type)));
	Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));
	
	//operator new = ��ü�� �����ڸ� ȣ��
	//���ø����� �����ϱ� � �����ڸ� ȣ������ ���� �̰����� �� �޴´� ��� ����
	new(memory)Type(forward<Args>(args)...);
	//new (buf) int(3); buf�� intŸ�� 3�� �ִ´�
	//forward : �������� L-Value���� R-Value���� Ȯ��
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