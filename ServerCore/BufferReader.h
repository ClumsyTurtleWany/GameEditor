#pragma once

/*----------------
	BufferReader
-----------------*/

class BufferReader
{
	BYTE* _buffer = nullptr;
	uint32 _size = 0;
	uint32 _pos = 0;
public:
	BufferReader();
	BufferReader(BYTE* buffer, uint32 size, uint32 pos = 0);
	~BufferReader();
public:
	BYTE* Buffer() { return _buffer; }
	uint32 Size() { return _size; }
	uint32 ReadSize() { return _pos; }
	uint32 FreeSize() { return _size - _pos; }
public:
	//해당위치에서 데이터를 꺼내 쓸 수있는지 확인
	template<typename T>
	bool Peek(T* dest) { return Peek(dest, sizeof(T)); }
	bool Peek(void* dest, uint32 len); // copy
	
	template<typename T>
	bool Read(T* dest) { return Read(dest, sizeof(T)); }
	bool Read(void* dest, uint32 len); //cut & paste

	template<typename T>
	BufferReader& operator>>(OUT T& dest);
};

template<typename T>
inline BufferReader& BufferReader::operator>>(OUT T& dest)
{
	dest = *reinterpret_cast<T*>(&_buffer[_pos]);
	_pos += sizeof(T);

	return *this;
}
