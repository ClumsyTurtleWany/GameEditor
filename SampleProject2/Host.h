#pragma once
#include"NetworkPch.h"

class Host
{
public:
	std::wstring ip;
	int16 port;
	int16 sessionCount;
	ServerServiceRef service;
	bool connecting = false;
public:
	Host(std::wstring ip, int16 port, int16 sessionCount = 1);
	//~Host();
public:
	bool Init();
	bool Frame();
	bool Render();
	bool Release();
public:
	bool CancelAccept();
	bool IsConnected();
public:
	USE_LOCK;
	ServerServiceRef GetService();
	int16 GetSessionCount();
	bool& SetConnect();
};

extern Host* gpHost;