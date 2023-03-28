#include "pch.h"
#include "SocketUtils.h"

/*----------------
	SocketUtils
-----------------*/

LPFN_CONNECTEX		SocketUtils::ConnectEx = nullptr;
LPFN_DISCONNECTEX	SocketUtils::DisconnectEx = nullptr;
LPFN_ACCEPTEX		SocketUtils::AcceptEx = nullptr;

void SocketUtils::Init()
{
	WSADATA wsaData;
	ASSERT_CRASH(::WSAStartup(MAKEWORD(2, 2), OUT & wsaData) == 0);

	/* 런타임에 주소 얻어오는 API */
	SOCKET dummySocket = CreateSocket();
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisconnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));
	Close(dummySocket);
}

void SocketUtils::Clear()
{
	::WSACleanup();
}

bool SocketUtils::BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn)
{
	DWORD bytes = 0;
	return SOCKET_ERROR != ::WSAIoctl(
		socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), fn, sizeof(*fn), OUT & bytes, NULL, NULL);
	//CONNETCT, DISCONNECT, ACCEPT를 런타임에 불러오기 위한 함수 msdn참조
	//ioctlsocket기본기능 + 윈도우 비동기 connect, disconnect, accept 에 대한 추가 작업?
	//너무 광범위 해서 자세한 설명은 못하겠고, 여기서 용도는 런타임에 함수포인터에 넣는 작업정도?
}

SOCKET SocketUtils::CreateSocket()
{
	//socket함수랑 같은 놈임 msdn 찾아보자
	//socket함수는 다른 os에도 쓸 수있고, WSASocket은 윈도우에서만 쓸수있고 차이 정도만 알면 될듯(윈도우 최적화).
	return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	// Winsock의 socket() 함수는 기본적으로 소켓 속성을 오버랩드 I / O로 주긴 합니다.
	// 그러나 Asynchronous I / O를 구현 하기 위해서 BSD의 select() 뿐 아니라 Winsock에서는 윈도우즈의 메시지 메커니즘이나 
	// Win32 Event 메커니즘을 이용하는 WSAAsyncSelect 또는 WSAEventSelect를 추가로 지원하며 
	// 윈도우즈 플랫폼에서 select() 보다 더 효율적인 프로그래밍을 가능하게 합니다.
	// 즉, Winsock API는 BSD 함수보다 윈도우즈의 특성에 맞도록 보다 효율적인 성능을 내도록 하는 함수인 것이죠.
	// 다음 세번째 인자의 protocol의 경우 BSD나 Winsock이나 af가 INET(인터넷)이며, type이 SOCK_STREAM(연결지향)이면
	// protocol이 0이여도 IPPROTO_TCP로 동작됩니다.디폴트 프로토콜을 선택한다는 의미이지요.
	// 하지만, 명시해주는 것이 좋을 것 같습니다.
	// 예로, AF_INET에서 RAW 타입으로 ICMP 프로토콜을 사용한다면
	// socket(AF_INET, SOCK_RAW, IPPROTO_ICMP) 를 해주어야 합니다.
	// 프로토콜 값 및 af, type은 IPX(AF_IPX)나 적외선통신(AF_IRDA), 블루투스(AF_BTH) 등 마다
	// 각각 프로토콜 및 type이 정해져 있습니다.
}

bool SocketUtils::SetLinger(SOCKET socket, uint16 onoff, uint16 linger)
{
	LINGER option;
	option.l_onoff = onoff;
	option.l_linger = linger;
	return SetSockOpt(socket, SOL_SOCKET, SO_LINGER, option);
}

bool SocketUtils::SetReuseAddress(SOCKET socket, bool flag)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
}

bool SocketUtils::SetRecvBufferSize(SOCKET socket, int32 size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_RCVBUF, size);
}

bool SocketUtils::SetSendBufferSize(SOCKET socket, int32 size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_SNDBUF, size);
}

bool SocketUtils::SetTcpNoDelay(SOCKET socket, bool flag)
{
	//nagle 알고리즘 사용X
	return SetSockOpt(socket, SOL_SOCKET, TCP_NODELAY, flag);
}

// ListenSocket의 속성?특성?을 ClientSocket에 그대로 적용
bool SocketUtils::SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listenSocket);
}

bool SocketUtils::Bind(SOCKET socket, NetAddress netAddr)
{
	return SOCKET_ERROR != ::bind(
		socket, reinterpret_cast<const SOCKADDR*>(&netAddr.GetSockAddr()), sizeof(SOCKADDR_IN));
}

bool SocketUtils::BindAnyAddress(SOCKET socket, uint16 port)
{
	SOCKADDR_IN myAddress;
	myAddress.sin_family = AF_INET;
	myAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);
	myAddress.sin_port = ::htons(port);

	return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(&myAddress), sizeof(myAddress));
}

//backlog = 허용 인원
bool SocketUtils::Listen(SOCKET socket, int32 backlog)
{
	return SOCKET_ERROR != ::listen(socket, backlog);
}

void SocketUtils::Close(SOCKET& socket)
{
	if (socket != INVALID_SOCKET)
		::closesocket(socket);
	socket = INVALID_SOCKET;
}

