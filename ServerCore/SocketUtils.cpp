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

	/* ��Ÿ�ӿ� �ּ� ������ API */
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
	//CONNETCT, DISCONNECT, ACCEPT�� ��Ÿ�ӿ� �ҷ����� ���� �Լ� msdn����
	//ioctlsocket�⺻��� + ������ �񵿱� connect, disconnect, accept �� ���� �߰� �۾�?
	//�ʹ� ������ �ؼ� �ڼ��� ������ ���ϰڰ�, ���⼭ �뵵�� ��Ÿ�ӿ� �Լ������Ϳ� �ִ� �۾�����?
}

SOCKET SocketUtils::CreateSocket()
{
	//socket�Լ��� ���� ���� msdn ã�ƺ���
	//socket�Լ��� �ٸ� os���� �� ���ְ�, WSASocket�� �����쿡���� �����ְ� ���� ������ �˸� �ɵ�(������ ����ȭ).
	return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	// Winsock�� socket() �Լ��� �⺻������ ���� �Ӽ��� �������� I / O�� �ֱ� �մϴ�.
	// �׷��� Asynchronous I / O�� ���� �ϱ� ���ؼ� BSD�� select() �� �ƴ϶� Winsock������ ���������� �޽��� ��Ŀ�����̳� 
	// Win32 Event ��Ŀ������ �̿��ϴ� WSAAsyncSelect �Ǵ� WSAEventSelect�� �߰��� �����ϸ� 
	// �������� �÷������� select() ���� �� ȿ������ ���α׷����� �����ϰ� �մϴ�.
	// ��, Winsock API�� BSD �Լ����� ���������� Ư���� �µ��� ���� ȿ������ ������ ������ �ϴ� �Լ��� ������.
	// ���� ����° ������ protocol�� ��� BSD�� Winsock�̳� af�� INET(���ͳ�)�̸�, type�� SOCK_STREAM(��������)�̸�
	// protocol�� 0�̿��� IPPROTO_TCP�� ���۵˴ϴ�.����Ʈ ���������� �����Ѵٴ� �ǹ�������.
	// ������, ������ִ� ���� ���� �� �����ϴ�.
	// ����, AF_INET���� RAW Ÿ������ ICMP ���������� ����Ѵٸ�
	// socket(AF_INET, SOCK_RAW, IPPROTO_ICMP) �� ���־�� �մϴ�.
	// �������� �� �� af, type�� IPX(AF_IPX)�� ���ܼ����(AF_IRDA), �������(AF_BTH) �� ����
	// ���� �������� �� type�� ������ �ֽ��ϴ�.
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
	//nagle �˰��� ���X
	return SetSockOpt(socket, SOL_SOCKET, TCP_NODELAY, flag);
}

// ListenSocket�� �Ӽ�?Ư��?�� ClientSocket�� �״�� ����
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

//backlog = ��� �ο�
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

