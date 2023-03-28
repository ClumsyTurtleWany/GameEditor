#pragma once
#include"IocpCore.h"
#include"NetAddress.h"

class AcceptEvent;
class ServerService;

/*--------------
	Listener
---------------*/

class Listener : public IocpObject
{
protected:
	//listenSocket
	SOCKET _socket = INVALID_SOCKET;
	//sessionManager?
	Vector<AcceptEvent*> _acceptEvents;
	ServerServiceRef _service;
public:
	Listener() = default;
	~Listener();
public:
	/* �ܺο��� ��� */
	//bool StartAccept(NetAddress netAddress);
	bool StartAccept(ServerServiceRef netAddress);
	void CloseSocket();
public:
	/* �������̽� ���� */
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
private:
	/* ���� ���� */
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);
};

