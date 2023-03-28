#pragma once

#include"Types.h"
#include"MemoryPool.h"

template<typename Type>
class ObjectPool
{
	static int32		s_allocSize;
	static MemoryPool	s_pool;

public:
	template<typename... Args>
	static Type* Pop(Args&&... args)
	{
#ifdef _STOMP
		MemoryHeader* ptr = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(s_allocSize));
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(ptr, s_allocSize));
#else
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(s_pool.Pop(), s_allocSize));
#endif // _STOMP
		new(memory)Type(forward<Args>(args)...);

		return memory;
	}
	
	static void Push(Type* obj)
	{
		obj->~Type();
#ifdef _STOMP
		StompAllocator::Release(MemoryHeader::DetachHeader(obj));
#else
		s_pool.Push(MemoryHeader::DetachHeader(obj));
#endif //_STOMP
	}
	//static shared_ptr<Type> MakeShared()
	//{
	//	shared_ptr<Type> ptr = { Pop(), Push };
	//	return ptr;
	//}
	template<typename... Args>
	static shared_ptr<Type> MakeShared(Args&&... args)
	{
		shared_ptr<Type> ptr = { Pop(forward<Args>(args)...), Push };
		return ptr;
	}
};

template<typename Type>
int32 ObjectPool<Type>::s_allocSize = sizeof(Type) + sizeof(MemoryHeader);
template<typename Type>
MemoryPool ObjectPool<Type>::s_pool{s_allocSize};