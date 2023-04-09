#pragma once

#include "NetworkPch.h"
#include "protocol.pb.h"

// return type : bool / argument : PacketSession&, BYTE*, int32 �� �Լ� ������
// ���������� �޴� ������ refcount�� �Ѱ� �� �߰����� ��������
using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;

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
	PKT_S_USECARD = 9,
	PKT_C_USECARD = 10,
	PKT_S_SHUFFLE_DECK = 11,
	PKT_C_SHUFFLE_DECK = 12,
};

namespace client
{
	// �� �Լ� �������� �迭
	extern PacketHandlerFunc GPacketHandler[UINT16_MAX];
	// TODO : �ڵ�ȭ
	// Custom Handlers
	bool Handle_INVALID(PacketSessionRef& session, BYTE * buffer, int32 len);

	//bool Handle_S_TEST(PacketSessionRef& session, protocol::S_TEST& pkt);
	bool Handle_C_TEST(PacketSessionRef& session, protocol::C_TEST& pkt);
	bool Handle_C_MOVE(PacketSessionRef& session, protocol::C_MOVE& pkt);
	bool Handle_S_TEST(PacketSessionRef& session, protocol::S_TEST& pkt);
	bool Handle_S_LOGIN(PacketSessionRef& session, protocol::S_LOGIN& pkt);
	bool Handle_S_CONNECT(PacketSessionRef& session, protocol::S_CONNECT& pkt);
	bool Handle_C_CONNECT(PacketSessionRef& session, protocol::C_CONNECT& pkt);
	bool Handle_S_DECK(PacketSessionRef& session, protocol::S_DECK& pkt);
	bool Handle_C_DECK(PacketSessionRef& session, protocol::C_DECK& pkt);
	bool Handle_S_USECARD(PacketSessionRef& session, protocol::S_USECARD& pkt);
	bool Handle_C_USECARD(PacketSessionRef& session, protocol::C_USECARD& pkt);
	bool Handle_S_SHUFFLE_DECK(PacketSessionRef& session, protocol::S_SHUFFLE_DECK& pkt);
	bool Handle_C_SHUFFLE_DECK(PacketSessionRef& session, protocol::C_SHUFFLE_DECK& pkt);
}



//class ServerPacketHandler
class ClientPacketHandler
{
public: //�ܺ� ���(����?)

	// TODO : �ڵ�ȭ
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			client::GPacketHandler[i] = client::Handle_INVALID;

		//GPacketHandler[PKT_S_TEST] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		//{return HandlePacket<protocol::S_TEST>(Handle_S_TEST, session, buffer, len); };
		client::GPacketHandler[PKT_S_TEST] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{return ClientPacketHandler::HandlePacket<protocol::S_TEST>(client::Handle_S_TEST, session, buffer, len); };
		client::GPacketHandler[PKT_S_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{return ClientPacketHandler::HandlePacket<protocol::S_LOGIN>(client::Handle_S_LOGIN, session, buffer, len); };
		client::GPacketHandler[PKT_S_CONNECT] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{return ClientPacketHandler::HandlePacket<protocol::S_CONNECT>(client::Handle_S_CONNECT, session, buffer, len); };
		client::GPacketHandler[PKT_S_DECK] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{return ClientPacketHandler::HandlePacket<protocol::S_DECK>(client::Handle_S_DECK, session, buffer, len); };
		client::GPacketHandler[PKT_S_USECARD] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{return ClientPacketHandler::HandlePacket<protocol::S_USECARD>(client::Handle_S_USECARD, session, buffer, len); };
		client::GPacketHandler[PKT_S_SHUFFLE_DECK] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{return ClientPacketHandler::HandlePacket<protocol::S_SHUFFLE_DECK>(client::Handle_S_SHUFFLE_DECK, session, buffer, len); };
	}

	// C++���� ���ø� �Լ��� ��-���ø� �Լ��� ���� �̸��� ���� �� �ֽ��ϴ�. 
	// �� ���, �Լ� ȣ�� �� �����Ϸ��� ��-���ø� �Լ��� �켱������ �����մϴ�
	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		//session : � session�� �������� Ȯ�� �뵵
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return client::GPacketHandler[header->id](session, buffer, len);
	}

	//static SendBufferRef MakeSendBuffer(protocol::S_TEST& pkt) { return MakeSendBuffer(pkt, PKT_S_TEST); }
	static SendBufferRef MakeSendBuffer(protocol::C_TEST& pkt) { return MakeSendBuffer(pkt, PKT_C_TEST); }
	static SendBufferRef MakeSendBuffer(protocol::C_MOVE& pkt) { return MakeSendBuffer(pkt, PKT_C_MOVE); }
	static SendBufferRef MakeSendBuffer(protocol::C_CONNECT& pkt) { return MakeSendBuffer(pkt, PKT_C_CONNECT); }
	static SendBufferRef MakeSendBuffer(protocol::C_DECK& pkt) { return MakeSendBuffer(pkt, PKT_C_DECK); }
	static SendBufferRef MakeSendBuffer(protocol::C_USECARD& pkt) { return MakeSendBuffer(pkt, PKT_C_USECARD); }
	static SendBufferRef MakeSendBuffer(protocol::C_SHUFFLE_DECK& pkt) { return MakeSendBuffer(pkt, PKT_C_SHUFFLE_DECK); }

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