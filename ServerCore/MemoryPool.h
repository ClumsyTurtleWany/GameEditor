#pragma once

//�޸�Ǯ�� ������ ������ٰǴ� ���� �ٸ� ũ�⸦ ���
//https://unrealstudy.tistory.com/23
//�޸�Ǯ ������ 2������ �����޸�Ǯ�� �����޸�Ǯ�� �ִ�.
//1. ������ ���̤� �ó����� ��͵Ѳ���
//2. ���������� ���� ���ٰ��ΰ� -> �޸� ������ ���ؼ� �� ������ ��ŭ�� ��� ũ�� ����
/*-----------------
	MemoryHeader
------------------*/
//�޸𸮸� �Ҵ��Ҷ� ������� �����ֱ� ���� ���
//delete �Ҷ� �����Ҵ�� ��ü�� ũ�⸦ �˷������� �ʾҴµ� ��� ������?
//-> ���� ����� �ִ���� ó�� ��ü�� ������ ������, ���� �Ҵ�� ���� �ּҴ� ���� ������ ���� ����������

enum
{
	SLIST_ALIGNMENT = 16
};

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : public SLIST_ENTRY //������ ĳ���� ���� �����ؼ� ����������� ����
{
	int32 allocSize;
	//TODO : �ʿ��� �߰� ����

	// [MemoryHeader][Data]
	MemoryHeader(int32 size) : allocSize(size) {}

	//�޸𸮸� �޾��� ���� ����� ����
	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size);// placement new
		
		//�� �����Ͱ� �����ϴ� �ּ� ��ȯ
		return reinterpret_cast<void*>(++header);
	}

	//�޸� �� ���� �ݳ��Ҷ� ��� ����
	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;

		return header;
	}
};

/*-----------------
	MemoryPool
------------------*/
DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
	SLIST_HEADER	_header;
	int32			_allocSize = 0;		//�޸� ������
	atomic<int32>	_useCount = 0;	//�޸� Ǯ���� ����� �޸� ����(����� �뵵)
	atomic<int32>	_reserveCount = 0;

	//USE_LOCK;
	//queue<MemoryHeader*> _queue;

public:
	MemoryPool(int32 allocSize);
	~MemoryPool();
public:
	void			Push(MemoryHeader* ptr);
	MemoryHeader*	Pop();
};

