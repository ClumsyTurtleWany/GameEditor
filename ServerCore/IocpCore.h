#pragma once

/*----------------
	IocpObject
-----------------*/
//iocp�� ��� ������ ������Ʈ
//getqueuedcompletionstatus�� key������ �Ѱ��� ������Ʈ

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
