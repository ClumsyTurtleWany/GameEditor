#include "Host.h"
#include "ServerPacketHandler.h"
#include "HostSession.h"
#include "HostSessionManager.h"
#include "Service.h"
#include "ThreadManager.h"

Host* gpHost = nullptr;

Host::Host(std::wstring ip, int16 port, int16 sessionCount)
	:ip(ip), port(port), sessionCount(sessionCount)
{
}

//Host::~Host()
//{
//}

bool Host::Init()
{
	ServerPacketHandler::Init();

	service = MakeShared<ServerService>(
		NetAddress(ip, port),
		MakeShared<IocpCore>(),
		MakeShared<HostSession>,
		sessionCount);

	//for (int32 i = 0; i < std::thread::hardware_concurrency() / 2 - 1; i++)
	for (int32 i = 0; i < 1 ; i++)	//TEST
	{
		GThreadManager->Launch([=]()
			{
				while (1)
				{
					//if (service->GetIocpCore()->Dispatch() && connecting == false)
					////if (service->GetIocpCore()->Dispatch())
					//	connecting = true;
					if (service->GetIocpCore()->Dispatch() == false)
						return false;
					//for(auto& session : service->GetSessions())
					//{
					//	if (session->IsConnected())
					//	{
					//		connecting = true;
					//		break;
					//	}
					//}
				}
			}
		);
	}
	ASSERT_CRASH(service->Start());

	return true;
}

bool Host::Frame()
{
	return false;
}

bool Host::Render()
{
	return false;
}

bool Host::Release()
{
	GThreadManager->Join();
	
	return true;
}

bool Host::CancelAccept()
{
	WRITE_LOCK;
	for (auto& acEvent : service->_listener->GetAcceptVector())
	{
		AcceptEvent* acceptEvent = acEvent;
		BOOL result = CancelIoEx((HANDLE)acceptEvent->_session->GetSocket(), static_cast<LPOVERLAPPED>(acceptEvent));
		if (!result)
		{
			int32 errorCode = GetLastError();
			if (errorCode != ERROR_NOT_FOUND)
			{
				wstring errorString = L"CancelIoEx failed with error : ";
				errorString += to_wstring(errorCode);
				OutputDebugString(errorString.c_str());
				return false;
			}
		}
	}
	for (auto& acEvent : service->_listener->GetAcceptVector())
		xdelete(acEvent);
	service->_listener->clearAceeptEvents();
	return true;
}

bool Host::IsConnected()
{
	return connecting;
}

ServerServiceRef Host::GetService()
{
	return service;
}

int16 Host::GetSessionCount()
{
	return sessionCount;
}

bool& Host::SetConnect()
{
	// TODO: 여기에 return 문을 삽입합니다.

	return connecting;
}
