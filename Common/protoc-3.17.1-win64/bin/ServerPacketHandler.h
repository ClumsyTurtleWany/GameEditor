#pragma once

#include "ServerPch.h"
#include "protocol.pb.h"

// return type : bool / argument : PacketSession&, BYTE*, int32 인 함수 포인터
// 참조값으로 받는 이유는 refcount를 한개 더 추가하지 않을려고
using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;

enum : uint16
{
	//PKT_S_TEST = 1,
	PKT_C_TEST = 1,
	PKT_C_MOVE = 2,
	PKT_S_TEST = 3,
	PKT_S_LOGIN = 4,
};

namespace server
{
	// 그 함수 포인터의 배열
	extern PacketHandlerFunc GPacketHandler[UINT16_MAX];
	// TODO : 자동화
	// Custom Handlers
	bool Handle_INVALID(PacketSessionRef& session, BYTE * buffer, int32 len);

	//bool Handle_S_TEST(PacketSessionRef& session, protocol::S_TEST& pkt);
	bool Handle_C_TEST(PacketSessionRef& session, protocol::C_TEST& pkt);
	bool Handle_C_MOVE(PacketSessionRef& session, protocol::C_MOVE& pkt);
	bool Handle_S_TEST(PacketSessionRef& session, protocol::S_TEST& pkt);
	bool Handle_S_LOGIN(PacketSessionRef& session, protocol::S_LOGIN& pkt);
}



//class ServerPacketHandler
class ServerPacketHandler
{
public: //외부 사용(래핑?)

	// TODO : 자동화
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			server::GPacketHandler[i] = server::Handle_INVALID;

		//GPacketHandler[PKT_S_TEST] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		//{return HandlePacket<protocol::S_TEST>(Handle_S_TEST, session, buffer, len); };
		server::GPacketHandler[PKT_C_TEST] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{return ServerPacketHandler::HandlePacket<protocol::C_TEST>(server::Handle_C_TEST, session, buffer, len); };
		server::GPacketHandler[PKT_C_MOVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{return ServerPacketHandler::HandlePacket<protocol::C_MOVE>(server::Handle_C_MOVE, session, buffer, len); };
	}

	// C++에서 템플릿 함수와 비-템플릿 함수가 같은 이름을 가질 수 있습니다. 
	// 이 경우, 함수 호출 시 컴파일러는 비-템플릿 함수를 우선적으로 선택합니다
	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		//session : 어떤 session이 보냇는지 확인 용도
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return server::GPacketHandler[header->id](session, buffer, len);
	}

	//static SendBufferRef MakeSendBuffer(protocol::S_TEST& pkt) { return MakeSendBuffer(pkt, PKT_S_TEST); }
	static SendBufferRef MakeSendBuffer(protocol::S_TEST& pkt) { return MakeSendBuffer(pkt, PKT_S_TEST); }
	static SendBufferRef MakeSendBuffer(protocol::S_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_S_LOGIN); }

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