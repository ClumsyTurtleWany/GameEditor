#include "pch.h"
#include "SendBuffer.h"

/*-----------------
	SendBuffer
-------------------*/

//SendBuffer::SendBuffer(int32 bufferSize)
//{
//	_buffer.resize(bufferSize);
//}

SendBuffer::SendBuffer(SendBufferChunkRef owner, BYTE* buffer, int32 allocSize)
	:_owner(owner),_buffer(buffer),_allocSize(allocSize)
{
}

SendBuffer::~SendBuffer()
{
}

//void SendBuffer::CopyData(void* data, int32 len)
//{
//	ASSERT_CRASH(Capacity() >= len);
//	::memcpy(_buffer.data(), data, len);
//	_writeSize = len;
//}

void SendBuffer::Close(uint32 writeSize)
{
	ASSERT_CRASH(_allocSize >= writeSize);
	_writeSize = writeSize;
	_owner->Close(writeSize);

}

/*--------------------
	SendBufferChunk
--------------------*/
//TLS���� �����帶�� �ܵ����� �����ִ°Ŷ� lock�� �� �ʿ������ ����

SendBufferChunk::SendBufferChunk()
{
}

SendBufferChunk::~SendBufferChunk()
{
}

void SendBufferChunk::Reset()
{
	_open = false;
	_usedSize = 0;
}

SendBufferRef SendBufferChunk::Open(uint32 allocSize)
{
	ASSERT_CRASH(allocSize <= SEND_BUFFER_CHUNK_SIZE);
	ASSERT_CRASH(_open == false);

	if (allocSize > FreeSize())
		return nullptr;

	_open = true;

	// �� ObjectPool�� �̿�? -> sendbufferchunk�� GMemory���� �Ҵ�޾Ƽ� ���, 
	// �Ȱ��� ObjectPool�� GMemory���� �Ҵ��ؼ� ����ϱ⿡ ���� ������ �Ҵ�޴´ٰ� ����

	return ObjectPool<SendBuffer>::MakeShared(shared_from_this(), Buffer(), allocSize);
}

void SendBufferChunk::Close(uint32 writeSize)
{
	ASSERT_CRASH(_open == true);
	_open = false;
	_usedSize += writeSize;
}

/*---------------------
	SendBufferManager
----------------------*/

SendBufferChunkRef SendBufferManager::Pop()
{
	{
		WRITE_LOCK;
		if (_sendBufferChunks.empty() == false)
		{
			SendBufferChunkRef sendBufferChunk = _sendBufferChunks.back();
			_sendBufferChunks.pop_back();
			return sendBufferChunk;
		}
	}
	return SendBufferChunkRef(xnew<SendBufferChunk>(),PushGlobal);
}

void SendBufferManager::Push(SendBufferChunkRef buffer)
{
	WRITE_LOCK;
	_sendBufferChunks.push_back(buffer);
}

void SendBufferManager::PushGlobal(SendBufferChunk* buffer)
{
	GSendBufferManager->Push(SendBufferChunkRef(buffer, PushGlobal));
}

//�̸� ũ�� �Ҵ��� ������ size��ŭ�� ���� ���� ����ϰԾ�
SendBufferRef SendBufferManager::Open(uint32 size)
{
	//TLS�� �־ �ڽŸ��� chunk �ִ��� Ȯ��
	if (LSendBufferChunk == nullptr)
	{
		LSendBufferChunk = Pop();		//WRITE_LOCK
		LSendBufferChunk->Reset();
	}
	
	ASSERT_CRASH(LSendBufferChunk->IsOpen() == false);

	if (LSendBufferChunk->FreeSize() < size)
	{
		LSendBufferChunk = Pop();
		LSendBufferChunk->Reset();
	}
	wstring freeSize = std::to_wstring(LSendBufferChunk->FreeSize());
	OutputDebugString(freeSize.c_str());
	cout << "FREE : " << LSendBufferChunk->FreeSize() << endl;

	return LSendBufferChunk->Open(size);
}
