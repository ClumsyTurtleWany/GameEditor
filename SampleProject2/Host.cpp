#include "Host.h"
#include "ServerPacketHandler.h"
#include "HostSession.h"
#include "HostSessionManager.h"
#include "Service.h"
#include "ThreadManager.h"

Host* gpHost;

Host::Host(std::wstring ip, int16 port, int16 sessionCount)
	:ip(ip), port(port), sessionCount(sessionCount)
{
}

bool Host::Init()
{
	ServerPacketHandler::Init();

	service = MakeShared<ServerService>(
		NetAddress(ip, port),
		MakeShared<IocpCore>(),
		MakeShared<HostSession>,
		sessionCount);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < std::thread::hardware_concurrency() / 2 - 1; i++)
	{
		GThreadManager->Launch(
			[=]()
			{
				while (1)
				{
					service->GetIocpCore()->Dispatch();
				}
			}
		);
	}

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
	return false;
}

