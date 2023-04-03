#pragma once
class HostSession;

using HostSessionRef = shared_ptr<HostSession>;

class HostSessionManager
{
public:
	void Add(HostSessionRef session);
	void Remove(HostSessionRef session);
	void Broadcast(SendBufferRef sendBuffer);

private:
	USE_LOCK;
	Set<HostSessionRef> _sessions;
};
extern HostSessionManager GHostSessionManager;