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
// virtual ���X, virtual ����ϸ� virtual table �� offset 0���� ���鼭 
// overlapped���õ� �޸𸮰� �ƴ� ���� �޸𸮷� ü���� �� �ֱ� ������ ���x 
// (�׷��� struct�� ���� ����ϳ� �ͱ⵵ �ϰ�?) 

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