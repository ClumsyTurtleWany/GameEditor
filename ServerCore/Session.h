#pragma once
#include"IocpCore.h"
#include"IocpEvent.h"
#include"NetAddress.h"
#include"RecvBuffer.h"


class Service;

/*--------------
	Session
---------------*/

class Session : public IocpObject
{
	//세션을 마음대로 사용 해야하는 놈들은 예외로 사용할수있게
	friend class Listener;
	friend class IocpCore;
	friend class Service;

	enum
	{
		BUFFER_SIZE = 0x10000, //64KB
	};

	//session 내부에서도 service에 대한 정보를 알아야 끊든 뭘하든 하니까 일단은 가지고있을꺼고, 
	//순환 참조 방지용도로 weak_ptr을 이용
	weak_ptr<Service> _service;
	SOCKET			_socket = INVALID_SOCKET;
	NetAddress		_netAddress = {};
	Atomic<bool>	_connected = false;

	USE_LOCK;

	/* 수신 관련 */
	RecvBuffer _recvBuffer;

	/* 송신 관련 */
	Queue<SendBufferRef> _sendQueue;
	Atomic<bool> _sendRegistered = false;

	/* IocpEvent 재사용 */
	RecvEvent _recvEvent;
	ConnectEvent _connectEvent;
	DisconnectEvent _disconnectEvent;
	SendEvent _sendEvent;
public:
	// TEMP : listener->acceptex 인자에서 요구해서 임시로 만듬
	//BYTE _recvBuffer[1000] = {};

public:
	Session();
	virtual ~Session();
public:
	void					Send(BYTE* buffer, int32 len);
	void					Send(SendBufferRef sendBuffer);
	bool					Connect();	//서버끼리 연결할 필요가 있을때
	void					Disconnect(const WCHAR* cause);
	void					SetService(shared_ptr<Service> service) { _service = service; }
	shared_ptr<Service>		GetService() { return _service.lock(); }
public:
	/* 정보 관련 */
	void					SetNetAddress(NetAddress address) { _netAddress = address; }
	NetAddress				GetAddress() { return _netAddress; }
	SOCKET					GetSocket() { return _socket; }
	bool					IsConnected() { return _connected; }
	SessionRef				GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }
public:
	/* 인터페이스 구현 */
	virtual HANDLE			GetHandle() override;
	virtual void			Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0)override;

private:	//Listner와 마찬가지로 register 되면 process를 진행하는 방식으로 동작
	bool					RegisterConnect();
	bool					RegisterDisconnect();
	void					RegisterRecv();
	void					RegisterSend(SendEvent* sendEvent);
	void					RegisterSend();

	void					ProcessConnect();
	void					ProcessDisconnect();
	void					ProcessRecv(int32 numOfBytes);		//전송 받은 바이트
	void					ProcessSend(SendEvent* sendEvent, int32 numOfBytes);		//전송한 바이트
	void					ProcessSend(int32 numOfBytes);		//전송한 바이트

	void					HandleError(int32 errorCode);
protected:
	//세션을 상속받아서 사용할때 필요한 놈들
						/* 컨텐츠 코드에서 오버로딩 */
	virtual void			OnConnected() {}
	virtual void			OnDisconnected() {}
	virtual int32			OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void			OnSend(int32 len) {}
};

/*-----------------
	PacketSession
------------------*/

struct PacketHeader
{
	uint16 size;
	uint16 id;	// 프로토콜ID (ex. 1=로그인, 2=이동요청)
};

class PacketSession : public Session
{
protected:
	virtual int32 OnRecv(BYTE* buffer, int32 len) sealed;				// 더이상 재정의 불가
	virtual void OnRecvPacket(BYTE* buffer, int32 len) abstract;		// 반드시 재정의

	PacketSessionRef GetPacketSessionRef() { return static_pointer_cast<PacketSession>(shared_from_this()); }
public:
	PacketSession();
	virtual ~PacketSession();
};