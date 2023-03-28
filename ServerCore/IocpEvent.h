#pragma once

class Session;

enum class EventType : uint8
{
	Connect,
	DisconnectEvent,
	Accept,
	//PreRecv,
	Recv,
	Send
};

/*--------------
	IocpEvent
---------------*/
// virtual 사용X, virtual 사용하면 virtual table 이 offset 0번에 들어가면서 
// overlapped관련된 메모리가 아닌 따른 메모리로 체워질 수 있기 때문에 사용x 
// (그래서 struct로 많이 사용하나 싶기도 하고?) 

class IocpEvent : public OVERLAPPED
{
//protected:
public:
	EventType		_type;
	IocpObjectRef	_owner;
public:
	IocpEvent(EventType type);
	void Init();
	EventType GetType() { return _type; }
};

/*----------------
	ConnectEvent
-----------------*/

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) {}

};
/*--------------------
	DisconnectEvent
--------------------*/

class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() : IocpEvent(EventType::DisconnectEvent) {}

};

/*----------------
	AcceptEvent
-----------------*/

class AcceptEvent : public IocpEvent
{
public:
	SessionRef _session = nullptr;
public:
	AcceptEvent() : IocpEvent(EventType::Accept) {}
public:
	//void		SetSession(Session* session) { _session = session; }
	//Session*	GetSession() { return _session; }
};

/*----------------
	RecvEvent
-----------------*/

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) { }
};

/*----------------
	SendEvent
-----------------*/

class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) { }

	//TEMP
	Vector<BYTE> buffer;
	Vector<SendBufferRef> SendBuffers;
};