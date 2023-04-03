#include "NetworkPch.h"
#include "HostSession.h"
#include "HostSessionManager.h"
#include "ServerPacketHandler.h"

void HostSession::OnConnected()
{
	GHostSessionManager.Add(static_pointer_cast<HostSession>(shared_from_this()));
}

void HostSession::OnDisconnected()
{
	GHostSessionManager.Remove(static_pointer_cast<HostSession>(shared_from_this()));
}

void HostSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	ServerPacketHandler::HandlePacket(session, buffer, len);
}

void HostSession::OnSend(int32 len)
{
}
