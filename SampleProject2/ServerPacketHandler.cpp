
#include "ServerPacketHandler.h"
#include "bufferReader.h"
#include "bufferWriter.h"

PacketHandlerFunc server::GPacketHandler[UINT16_MAX];

// Á÷Á¢ ÄÁÅÙÃ÷ ÀÛ¾÷ÀÚ 

bool server::Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	//TODO : LOG

	return false;
}
bool server::Handle_C_TEST(PacketSessionRef& session, protocol::C_TEST& pkt)
{
	return false;
}

bool server::Handle_S_TEST(PacketSessionRef& session, protocol::S_TEST& pkt)
{
	//TODO 

	return true;
}
bool server::Handle_C_MOVE(PacketSessionRef& session, protocol::C_MOVE& pkt)
{
	return false;
}

bool server::Handle_S_LOGIN(PacketSessionRef& session, protocol::S_LOGIN& pkt)
{
	return false;
}

bool server::Handle_S_CONNECT(PacketSessionRef& session, protocol::S_CONNECT& pkt)
{
	return false;
}

bool server::Handle_C_CONNECT(PacketSessionRef& session, protocol::C_CONNECT& pkt)
{
	return false;
}

bool server::Handle_S_DECK(PacketSessionRef& session, protocol::S_DECK& pkt)
{
	return false;
}

bool server::Handle_C_DECK(PacketSessionRef& session, protocol::C_DECK& pkt)
{
	return false;
}

bool server::Handle_S_USECARD(PacketSessionRef& session, protocol::S_USECARD& pkt)
{
	return false;
}

bool server::Handle_C_USECARD(PacketSessionRef& session, protocol::C_USECARD& pkt)
{
	return false;
}

bool server::Handle_S_SHUFFLE_DECK(PacketSessionRef& session, protocol::S_SHUFFLE_DECK& pkt)
{
	return false;
}

bool server::Handle_C_SHUFFLE_DECK(PacketSessionRef& session, protocol::C_SHUFFLE_DECK& pkt)
{
	return false;
}