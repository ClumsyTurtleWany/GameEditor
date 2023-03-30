#pragma once
#include"NetworkPch.h"
class Client
{
		std::wstring ip;
		int16 port;
		int16 sessionCount;
		ServerServiceRef service;
	public:
		Client(std::wstring ip, int16 port, int16 sessionCount = 1);
	public:
		bool Init();
		bool Frame();
		bool Render();
		bool Release();
};
extern Client* gpClient;
