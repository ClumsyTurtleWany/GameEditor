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
	// 생각할 문제
	// 1) 버퍼 관리? 
	// 2) sendEvent 관리? (단일? 여러개? WSASend 중첩?)
	//	recv와 달리 언제 send함수가 호출 될지 모르고, send를 여러게 호출 할 수있으니 
	//  session에서 sendEvent를 가지고있는게 비효율적일 수 있음.

	// TEMP
	// sendbuffer를 session안에 만들어놔서 거기다가 저장하고 WSASend에다 넘겨주는 방식?
	// 문제점 1) 기존에 남아있는 영역이 꽉 찰때까지 덮어 쓰지 않고 그 다음 영역에 이어서 저장하는 방식, 
	// 그리고 끝에 도달하면 처음으로 돌아와서 덮어쓰는 방식
	// circular buffer 방식 이라고 하는데 이거 단점은 복사 비용이 조금 큼
	// 
	// 그래서 임시로 sendEvent에 sendbuffer를 만들어서 거따가 넣음

	SendEvent* sendEvent = xnew<SendEvent>();
	sendEvent->_owner = shared_from_this(); //ADD_REF
	sendEvent->buffer.resize(len);
	::memcpy(sendEvent->buffer.data(), buffer, len);

	WRITE_LOCK;
	RegisterSend(sendEvent);
}

void Session::Send(SendBufferRef sendBuffer)
{
	//현재 registerSend가 걸리지 않은 상태라면, 걸어준다.
	//registerSend가 걸려있어서 WSASend / sendEvent를 사용할 수 없는 상황이라면 queue에다가 넣고 빠져나온다.

	WRITE_LOCK;

	_sendQueue.push(sendBuffer);

	//sendQueue를 LockFreeQueue 형식으로 바꿀 수 있으니 안전을 위해서 atomic을 사용
	if (_sendRegistered.exchange(true) == false)
		RegisterSend();
}

bool Session::Connect()
{
	return RegisterConnect();
}
void Session::Disconnect(const WCHAR* cause)
{
	//exchange : _connected 안에 false를 넣고 넣기 전의 값을 리턴함
	if (_connected.exchange(false) == false)
		return;

	//TEMP
	wcout << "Disconnect : " << cause << endl;

	//OnDisconnected();		//컨텐츠 코드에서 재정의
	////SocketUtils::Close(_socket);
	//GetService()->ReleaseSession(GetSessionRef());

	RegisterDisconnect();
}

HANDLE Session::GetHandle()
{
	//근대 왜 소캣을 핸들로 바꿔서 리턴하지?
	//아마 iocp감시 리스트에 추가하기위해?

	return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	// TODO iocpEvent가 recv나 send로 들어오면 그걸 처리 하는 함수 

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

	//현재 이 session과 연결되어있는 service type이 클라이언트 여야 한다.
	//그래야 다른 서버에 연결 할 수있다.
	if (GetService()->GetServiceType() != ServiceType::Client)
		return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	//이거 해줘야 connectex 사용 가능
	//이유는 처음에 바인드만 해야한다고 하고 딱히 설명이 없어
	if (SocketUtils::BindAnyAddress(_socket, 0/*남는거*/) == false)
		return false;

	//_connectEvent.Init();
	//_connectEvent._owner = shared_from_this();// ADD_REF
	//
	//DWORD numOfBytes = 0;
	////연결할 서버의 주소
	//SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
	//
	//if (false == SocketUtils::ConnectEx(
	//	_socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &_connectEvent))
	//{
	//	int32 temp = GetService()->GetIocpCore()->forReturnErrorCode;
	//	int32 errorCode = ::WSAGetLastError();
	//	if (errorCode != WSA_IO_PENDING)
	//	{
	//		_connectEvent._owner = nullptr;
	//		return false;
	//	}
	//}

	GThreadManager->Launch([&]()
		{
			WRITE_LOCK;
			do
			{
				_connectEvent.Init();
				_connectEvent._owner = shared_from_this();// ADD_REF

				DWORD numOfBytes = 0;
				//연결할 서버의 주소
				SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
				if(false == SocketUtils::ConnectEx(_socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &_connectEvent))
				{
					int32 temp = GetService()->GetIocpCore()->forReturnErrorCode;
					int32 errorCode = ::WSAGetLastError();
					//if (errorCode == WSAEINVAL)
					//	continue;
					if (errorCode != WSA_IO_PENDING)
					{
						_connectEvent._owner = nullptr;
						return false;
					}
				}
				this_thread::sleep_for(2s);
			} while (GetService()->GetIocpCore()->forReturnErrorCode == ERROR_CONNECTION_REFUSED);
		});


	return true;
}

bool Session::RegisterDisconnect()
{
	_disconnectEvent.Init();
	_disconnectEvent._owner = shared_from_this(); //ADD_REF

	if (false == SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
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

	//sesison의 생명주기를 확실하게 관리 하기 위해, wsarecv가 성공 or 실패시 release_ref
	//recv를 받을 동안 session이 살아 있어야 하니까 session의 참조 카운트를 하나 늘려서 recv받기를 보장하게 함
	_recvEvent._owner = shared_from_this(); //ADD_REf

	WSABUF wsaBuf;
	//wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer);
	//wsaBuf.len = len32(_recvBuffer);

	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
	wsaBuf.len = _recvBuffer.FreeSize();

	DWORD numOfBytes = 0;
	DWORD flags = 0;

	//overlapped구조체를 상속받은 recvEvent를 WSARecv할때마다 동적할당 받고 다받으면 삭제하는 식으로 해도 됨
	//선생님이 했던거 처럼

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

	//이친구도 sendEvent에 넣어서 관리해도 상관없음 
	//pagelock?
	WSABUF wsaBuf;
	wsaBuf.buf = (char*)sendEvent->buffer.data();
	wsaBuf.len = (ULONG)sendEvent->buffer.size();

	DWORD numOfBytes = 0;

	//	WSASend가 ThreadSafe한가? not Safe, 그래서 Lock 걸자
	//	 
	//	여러번 중복해서 호출했을 때 : 크게 문제가 되지는 않음, 호출순서대로 가기는 하는데 iocpQueue에 순서대로 쌓인다는 보장이 없음
	//	따라서 ProcessSend호출 순서가 섞일 수 있음
	//	그리고 pending이 떳다는거는 커널에 SendBUffer가 꽉 차서 더이상 못보내는데 계속 보낼려고 미는거니까 좋지 않음
	//	
	//	WSASend는 모와서 한번에 보내는게 좋음(그렇게 할려고 wsabuf는 주소로 받고, 3번쨰 인자도 보낼 개수를 나타내는 거니까)
	//	따라서 send할거를 모와둔다음에 한번에 보내는 방식이 성능상 유리 -> 나중에

	//send는 구현 방식이 다양함

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

	//보낼 데이터 sendEvent에 등록
	{
		WRITE_LOCK;				//외부에서 lock안걸어줬을때 대비
		int32 writeSize = 0;

		//	왜 sendQueue에 있는걸 SendEvent에 sendbuffer에 넣을까?
		//	WSASend를 하는 순간 queue에있는 sendbufferRef의 refCount를 유지해야되는데 
		//  queue에서 뺴내는 순간(pop) refcount가 줄어들어 사라질 가능성이 존재

		while (_sendQueue.empty() == false)
		{
			SendBufferRef sendBuffer = _sendQueue.front();
			writeSize += sendBuffer->WriteSize();

			//TODO : 예외체크 

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

	//세션 등록
	GetService()->AddSession(GetSessionRef());

	//컨텐츠 코드에서 오버로딩
	OnConnected();

	//수신 등록
	RegisterRecv();
}

void Session::ProcessDisconnect()
{
	_disconnectEvent._owner = nullptr; // RELEASE_REF

	OnDisconnected();		//컨텐츠 코드에서 재정의
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

	// 수신 등록
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

	//컨텐츠 코드에서 재정의
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

	//컨텐츠 코드에서 재정의
	OnSend(numOfBytes);

	//멀티쓰레드 환경에서 동시 접근 가능하기에 막음
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
		//콘솔로 출력하는것도 컨텍스트 스위칭 비용이 들기때문에 
		//로그만 찍는 스레드 하나 만든다음에 거기서 실행을 시키긴 함.

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
		// 최소한 헤더는 파싱할 수 있어야 한다 https://na27.tistory.com/230
		if (dataSize < sizeof(PacketHeader))
			break;

		PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
		// 헤더에 기록된 패킷 크기를 파싱할 수 있어야 한다
		if (dataSize < header.size)
			break;

		// 패킷 조립 성공
		OnRecvPacket(&buffer[processLen], header.size);

		processLen += header.size;
	}
	return processLen;
}