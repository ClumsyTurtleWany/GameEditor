#pragma once

#include"Session.h"

class HostSession : public PacketSession
{
public:
	~HostSession()
	{
#ifdef _DEBUG
		cout << "~GameSession()" << endl;
		OutputDebugString(L"~GameSession()");
#endif // _DEBUG
	}
	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;
public:
	// TODO : host가 draw했을때 변경된 deck 보낼 패킷 만들기
	void SendDrawnDeck();
};

