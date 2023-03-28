#pragma once

/*----------------
	IocpObject
-----------------*/
//iocp에 등록 가능한 오브젝트
//getqueuedcompletionstatus의 key값으로 넘겨줄 오브젝트

class IocpObject : public enable_shared_from_this<IocpObject>
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) abstract;
};

/*--------------
	IocpCore
---------------*/

class IocpCore
{
	HANDLE _iocpHandle;
public:
	IocpCore();
	~IocpCore();
public:
	HANDLE GetHandle() { return _iocpHandle; }
	//bool Register(class IocpObject* iocpObject);
	bool Register(IocpObjectRef iocpObject);
	bool Dispatch(uint32 timeoutMs = INFINITE);
};

//TEMP
extern IocpCore GIocpCore;
