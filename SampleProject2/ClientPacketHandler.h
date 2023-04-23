#pragma once

#include "NetworkPch.h"
#include "protocol.pb.h"

class MultiBattleScene;

// return type : bool / argument : PacketSession&, BYTE*, int32 인 함수 포인터
// 참조값으로 받는 이유는 refcount를 한개 더 추가하지 않을려고
using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;

extern MultiBattleScene* multyclient;

//enum : uint16
//{
//	//PKT_S_TEST = 1,
//
//	PKT_S_CONNECT = 1,
//
//	PKT_C_CONNECT = 2,
//
//	PKT_S_USECARD = 3,
//
//	PKT_C_USECARD = 4,
//
//	PKT_S_TURNEND = 5,
//
//	PKT_C_TURNEND = 6,
//
//	PKT_S_MOVECLICKED = 7,
//
//	PKT_C_MOVECLICKED = 8,
//
//	PKT_S_MOVETOPOINT = 9,
//
//	PKT_C_MOVETOPOINT = 10,
//
//};

namespace client
{
	enum : uint16
	{
		//PKT_S_TEST = 1,
		PKT_S_CONNECT = 1,
		PKT_C_CONNECT = 2,
		PKT_S_USECARD = 3,
		PKT_C_USECARD = 4,
		PKT_S_TURNEND = 5,
		PKT_C_TURNEND = 6,
		PKT_S_MOVECLICKED = 7,
		PKT_C_MOVECLICKED = 8,
		PKT_S_MOVETOPOINT = 9,
		PKT_C_MOVETOPOINT = 10,
	};

	// 그 함수 포인터의 배열
	extern PacketHandlerFunc GPacketHandler[UINT16_MAX];
	// TODO : 자동화
	// Custom Handlers
	bool Handle_INVALID(PacketSessionRef& session, BYTE * buffer, int32 len);

	//bool Handle_S_TEST(PacketSessionRef& session, protocol::S_TEST& pkt);
	bool Handle_S_CONNECT(PacketSessionRef& session, protocol::S_CONNECT& pkt);
	bool Handle_S_USECARD(PacketSessionRef& session, protocol::S_USECARD& pkt);
	bool Handle_S_TURNEND(PacketSessionRef& session, protocol::S_TURNEND& pkt);
	bool Handle_S_MOVECLICKED(PacketSessionRef& session, protocol::S_MOVECLICKED& pkt);
	bool Handle_S_MOVETOPOINT(PacketSessionRef& session, protocol::S_MOVETOPOINT& pkt);
}



//class ServerPacketHandler
class ClientPacketHandler
{
public: //외부 사용(래핑?)

	// TODO : 자동화
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			client::GPacketHandler[i] = client::Handle_INVALID;

		//GPacketHandler[PKT_S_TEST] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		//{return HandlePacket<protocol::S_TEST>(Handle_S_TEST, session, buffer, len); };
		client::GPacketHandler[client::PKT_S_CONNECT] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{return ClientPacketHandler::HandlePacket<protocol::S_CONNECT>(client::Handle_S_CONNECT, session, buffer, len); };
		client::GPacketHandler[client::PKT_S_USECARD] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{return ClientPacketHandler::HandlePacket<protocol::S_USECARD>(client::Handle_S_USECARD, session, buffer, len); };
		client::GPacketHandler[client::PKT_S_TURNEND] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{return ClientPacketHandler::HandlePacket<protocol::S_TURNEND>(client::Handle_S_TURNEND, session, buffer, len); };
		client::GPacketHandler[client::PKT_S_MOVECLICKED] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{return ClientPacketHandler::HandlePacket<protocol::S_MOVECLICKED>(client::Handle_S_MOVECLICKED, session, buffer, len); };
		client::GPacketHandler[client::PKT_S_MOVETOPOINT] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{return ClientPacketHandler::HandlePacket<protocol::S_MOVETOPOINT>(client::Handle_S_MOVETOPOINT, session, buffer, len); };
	}

	// C++에서 템플릿 함수와 비-템플릿 함수가 같은 이름을 가질 수 있습니다. 
	// 이 경우, 함수 호출 시 컴파일러는 비-템플릿 함수를 우선적으로 선택합니다
	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		//session : 어떤 session이 보냇는지 확인 용도
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return client::GPacketHandler[header->id](session, buffer, len);
	}

	//static SendBufferRef MakeSendBuffer(protocol::S_TEST& pkt) { return MakeSendBuffer(pkt, PKT_S_TEST); }
static SendBufferRef MakeSendBuffer(protocol::C_CONNECT& pkt) { return MakeSendBuffer(pkt, client::PKT_C_CONNECT); }
static SendBufferRef MakeSendBuffer(protocol::C_USECARD& pkt) { return MakeSendBuffer(pkt, client::PKT_C_USECARD); }
static SendBufferRef MakeSendBuffer(protocol::C_TURNEND& pkt) { return MakeSendBuffer(pkt, client::PKT_C_TURNEND); }
static SendBufferRef MakeSendBuffer(protocol::C_MOVECLICKED& pkt) { return MakeSendBuffer(pkt, client::PKT_C_MOVECLICKED); }
static SendBufferRef MakeSendBuffer(protocol::C_MOVETOPOINT& pkt) { return MakeSendBuffer(pkt, client::PKT_C_MOVETOPOINT); }

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