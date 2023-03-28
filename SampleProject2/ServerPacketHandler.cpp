
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
