#pragma once

/*-----------------
	SendBuffer
-------------------*/
//���� chunk���� �ڸ� ������ ����ϴ� �� 

class SendBuffer : public enable_shared_from_this<SendBuffer>
{
	//Vector<BYTE>	_buffer;
	BYTE* _buffer;
	int32 _allocSize = 0;
	int32			_writeSize = 0;
	SendBufferChunkRef _owner;
public:
	//SendBuffer(int32 bufferSize);
	SendBuffer(SendBufferChunkRef owner, BYTE* buffer, int32 allocSize);
	~SendBuffer();
public:
	//BYTE*	Buffer() { return _buffer.data(); }
	BYTE*	Buffer() { return _buffer; }
	int32	WriteSize() { return _writeSize; }
	int32	AllocSize() { return _allocSize; }
	//int32	Capacity() { return static_cast<int32>(_buffer.size()); }
	void Close(uint32 writeSize);
public:
	//void	CopyData(void* data, int32 len);
};

/*--------------------
	SendBufferChunk
--------------------*/
//ū �޸𸮸� �̸� �Ҵ�ް� �߶� ���� ���

class SendBufferChunk : public enable_shared_from_this<SendBufferChunk>
{
	enum
	{
		SEND_BUFFER_CHUNK_SIZE = 0x10000
	};
	Array<BYTE, SEND_BUFFER_CHUNK_SIZE> _buffer = {}; // BYTE _buffer[SEND_BUFFER_CHUNK_SIZE] = {};
	bool								_open = false;
	uint32								_usedSize = 0;
public:
	SendBufferChunk();
	~SendBufferChunk();
public:
	void			Reset();
	SendBufferRef	Open(uint32 allocSize);
	void			Close(uint32 writeSize);
public:
	bool IsOpen() { return _open; }
	BYTE* Buffer() { return &_buffer[_usedSize]; }
	uint32	FreeSize() { return static_cast<uint32>(_buffer.size()) - _usedSize; }
};

/*---------------------
	SendBufferManager
----------------------*/

class SendBufferManager
{
	USE_LOCK;
	Vector<SendBufferChunkRef> _sendBufferChunks; //�پ� ûũ ��͵д�
private:
	SendBufferChunkRef Pop();
	void Push(SendBufferChunkRef buffer);

	//����Ʈ������ delete �ڸ��� �ɹ� �Լ��� ���� �� ��� static
	static void PushGlobal(SendBufferChunk* buffer);
public:
	SendBufferRef Open(uint32 size);
};