#pragma once
#include"NetworkPch.h"
class Client
{
	std::wstring ip;
	int16 port;
	int16 sessionCount;
	ClientServiceRef service;
	bool connecting = false;
public:
	Client(std::wstring ip, int16 port, int16 sessionCount = 1);
public:
	bool Init();
	bool Frame();
	bool Render();
	bool Release();
	bool IsConnected();
public:
	USE_LOCK;
	bool CancelConnect();
	ClientServiceRef& GetClientservice() { return service; }
	bool& SetConnect();
};
extern Client* gpClient;
