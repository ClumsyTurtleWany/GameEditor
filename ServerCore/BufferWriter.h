#pragma once

/*----------------
	BufferWriter
-----------------*/

class BufferWriter
{
	BYTE* _buffer = nullptr;
	uint32 _size = 0;
	uint32 _pos = 0;
public:
	BufferWriter();
	BufferWriter(BYTE* buffer, uint32 size, uint32 pos = 0);
	~BufferWriter();
public:
	BYTE* Buffer() { return _buffer; }
	uint32 Size() { return _size; }
	uint32 WriteSize() { return _pos; }
	uint32 FreeSize() { return _size - _pos; }
public:
	template<typename T>
	//T* Reserve();
	T* Reserve(uint16 count = 1);

	template<typename T>
	bool Write(T* dest) { return Write(dest, sizeof(T)); }
	bool Write(void* dest, uint32 len); // copy

	template<typename T>
	BufferWriter& operator<<(OUT const T& src);

	template<typename T>
	BufferWriter& operator<<(OUT T&& src);
};

//template<typename T>
//inline T* BufferWriter::Reserve()
//{
//	if(FreeSize() < sizeof(T))
//		return nullptr;
//
//	T* ret = reinterpret_cast<T*>(&_buffer[_pos]);
//	_pos += sizeof(T);
//
//	return ret;
//}

template<typename T>
inline T* BufferWriter::Reserve(uint16 count)
{
	if (FreeSize() < (sizeof(T) * count))
		return nullptr;

	T* ret = reinterpret_cast<T*>(&_buffer[_pos]);
	_pos += sizeof(T) * count;

	return ret;
}

//template<typename T>
//inline BufferWriter& BufferWriter::operator<<(OUT const T& src)
//{
//	*reinterpret_cast<T*>(&_buffer[_pos]) = src;
//	_pos += sizeof(T);
//
//	return *this;
//}

//template<typename T>
//inline BufferWriter& BufferWriter::operator<<(OUT T&& src)
//{
//	*reinterpret_cast<T*>(&_buffer[_pos]) = std::move(src);
//	_pos += sizeof(T);
//
//	return *this;
//}

/*수정*/
//템플릿 Lvalue참조 perfect forwarding?
template<typename T>
inline BufferWriter& BufferWriter::operator<<(OUT T&& src)
{
	using DataType = std::remove_reference_t<T>;
	*reinterpret_cast<DataType*>(&_buffer[_pos]) = std::forward<DataType>(src);
	_pos += sizeof(T);

	return *this;
}
