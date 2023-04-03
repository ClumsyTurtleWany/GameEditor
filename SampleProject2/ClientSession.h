#pragma once
#include"NetworkPch.h"
#include"Session.h"

class ClientSession : public PacketSession
{
public:
	~ClientSession();
public:
	virtual void OnConnected() override;
	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;
	virtual void OnDisconnected() override;
};

