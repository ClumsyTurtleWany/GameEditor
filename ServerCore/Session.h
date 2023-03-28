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
	//������ ������� ��� �ؾ��ϴ� ����� ���ܷ� ����Ҽ��ְ�
	friend class Listener;
	friend class IocpCore;
	friend class Service;

	enum
	{
		BUFFER_SIZE = 0x10000, //64KB
	};

	//session ���ο����� service�� ���� ������ �˾ƾ� ���� ���ϵ� �ϴϱ� �ϴ��� ��������������, 
	//��ȯ ���� �����뵵�� weak_ptr�� �̿�
	weak_ptr<Service> _service;
	SOCKET			_socket = INVALID_SOCKET;
	NetAddress		_netAddress = {};
	Atomic<bool>	_connected = false;

	USE_LOCK;

	/* ���� ���� */
	RecvBuffer _recvBuffer;

	/* �۽� ���� */
	Queue<SendBufferRef> _sendQueue;
	Atomic<bool> _sendRegistered = false;

	/* IocpEvent ���� */
	RecvEvent _recvEvent;
	ConnectEvent _connectEvent;
	DisconnectEvent _disconnectEvent;
	SendEvent _sendEvent;
public:
	// TEMP : listener->acceptex ���ڿ��� �䱸�ؼ� �ӽ÷� ����
	//BYTE _recvBuffer[1000] = {};

public:
	Session();
	virtual ~Session();
public:
	void					Send(BYTE* buffer, int32 len);
	void					Send(SendBufferRef sendBuffer);
	bool					Connect();	//�������� ������ �ʿ䰡 ������
	void					Disconnect(const WCHAR* cause);
	void					SetService(shared_ptr<Service> service) { _service = service; }
	shared_ptr<Service>		GetService() { return _service.lock(); }
public:
	/* ���� ���� */
	void					SetNetAddress(NetAddress address) { _netAddress = address; }
	NetAddress				GetAddress() { return _netAddress; }
	SOCKET					GetSocket() { return _socket; }
	bool					IsConnected() { return _connected; }
	SessionRef				GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }
public:
	/* �������̽� ���� */
	virtual HANDLE			GetHandle() override;
	virtual void			Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0)override;

private:	//Listner�� ���������� register �Ǹ� process�� �����ϴ� ������� ����
	bool					RegisterConnect();
	bool					RegisterDisconnect();
	void					RegisterRecv();
	void					RegisterSend(SendEvent* sendEvent);
	void					RegisterSend();

	void					ProcessConnect();
	void					ProcessDisconnect();
	void					ProcessRecv(int32 numOfBytes);		//���� ���� ����Ʈ
	void					ProcessSend(SendEvent* sendEvent, int32 numOfBytes);		//������ ����Ʈ
	void					ProcessSend(int32 numOfBytes);		//������ ����Ʈ

	void					HandleError(int32 errorCode);
protected:
	//������ ��ӹ޾Ƽ� ����Ҷ� �ʿ��� ���
						/* ������ �ڵ忡�� �����ε� */
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
	uint16 id;	// ��������ID (ex. 1=�α���, 2=�̵���û)
};

class PacketSession : public Session
{
protected:
	virtual int32 OnRecv(BYTE* buffer, int32 len) sealed;				// ���̻� ������ �Ұ�
	virtual void OnRecvPacket(BYTE* buffer, int32 len) abstract;		// �ݵ�� ������

	PacketSessionRef GetPacketSessionRef() { return static_pointer_cast<PacketSession>(shared_from_this()); }
public:
	PacketSession();
	virtual ~PacketSession();
};