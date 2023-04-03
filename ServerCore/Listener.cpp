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

	//종종 주소가 겹처서 서버가 안뜨는 문제가 발생 방지용도
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
		//이렇게 하면 참조카운트가 1인 shared_ptr하나 생성하는거임 
		//acceptex를 실행시킨놈이 listener자기 자신이니까 여기서 acceptevent의 owner는 listener자기자신

		/*추가*/acceptEvent->_owner = shared_from_this();
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
	SessionRef session = _service->CreateSession(); //serverservice에서 바꿧네

	acceptEvent->Init();
	//acceptEvent->SetSession(session);
	acceptEvent->_session = session;

	DWORD bytesRecevied = 0;
	//처음에 만든 acceptex를 계속 사용
	//클라와 관련된 모든 정보를 session에서 관리
	//								listensocket, clientsocket							,시작위치
	if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(), session->_recvBuffer.WritePos(), 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & bytesRecevied, static_cast<LPOVERLAPPED>(acceptEvent)))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != ERROR_IO_PENDING)
		{
			// pending상태가 아닌데 에러가 떳으면 일단 문제, 일단 다시 Accept 걸어준다 
			// accept를 성공할때 까지 받는다는건가?

			RegisterAccept(acceptEvent);
		}
	}
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
	//Session* session = acceptEvent->GetSession();
	SessionRef session = acceptEvent->_session;

	//listnsocket과 clientsocket의 소캣 옵션 동기화
	if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _socket))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN sockAddress;
	int32 sizeOfSockAddr = sizeof(SOCKADDR_IN);
	//getpeername으로 방금 접속한 client의 정보를 추출
	//https://www.joinc.co.kr/w/man/2/getpeername

	if (SOCKET_ERROR == ::getpeername(
		session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	//acceptevnet에 있는 session에 client주소 추가
	session->SetNetAddress(NetAddress(sockAddress));

	cout << "client Connected" << endl;
	OutputDebugString(L"client Connected");

	//TODO
	session->ProcessConnect();

	//다음꺼 받아요 
	RegisterAccept(acceptEvent);
}
