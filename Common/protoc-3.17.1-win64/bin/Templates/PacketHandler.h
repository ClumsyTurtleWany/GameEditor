#pragma once

#include "NetworkPch.h"
#include "protocol.pb.h"

class MultiBattleScene;

// return type : bool / argument : PacketSession&, BYTE*, int32 인 함수 포인터
// 참조값으로 받는 이유는 refcount를 한개 더 추가하지 않을려고
using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;

extern MultiBattleScene* multy{{spacename}};

//enum : uint16
//{
//	//PKT_S_TEST = 1,
//{%- for pkt in parser.total_pkt %}
//	PKT_{{pkt.name}} = {{pkt.id}},
//{%- endfor %}
//};

namespace {{spacename}}
{
	enum : uint16
	{
		//PKT_S_TEST = 1,
{%- for pkt in parser.total_pkt %}
		PKT_{{pkt.name}} = {{pkt.id}},
{%- endfor %}
	};

	// 그 함수 포인터의 배열
	extern PacketHandlerFunc GPacketHandler[UINT16_MAX];
	// TODO : 자동화
	// Custom Handlers
	bool Handle_INVALID(PacketSessionRef& session, BYTE * buffer, int32 len);

	//bool Handle_S_TEST(PacketSessionRef& session, protocol::S_TEST& pkt);
{%- for pkt in parser.recv_pkt %}
	bool Handle_{{pkt.name}}(PacketSessionRef& session, protocol::{{pkt.name}}& pkt);
{%- endfor%}
}



//class ServerPacketHandler
class {{output}}
{
public: //외부 사용(래핑?)

	// TODO : 자동화
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			{{spacename}}::GPacketHandler[i] = {{spacename}}::Handle_INVALID;

		//GPacketHandler[PKT_S_TEST] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		//{return HandlePacket<protocol::S_TEST>(Handle_S_TEST, session, buffer, len); };		
{%- for pkt in parser.recv_pkt %}
		{{spacename}}::GPacketHandler[{{spacename}}::PKT_{{pkt.name}}] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{return {{output}}::HandlePacket<protocol::{{pkt.name}}>({{spacename}}::Handle_{{pkt.name}}, session, buffer, len); };
{%- endfor %}
	}

	// C++에서 템플릿 함수와 비-템플릿 함수가 같은 이름을 가질 수 있습니다. 
	// 이 경우, 함수 호출 시 컴파일러는 비-템플릿 함수를 우선적으로 선택합니다
	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		//session : 어떤 session이 보냇는지 확인 용도
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return {{spacename}}::GPacketHandler[header->id](session, buffer, len);
	}

	//static SendBufferRef MakeSendBuffer(protocol::S_TEST& pkt) { return MakeSendBuffer(pkt, PKT_S_TEST); }
{%- for pkt in parser.send_pkt %}
static SendBufferRef MakeSendBuffer(protocol::{{pkt.name}}& pkt) { return MakeSendBuffer(pkt, {{spacename}}::PKT_{{pkt.name}}); }
{%- endfor %}

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
