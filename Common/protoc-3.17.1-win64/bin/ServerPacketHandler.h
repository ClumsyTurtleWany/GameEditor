#pragma once

#include "NetworkPch.h"
#include "protocol.pb.h"

class MultiBattleScene;

// return type : bool / argument : PacketSession&, BYTE*, int32 �� �Լ� ������
// ���������� �޴� ������ refcount�� �Ѱ� �� �߰����� ��������
using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;

extern MultiBattleScene* multyserver;

//enum : uint16
//{
//	//PKT_S_TEST = 1,
//
//	PKT_C_TEST = 1,
//
//	PKT_C_MOVE = 2,
//
//	PKT_S_TEST = 3,
//
//	PKT_S_LOGIN = 4,
//
//	PKT_S_CONNECT = 5,
//
//	PKT_C_CONNECT = 6,
//
//	PKT_S_DECK = 7,
//
//	PKT_C_DECK = 8,
//
//	PKT_S_DRAWCARD = 9,
//
//	PKT_C_DRAWCARD = 10,
//
//	PKT_S_USECARD = 11,
//
//	PKT_C_USECARD = 12,
//
//	PKT_S_TURNEND = 13,
//
//	PKT_C_TURNEND = 14,
//
//};

namespace server
{
	enum : uint16
	{
		//PKT_S_TEST = 1,
		PKT_C_TEST = 1,
		PKT_C_MOVE = 2,
		PKT_S_TEST = 3,
		PKT_S_LOGIN = 4,
		PKT_S_CONNECT = 5,
		PKT_C_CONNECT = 6,
		PKT_S_DECK = 7,
		PKT_C_DECK = 8,
		PKT_S_DRAWCARD = 9,
		PKT_C_DRAWCARD = 10,
		PKT_S_USECARD = 11,
		PKT_C_USECARD = 12,
		PKT_S_TURNEND = 13,
		PKT_C_TURNEND = 14,
	};

	// �� �Լ� �������� �迭
	extern PacketHandlerFunc GPacketHandler[UINT16_MAX];
	// TODO : �ڵ�ȭ
	// Custom Handlers
	bool Handle_INVALID(PacketSessionRef& session, BYTE * buffer, int32 len);

	//bool Handle_S_TEST(PacketSessionRef& session, protocol::S_TEST& pkt);
	bool Handle_C_TEST(PacketSessionRef& session, protocol::C_TEST& pkt);
	bool Handle_C_MOVE(PacketSessionRef& session, protocol::C_MOVE& pkt);
	bool Handle_C_CONNECT(PacketSessionRef& session, protocol::C_CONNECT& pkt);
	bool Handle_C_DECK(PacketSessionRef& session, protocol::C_DECK& pkt);
	bool Handle_C_DRAWCARD(PacketSessionRef& session, protocol::C_DRAWCARD& pkt);
	bool Handle_C_USECARD(PacketSessionRef& session, protocol::C_USECARD& pkt);
	bool Handle_C_TURNEND(PacketSessionRef& session, protocol::C_TURNEND& pkt);
}



//class ServerPacketHandler
class ServerPacketHandler
{
public: //�ܺ� ���(����?)

	// TODO : �ڵ�ȭ
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			server::GPacketHandler[i] = server::Handle_INVALID;

		//GPacketHandler[PKT_S_TEST] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		//{return HandlePacket<protocol::S_TEST>(Handle_S_TEST, session, buffer, len); };
		server::GPacketHandler[server::PKT_C_TEST] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{return ServerPacketHandler::HandlePacket<protocol::C_TEST>(server::Handle_C_TEST, session, buffer, len); };
		server::GPacketHandler[server::PKT_C_MOVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{return ServerPacketHandler::HandlePacket<protocol::C_MOVE>(server::Handle_C_MOVE, session, buffer, len); };
		server::GPacketHandler[server::PKT_C_CONNECT] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{return ServerPacketHandler::HandlePacket<protocol::C_CONNECT>(server::Handle_C_CONNECT, session, buffer, len); };
		server::GPacketHandler[server::PKT_C_DECK] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{return ServerPacketHandler::HandlePacket<protocol::C_DECK>(server::Handle_C_DECK, session, buffer, len); };
		server::GPacketHandler[server::PKT_C_DRAWCARD] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{return ServerPacketHandler::HandlePacket<protocol::C_DRAWCARD>(server::Handle_C_DRAWCARD, session, buffer, len); };
		server::GPacketHandler[server::PKT_C_USECARD] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{return ServerPacketHandler::HandlePacket<protocol::C_USECARD>(server::Handle_C_USECARD, session, buffer, len); };
		server::GPacketHandler[server::PKT_C_TURNEND] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{return ServerPacketHandler::HandlePacket<protocol::C_TURNEND>(server::Handle_C_TURNEND, session, buffer, len); };
	}

	// C++���� ���ø� �Լ��� ��-���ø� �Լ��� ���� �̸��� ���� �� �ֽ��ϴ�. 
	// �� ���, �Լ� ȣ�� �� �����Ϸ��� ��-���ø� �Լ��� �켱������ �����մϴ�
	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		//session : � session�� �������� Ȯ�� �뵵
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return server::GPacketHandler[header->id](session, buffer, len);
	}

	//static SendBufferRef MakeSendBuffer(protocol::S_TEST& pkt) { return MakeSendBuffer(pkt, PKT_S_TEST); }
static SendBufferRef MakeSendBuffer(protocol::S_TEST& pkt) { return MakeSendBuffer(pkt, server::PKT_S_TEST); }
static SendBufferRef MakeSendBuffer(protocol::S_LOGIN& pkt) { return MakeSendBuffer(pkt, server::PKT_S_LOGIN); }
static SendBufferRef MakeSendBuffer(protocol::S_CONNECT& pkt) { return MakeSendBuffer(pkt, server::PKT_S_CONNECT); }
static SendBufferRef MakeSendBuffer(protocol::S_DECK& pkt) { return MakeSendBuffer(pkt, server::PKT_S_DECK); }
static SendBufferRef MakeSendBuffer(protocol::S_DRAWCARD& pkt) { return MakeSendBuffer(pkt, server::PKT_S_DRAWCARD); }
static SendBufferRef MakeSendBuffer(protocol::S_USECARD& pkt) { return MakeSendBuffer(pkt, server::PKT_S_USECARD); }
static SendBufferRef MakeSendBuffer(protocol::S_TURNEND& pkt) { return MakeSendBuffer(pkt, server::PKT_S_TURNEND); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;
		return func(session, pkt);
	}
	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktID)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktID;
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};