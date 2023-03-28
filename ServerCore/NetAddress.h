#pragma once

/*--------------
	NetAddress
---------------*/

class NetAddress
{
	SOCKADDR_IN _sockAddr = {};
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN sockAddr);
	NetAddress(wstring ip, uint16 port);
public:
	SOCKADDR_IN& GetSockAddr() { return _sockAddr; }
	wstring GetIpAddress();
	uint16 GetPort() { return ::ntohs(_sockAddr.sin_port); }
public:
	static IN_ADDR	Ip2Address(const WCHAR* ip);
};

