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
	/* 외부에서 사용 */
	//bool StartAccept(NetAddress netAddress);
	bool StartAccept(ServerServiceRef netAddress);
	void CloseSocket();
public:
	/* 인터페이스 구현 */
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	Vector<AcceptEvent*> GetAcceptVector();
private:
	/* 수신 관련 */
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);
public:
	void addAcceptEvent();
	bool clearAceeptEvents();
};

