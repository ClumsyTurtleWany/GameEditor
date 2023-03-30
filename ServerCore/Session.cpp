#include "pch.h"
#include "Session.h"
#include"SocketUtils.h"
#include"Service.h"

Session::Session() : _recvBuffer(BUFFER_SIZE)
{
	_socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
	SocketUtils::Close(_socket);
}

void Session::Send(BYTE* buffer, int32 len)
{
	// ������ ����
	// 1) ���� ����? 
	// 2) sendEvent ����? (����? ������? WSASend ��ø?)
	//	recv�� �޸� ���� send�Լ��� ȣ�� ���� �𸣰�, send�� ������ ȣ�� �� �������� 
	//  session���� sendEvent�� �������ִ°� ��ȿ������ �� ����.

	// TEMP
	// sendbuffer�� session�ȿ� �������� �ű�ٰ� �����ϰ� WSASend���� �Ѱ��ִ� ���?
	// ������ 1) ������ �����ִ� ������ �� �������� ���� ���� �ʰ� �� ���� ������ �̾ �����ϴ� ���, 
	// �׸��� ���� �����ϸ� ó������ ���ƿͼ� ����� ���
	// circular buffer ��� �̶�� �ϴµ� �̰� ������ ���� ����� ���� ŭ
	// 
	// �׷��� �ӽ÷� sendEvent�� sendbuffer�� ���� �ŵ��� ����

	SendEvent* sendEvent = xnew<SendEvent>();
	sendEvent->_owner = shared_from_this(); //ADD_REF
	sendEvent->buffer.resize(len);
	::memcpy(sendEvent->buffer.data(), buffer, len);

	WRITE_LOCK;
	RegisterSend(sendEvent);
}

void Session::Send(SendBufferRef sendBuffer)
{
	//���� registerSend�� �ɸ��� ���� ���¶��, �ɾ��ش�.
	//registerSend�� �ɷ��־ WSASend / sendEvent�� ����� �� ���� ��Ȳ�̶�� queue���ٰ� �ְ� �������´�.

	WRITE_LOCK;

	_sendQueue.push(sendBuffer);

	//sendQueue�� LockFreeQueue �������� �ٲ� �� ������ ������ ���ؼ� atomic�� ���
	if (_sendRegistered.exchange(true) == false)
		RegisterSend();
}

bool Session::Connect()
{
	return RegisterConnect();
}

void Session::Disconnect(const WCHAR* cause)
{
	//exchange : _connected �ȿ� false�� �ְ� �ֱ� ���� ���� ������
	if (_connected.exchange(false) == false)
		return;

	//TEMP
	wcout << "Disconnect : " << cause << endl;

	//OnDisconnected();		//������ �ڵ忡�� ������
	////SocketUtils::Close(_socket);
	//GetService()->ReleaseSession(GetSessionRef());

	RegisterDisconnect();
}

HANDLE Session::GetHandle()
{
	//�ٴ� �� ��Ĺ�� �ڵ�� �ٲ㼭 ��������?
	//�Ƹ� iocp���� ����Ʈ�� �߰��ϱ�����?

	return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	// TODO iocpEvent�� recv�� send�� ������ �װ� ó�� �ϴ� �Լ� 

	switch (iocpEvent->_type)
	{
	case EventType::Connect:
		ProcessConnect();
		break;
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		//ProcessSend(static_cast<SendEvent*>(iocpEvent), numOfBytes);
		ProcessSend(numOfBytes);
		break;
	default:
		break;
	}
}

bool Session::RegisterConnect()
{
	if (IsConnected())
		return false;

	//���� �� session�� ����Ǿ��ִ� service type�� Ŭ���̾�Ʈ ���� �Ѵ�.
	//�׷��� �ٸ� ������ ���� �� ���ִ�.
	if (GetService()->GetServiceType() != ServiceType::Client)
		return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	//�̰� ����� connectex ��� ����
	//������ ó���� ���ε常 �ؾ��Ѵٰ� �ϰ� ���� ������ ����
	if (SocketUtils::BindAnyAddress(_socket, 0/*���°�*/) == false)
		return false;

	_connectEvent.Init();
	_connectEvent._owner = shared_from_this();// ADD_REF
	
	DWORD numOfBytes = 0;
	//������ ������ �ּ�
	SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
	if (false == SocketUtils::ConnectEx(
		_socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &_connectEvent))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_connectEvent._owner = nullptr;
			return false;
		}
	}
	return true;
}

bool Session::RegisterDisconnect()
{
	_disconnectEvent.Init();
	_disconnectEvent._owner = shared_from_this(); //ADD_REF

	if (false == SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0))
	{
		int32 errorCode = ::WSAGetLastError();
		if(errorCode != WSA_IO_PENDING)
		{
			_disconnectEvent._owner = nullptr;
			return false;
		}
	}
	return true;
}

void Session::RegisterRecv()
{
	if (IsConnected() == false)
		return;

	_recvEvent.Init();

	//sesison�� �����ֱ⸦ Ȯ���ϰ� ���� �ϱ� ����, wsarecv�� ���� or ���н� release_ref
	//recv�� ���� ���� session�� ��� �־�� �ϴϱ� session�� ���� ī��Ʈ�� �ϳ� �÷��� recv�ޱ⸦ �����ϰ� ��
	_recvEvent._owner = shared_from_this(); //ADD_REf

	WSABUF wsaBuf;
	//wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer);
	//wsaBuf.len = len32(_recvBuffer);

	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
	wsaBuf.len = _recvBuffer.FreeSize();

	DWORD numOfBytes = 0;
	DWORD flags = 0;

	//overlapped����ü�� ��ӹ��� recvEvent�� WSARecv�Ҷ����� �����Ҵ� �ް� �ٹ����� �����ϴ� ������ �ص� ��
	//�������� �ߴ��� ó��

	if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, OUT & numOfBytes, OUT & flags, &_recvEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_recvEvent._owner = nullptr;
		}
	}
}

void Session::RegisterSend(SendEvent* sendEvent)
{
	if (IsConnected() == false)
		return;

	//��ģ���� sendEvent�� �־ �����ص� ������� 
	//pagelock?
	WSABUF wsaBuf;
	wsaBuf.buf = (char*)sendEvent->buffer.data();
	wsaBuf.len = (ULONG)sendEvent->buffer.size();

	DWORD numOfBytes = 0;

	//	WSASend�� ThreadSafe�Ѱ�? not Safe, �׷��� Lock ����
	//	 
	//	������ �ߺ��ؼ� ȣ������ �� : ũ�� ������ ������ ����, ȣ�������� ����� �ϴµ� iocpQueue�� ������� ���δٴ� ������ ����
	//	���� ProcessSendȣ�� ������ ���� �� ����
	//	�׸��� pending�� ���ٴ°Ŵ� Ŀ�ο� SendBUffer�� �� ���� ���̻� �������µ� ��� �������� �̴°Ŵϱ� ���� ����
	//	
	//	WSASend�� ��ͼ� �ѹ��� �����°� ����(�׷��� �ҷ��� wsabuf�� �ּҷ� �ް�, 3���� ���ڵ� ���� ������ ��Ÿ���� �Ŵϱ�)
	//	���� send�ҰŸ� ��͵д����� �ѹ��� ������ ����� ���ɻ� ���� -> ���߿�

	//send�� ���� ����� �پ���

	if (SOCKET_ERROR == ::WSASend(_socket, &wsaBuf, 1, OUT & numOfBytes, 0, sendEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			sendEvent->_owner = nullptr;
			xdelete(sendEvent);
		}
	}
}

void Session::RegisterSend()
{
	if (IsConnected() == false)
		return;

	_sendEvent.Init();
	_sendEvent._owner = shared_from_this();
	
	//���� ������ sendEvent�� ���
	{
		WRITE_LOCK;				//�ܺο��� lock�Ȱɾ������� ���
		int32 writeSize = 0;

		//	�� sendQueue�� �ִ°� SendEvent�� sendbuffer�� ������?
		//	WSASend�� �ϴ� ���� queue���ִ� sendbufferRef�� refCount�� �����ؾߵǴµ� 
		//  queue���� ������ ����(pop) refcount�� �پ��� ����� ���ɼ��� ����

		while (_sendQueue.empty() == false)
		{
			SendBufferRef sendBuffer = _sendQueue.front();
			writeSize += sendBuffer->WriteSize();

			//TODO : ����üũ 

			_sendQueue.pop();
			_sendEvent.SendBuffers.push_back(sendBuffer);
		}
	}

	//Scatter-Gather
	Vector<WSABUF> wsaBufs;
	wsaBufs.reserve(_sendEvent.SendBuffers.size());
	for (SendBufferRef sendBuffer : _sendEvent.SendBuffers)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
		wsaBuf.len = static_cast<ULONG>(sendBuffer->WriteSize());
		wsaBufs.push_back(wsaBuf);
	}

	DWORD numOfBytes = 0;
	if (SOCKET_ERROR == ::WSASend(_socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT & numOfBytes, 0, &_sendEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_sendEvent._owner = nullptr; //RELEASE_REF
			_sendEvent.SendBuffers.clear();
			_sendRegistered.store(false);
		}
	}
}

void Session::ProcessConnect()
{
	_connectEvent._owner = nullptr; // RELEASE_REF

	_connected.store(true);

	//���� ���
	GetService()->AddSession(GetSessionRef());

	//������ �ڵ忡�� �����ε�
	OnConnected();

	//���� ���
	RegisterRecv();
}

void Session::ProcessDisconnect()
{
	_disconnectEvent._owner = nullptr; // RELEASE_REF

	OnDisconnected();		//������ �ڵ忡�� ������
	GetService()->ReleaseSession(GetSessionRef());
}

void Session::ProcessRecv(int32 numOfBytes)
{
	_recvEvent._owner = nullptr;

	if (numOfBytes == 0)
	{
		Disconnect(L"Recv 0");
		return;
	}

	if (_recvBuffer.OnWrite(numOfBytes) == false)
	{
		Disconnect(L"OnWrite Overflow");
		return;
	}

	//TODO 
	int32 dataSize = _recvBuffer.DataSize();
	int32 processLen = OnRecv(_recvBuffer.ReadPos(), dataSize);
	if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false)
	{
		Disconnect(L"OnRead Overflow");
	}
	//OnRecv(_recvBuffer, numOfBytes);
	//cout << "Recv Data Len = " << numOfBytes << endl;

	// ���� ���
	RegisterRecv();
}

void Session::ProcessSend(SendEvent* sendEvent, int32 numOfBytes)
{
	sendEvent->_owner = nullptr;	//RELEASE_REF
	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	//������ �ڵ忡�� ������
	OnSend(numOfBytes);
}

void Session::ProcessSend(int32 numOfBytes)
{
	_sendEvent._owner = nullptr; //RELEASE_REF
	_sendEvent.SendBuffers.clear();

	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	//������ �ڵ忡�� ������
	OnSend(numOfBytes);

	//��Ƽ������ ȯ�濡�� ���� ���� �����ϱ⿡ ����
	WRITE_LOCK;
	if (_sendQueue.empty())
		_sendRegistered.store(false);
	else
		RegisterSend();
}

void Session::HandleError(int32 errorCode)
{
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"HandleError");
		break;
	default:
		// TODO : Log
		//�ַܼ� ����ϴ°͵� ���ؽ�Ʈ ����Ī ����� ��⶧���� 
		//�α׸� ��� ������ �ϳ� ��������� �ű⼭ ������ ��Ű�� ��.

		std::wstring errCode = L"Handle Error : " + to_wstring(errorCode);
		OutputDebugString(errCode.c_str());

		cout << "Handle Error : " << errorCode << endl;
		break;
	}
}

/*-----------------
	PacketSession
------------------*/

PacketSession::PacketSession()
{
}

PacketSession::~PacketSession()
{
}

// [size(2)][id(2)][data....][size(2)][id(2)][data....]
int32 PacketSession::OnRecv(BYTE* buffer, int32 len)
{
	int32 processLen = 0;

	while (1)
	{
		int32 dataSize = len - processLen;
		// �ּ��� ����� �Ľ��� �� �־�� �Ѵ� https://na27.tistory.com/230
		if (dataSize < sizeof(PacketHeader))
			break;

		PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
		// ����� ��ϵ� ��Ŷ ũ�⸦ �Ľ��� �� �־�� �Ѵ�
		if (dataSize < header.size)
			break;

		// ��Ŷ ���� ����
		OnRecvPacket(&buffer[processLen], header.size);

		processLen += header.size;
	}
	return processLen;
} 