#include "ClientSession.h"
#include "ClientPacketHandler.h"

ClientSession::~ClientSession()
{
#ifdef _DEBUG
	cout << "~ClientSession" << endl;
	OutputDebugString(L"~ClientSession()");
#endif // _DEBUG

}

void ClientSession::OnConnected()
{
#ifdef _DEBUG
	cout << "Connected To Server" << endl;
	OutputDebugString(L"Connected To Server");
#endif // _DEBUG

}

void ClientSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	ClientPacketHandler::HandlePacket(session, buffer, len);
}

void ClientSession::OnSend(int32 len)
{
#ifdef _DEBUG
	std::wstring length = L"OnSend Len = ";
	length += to_wstring(len);
	OutputDebugString(length.c_str());
#endif // _DEBUG

}

void ClientSession::OnDisconnected()
{
#ifdef _DEBUG
	OutputDebugString(L"Disconnected");
#endif // _DEBUG
}
