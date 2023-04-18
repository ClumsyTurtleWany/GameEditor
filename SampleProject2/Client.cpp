#include "Client.h"
#include "ClientPacketHandler.h"
#include "ClientSession.h"
#include "Service.h"
#include "ThreadManager.h"

Client* gpClient = nullptr;

Client::Client(std::wstring ip, int16 port, int16 sessionCount)
	:ip(ip),port(port),sessionCount(sessionCount)
{
}

bool Client::Init()
{
	ClientPacketHandler::Init();

	service = MakeShared<ClientService>(
		NetAddress(ip, port),
		MakeShared<IocpCore>(),
		MakeShared<ClientSession>, // TODO : SessionManager µî
		sessionCount);

	//for (int32 i = 0; i < std::thread::hardware_concurrency() / 2 - 1; i++)
	for (int32 i = 0; i < 1; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					//if (service->GetIocpCore()->Dispatch() && connecting == false)
					////if (service->GetIocpCore()->Dispatch())
					//	connecting = true;
					if (service->GetIocpCore()->Dispatch() == false)
						return false;
					//for (auto& session : service->GetSessions())
					//{
					//	if (session->IsConnected())
					//	{
					//		connecting = true;
					//		break;
					//	}
					//}
				}
			});
	}

	ASSERT_CRASH(service->Start());

	return true;
}

bool Client::Frame()
{
	return false;
}

bool Client::Render()
{
	return false;
}

bool Client::Release()
{
	GThreadManager->Join();
	return false;
}

bool Client::IsConnected()
{
	return connecting;
}

bool Client::CancelConnect()
{
	WRITE_LOCK;
	//for (auto& session : service->sessionsForConnect)
	for (auto& session : service->GetSessions())
	{
		ConnectEvent& connectEvent = session->GetConnectEvent();
		BOOL result = CancelIoEx((HANDLE)session->GetSocket(), static_cast<LPOVERLAPPED>(&connectEvent));
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
	//for (auto& session : service->sessionsForConnect)
	//	session->GetConnectEvent()._owner = nullptr; 
	//service->sessionsForConnect.clear();

	return true;
}

bool& Client::SetConnect()
{
	return connecting;
}
