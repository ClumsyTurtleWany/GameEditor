#pragma once

#include"Session.h"

class HostSession : public PacketSession
{
public:
	~HostSession()
	{
#ifdef _DEBUG
		cout << "~GameSession()" << endl;
#endif // _DEBUG
	}
	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;
};

