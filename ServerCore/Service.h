#pragma once
#include"NetAddress.h"
#include"IocpCore.h"
#include"Listener.h"
#include<functional>

enum class ServiceType : uint8
{
	Server,
	Client
};

/*-------------
	Service
--------------*/
//manager 역할인가?

using SessionFactory = function<SessionRef(void)>;

class Service : public enable_shared_from_this<Service>
{
protected:
	USE_LOCK;
	ServiceType _type;
	NetAddress _netAddress = {};
	IocpCoreRef _iocpCore;

	Set<SessionRef> _sessions;
	int32 _sessionCount = 0;
	int32 _maxSessionCount = 0;
	SessionFactory _sessionFactory; //세션을 생성해주는 함수 포인터
public:
	Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~Service();
public:
	virtual bool Start() abstract;
	bool CanStart() { return _sessionFactory != nullptr; }

	virtual void CloseService();
	void SetSessionFactory(SessionFactory func) { _sessionFactory = func; }

	SessionRef CreateSession();
	void AddSession(SessionRef session);
	void ReleaseSession(SessionRef session);
	int32 GetCurrentSessionCount() { return _sessionCount; }
	int32 GetMaxSessionCount() { return _maxSessionCount; }
public:
	ServiceType GetServiceType() { return _type; }
	NetAddress GetNetAddress() { return _netAddress; }
	IocpCoreRef& GetIocpCore() { return _iocpCore; }
	Set<SessionRef>& GetSessions() { return _sessions; }
};

/*-----------------
	ClientService
------------------*/

class ClientService :public Service
{
public:
	Vector<SessionRef> sessionsForConnect;
public:
	ClientService(NetAddress address, IocpCoreRef core, SessionFactory factorty, int32 maxSessionCount = 1);
	virtual ~ClientService() {}

	virtual bool Start() override;
	bool Reconnect();
};

/*-----------------
	ServerService
------------------*/

class ServerService : public Service
{
public:
	ListenerRef _listener = nullptr;
public:
	ServerService(NetAddress address, IocpCoreRef core, SessionFactory factorty, int32 maxSessionCount = 1);
	virtual ~ServerService() {}
public:
	virtual bool Start() override;
	virtual void CloseService() override;
};