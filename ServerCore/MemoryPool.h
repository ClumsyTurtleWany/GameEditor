#pragma once

//메모리풀을 여러개 만들어줄건대 각각 다른 크기를 담당
//https://unrealstudy.tistory.com/23
//메모리풀 종류는 2가지로 고정메모리풀과 가변메모리풀이 있다.
//1. 동일한 데이ㅌ ㅓ끼리만 모와둘껀가
//2. 유동적으로 관리 해줄것인가 -> 메모리 사이즈 정해서 그 사이즈 만큼은 어느 크기 같이
/*-----------------
	MemoryHeader
------------------*/
//메모리를 할당할때 디버깅을 도와주기 위한 헤더
//delete 할때 동적할당된 객체의 크기를 알려주지도 않았는데 어떻게 지우지?
//-> 지금 만들고 있는헤더 처럼 객체의 사이즈 얼마인지, 힙에 할당된 다음 주소는 뭔지 정보를 같이 가지고있음

enum
{
	SLIST_ALIGNMENT = 16
};

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : public SLIST_ENTRY //포인터 캐스팅 까지 생각해서 상속형식으로 변경
{
	int32 allocSize;
	//TODO : 필요한 추가 정보

	// [MemoryHeader][Data]
	MemoryHeader(int32 size) : allocSize(size) {}

	//메모리를 받아준 다음 헤더를 껴줌
	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size);// placement new
		
		//실 데이터가 시작하는 주소 반환
		return reinterpret_cast<void*>(++header);
	}

	//메모리 다 사용시 반납할때 헤더 추출
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
	int32			_allocSize = 0;		//메모리 사이즈
	atomic<int32>	_useCount = 0;	//메모리 풀에서 뱉어준 메모리 개수(디버그 용도)
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

