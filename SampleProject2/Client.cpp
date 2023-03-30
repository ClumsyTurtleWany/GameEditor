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

	ClientServiceRef service = MakeShared<ClientService>(
		NetAddress(ip, port),
		MakeShared<IocpCore>(),
		MakeShared<ClientSession>, // TODO : SessionManager µî
		sessionCount);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < std::thread::hardware_concurrency() / 2 - 1; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

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
