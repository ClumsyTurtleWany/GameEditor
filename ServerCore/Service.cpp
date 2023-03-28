#include "pch.h"
#include "Service.h"
#include"Session.h"

Service::Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount)
	:_type(type), _netAddress(address), _iocpCore(core), _sessionFactory(factory), _maxSessionCount(maxSessionCount)
{
}

Service::~Service()
{
}

void Service::CloseService()
{
	//TODO
}

SessionRef Service::CreateSession()
{
	SessionRef session = _sessionFactory();
	session->SetService(shared_from_this());
	
	if (_iocpCore->Register(session) == false)
		return nullptr;

	return session;
}

void Service::AddSession(SessionRef session)
{
	WRITE_LOCK;
	_sessionCount++;
	_sessions.insert(session);
}

void Service::ReleaseSession(SessionRef session)
{
	WRITE_LOCK;
	ASSERT_CRASH(_sessions.erase(session) != 0);
	_sessionCount--;
}

/*-----------------
	ClientService
------------------*/

ClientService::ClientService(NetAddress address, IocpCoreRef core, SessionFactory factorty, int32 maxSessionCount)
	:Service(ServiceType::Client, address, core, factorty, maxSessionCount)
{
}


bool ClientService::Start()
{
	if (CanStart() == false)
		return false;
	const int32 sessionCount = GetMaxSessionCount();
	for (int32 i = 0; i < sessionCount; i++)
	{
		SessionRef session = CreateSession();
		if (session->Connect() == false)
			return false;
	}
	return true;
}

/*-----------------
	ServerService
------------------*/

ServerService::ServerService(NetAddress address, IocpCoreRef core, SessionFactory factorty, int32 maxSessionCount)
	:Service(ServiceType::Server, address, core, factorty, maxSessionCount)
{
}

bool ServerService::Start()
{
	if (CanStart() == false)
		return  false;

	_listener = MakeShared<Listener>();
	if (_listener == nullptr)
		return false;

	//서비스의 정보를 통해 listener가 작동을 해야하서
	ServerServiceRef service = static_pointer_cast<ServerService>(shared_from_this());
	if (_listener->StartAccept(service) == false)
		return false;

	return true;
}

void ServerService::CloseService()
{
	//TOOD

	Service::CloseService();
}
