#include "NetworkPch.h"
#include "HostSession.h"
#include "HostSessionManager.h"

HostSessionManager GHostSessionManager;

void HostSessionManager::Add(HostSessionRef session)
{
	WRITE_LOCK;
	_sessions.insert(session);
}

void HostSessionManager::Remove(HostSessionRef session)
{
	WRITE_LOCK;
	_sessions.erase(session);
}

void HostSessionManager::Broadcast(SendBufferRef sendBuffer)
{
	WRITE_LOCK;
	for (HostSessionRef session : _sessions)
		session->Send(sendBuffer);
}
