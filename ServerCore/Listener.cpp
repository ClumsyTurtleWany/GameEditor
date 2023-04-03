#include "pch.h"
#include "Listener.h"
#include"SocketUtils.h"
#include"IocpEvent.h"
#include"Session.h"
#include"Service.h"

/*--------------
	Listener
---------------*/

Listener::~Listener()
{
	SocketUtils::Close(_socket);

	for (AcceptEvent* acceptEvent : _acceptEvents)
	{
		//TODO

		xdelete(acceptEvent);
	}
}

//bool Listener::StartAccept(NetAddress netAddress)
bool Listener::StartAccept(ServerServiceRef service)
{
	/*Add*/_service = service;
	_socket = SocketUtils::CreateSocket();

	if (_socket == INVALID_SOCKET)
		return false;

	//if (GIocpCore.Register(this) == false)
	//	return false;
	if (_service->GetIocpCore()->Register(shared_from_this()) == false)
		return false;

	//���� �ּҰ� ��ó�� ������ �ȶߴ� ������ �߻� �����뵵
	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketUtils::SetLinger(_socket, 0, 0) == false)
		return false;

	//if (SocketUtils::Bind(_socket, netAddress) == false)
	//	return false;
	if (SocketUtils::Bind(_socket, _service->GetNetAddress()) == false)
		return false;

	if (SocketUtils::Listen(_socket) == false)
		return false;

	//const int32 acceptCount = 1;
	const int32 acceptCount = _service->GetMaxSessionCount();
	for (int32 i = 0; i < acceptCount; i++)
	{
		AcceptEvent* acceptEvent = xnew<AcceptEvent>();

		//acceptEvent->owner = shared_ptr<IocpObject>(this); 
		//�̷��� �ϸ� ����ī��Ʈ�� 1�� shared_ptr�ϳ� �����ϴ°��� 
		//acceptex�� �����Ų���� listener�ڱ� �ڽ��̴ϱ� ���⼭ acceptevent�� owner�� listener�ڱ��ڽ�

		/*�߰�*/acceptEvent->_owner = shared_from_this();
		_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}

	return true;
}

void Listener::CloseSocket()
{
	SocketUtils::Close(_socket);
}

HANDLE Listener::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	//ASSERT_CRASH(iocpEvent->GetType() == EventType::Accept);
	ASSERT_CRASH(iocpEvent->_type == EventType::Accept);
	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	ProcessAccept(acceptEvent);
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	//Session* session = xnew<Session>();
	//SessionRef session = MakeShared<Session>();
	SessionRef session = _service->CreateSession(); //serverservice���� �مf��

	acceptEvent->Init();
	//acceptEvent->SetSession(session);
	acceptEvent->_session = session;

	DWORD bytesRecevied = 0;
	//ó���� ���� acceptex�� ��� ���
	//Ŭ��� ���õ� ��� ������ session���� ����
	//								listensocket, clientsocket							,������ġ
	if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(), session->_recvBuffer.WritePos(), 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & bytesRecevied, static_cast<LPOVERLAPPED>(acceptEvent)))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != ERROR_IO_PENDING)
		{
			// pending���°� �ƴѵ� ������ ������ �ϴ� ����, �ϴ� �ٽ� Accept �ɾ��ش� 
			// accept�� �����Ҷ� ���� �޴´ٴ°ǰ�?

			RegisterAccept(acceptEvent);
		}
	}
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
	//Session* session = acceptEvent->GetSession();
	SessionRef session = acceptEvent->_session;

	//listnsocket�� clientsocket�� ��Ĺ �ɼ� ����ȭ
	if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _socket))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN sockAddress;
	int32 sizeOfSockAddr = sizeof(SOCKADDR_IN);
	//getpeername���� ��� ������ client�� ������ ����
	//https://www.joinc.co.kr/w/man/2/getpeername

	if (SOCKET_ERROR == ::getpeername(
		session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	//acceptevnet�� �ִ� session�� client�ּ� �߰�
	session->SetNetAddress(NetAddress(sockAddress));

	cout << "client Connected" << endl;
	OutputDebugString(L"client Connected");

	//TODO
	session->ProcessConnect();

	//������ �޾ƿ� 
	RegisterAccept(acceptEvent);
}
