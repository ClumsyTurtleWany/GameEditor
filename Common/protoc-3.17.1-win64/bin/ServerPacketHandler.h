#pragma once

#include "protocol.pb.h"

#define serialize 4
#define automation

#ifndef automation
enum
{
	// S_ : server to client
	// C_ : client to server 

	S_TEST = 1,
};

template<typename T, typename C>
class PacketIterator
{
	C& _container;
	uint16 _index;
public:
	PacketIterator(C& container, uint16 index) : _container(container), _index(index) {}
public:
	bool				operator!=(const PacketIterator& other) const { return _index != other._index; }
	const T& operator*() const { return _container[_index]; }
	T& operator*() { return _container[_index]; }
	T* operator->() { return &_container[_index]; }
	PacketIterator& operator++() { _index++; return *this; }
	PacketIterator		operator++(int32) { PacketIterator ret = *this; ++_index; return ret; }
};


// ���� �������� ������ġ, ���̸� �ƴϱ� ������ �ʿ� ���� �ٷ� ������ �� ���ְ� �ϴ� �뵵 
template<typename T>
class PacketList
{
	T* _data;	// ���� �ּ�
	uint16	_count;

public:
	PacketList() : _data(nullptr), _count(0) {}
	PacketList(T* data, uint16 count) : _data(data), _count(count) {}
public:
	T& operator[](uint16 index)
	{
		ASSERT_CRASH(index < _count);

		return _data[index];
	}
public:
	uint16 Count() { return _count; }
public:
	PacketIterator<T, PacketList<T>> begin() { return PacketIterator<T, PacketList<T>>(*this, 0); }
	PacketIterator<T, PacketList<T>> end() { return PacketIterator<T, PacketList<T>>(*this, _count); }
	//PacketIterator< PKT_S_TEST::BuffsListItem, PacketList< PKT_S_TEST::BuffsListItem>>
	//PacketIterator< PKT_S_TEST::BuffsListItem, PacketList< PKT_S_TEST::BuffsListItem>> tmp(*this, 0);
};

//struct BuffData
//{
//	uint64 buffID;
//	float remainTime;
//};

class ServerPacketHandler
{
public:
	static void HandlePacket(BYTE* buffer, int32 len);

#if (serialize == 1 || serialize == 2 || serialize == 3)
	static SendBufferRef Make_S_TEST(uint64 id, uint32 hp, uint16 attack, vector<BuffData> buffs, wstring name);
#elif (serialize == 4)
	//need protocol.pb.h
	static SendBufferRef MakeSendBuffer(protocol::S_TEST& pkt);
#else
	static SendBufferRef Make_S_TEST(uint64 id, uint32 hp, uint16 attack, vector<BuffData> buffs, wstring name);
#endif

};

//�޸� �е�
#pragma pack(1)
struct PKT_S_TEST
{
	// �׳� rpg ���� ������ ������
	struct BuffsListItem
	{
		uint64 buffId;
		float remainTime;

		//victim list
		uint16 victimsOffset;
		uint16 victimsCount;
	};

	//��Ŷ �ش����� ���� �����Ѵٸ�?
	uint16 packetSize;
	uint16 packID;

	uint64 id;
	uint32 hp;
	uint16 attack;

	// ���� �����͵��� ó���ϴ� �ٸ� �������
	// ���� ���� �����͵��� �Ѱ��� ���� ���ѳ���
	// ���� �������� offset, count�� �̸� �����س���
	// �׸��� offset, count�� ������ ���� ���� ������ �ڿ� �����س��� ���� �����͵��� �Ľ��ϴ� ���
	uint16 buffsOffset;	//���� �������� ������ġ
	uint16 buffsCount;

	//uint16 nameOffset;
	//uint16 nameLen;

	bool Validate() //�����ϴ�
	{
		uint32 size = 0;
		size += sizeof(PKT_S_TEST);
		if (packetSize < size)
			return false;

		size += buffsCount * sizeof(BuffsListItem);
		//size += nameLen * sizeof(WCHAR);
		if (size != packetSize)
			return false;

		//���� �������� ũ�⸦ �ѱ�� ��
		//if (buffsOffset + buffsCount * sizeof(BuffsListItem) + nameLen * sizeof(WCHAR) > packetSize)
		if (buffsOffset + buffsCount * sizeof(BuffsListItem) > packetSize)
			return false;

		return true;
	}

	//using BuffsList = PacketList<PKT_S_TEST::BuffsListItem>;
	//
	//BuffsList GetBuffsList()
	//{
	//	//PKT_S_TEST�� ù ������
	//	BYTE* data = reinterpret_cast<BYTE*>(this);
	//
	//	//ó�� ��ġ�� ���� ��ġ
	//	data += buffsOffset;
	//
	//	return BuffsList(reinterpret_cast<PKT_S_TEST::BuffsListItem*>(data), buffsCount);
	//}

	// ���� ������
	// 1) ���ڿ� (ex. name)
	// 2) �׳� ����Ʈ �迭 (ex. ��� �̹���)
	// 3) �Ϲ� ����Ʈ
	//vector<BuffData> buffs;
};

class PKT_S_TEST_WRITE
{
	PKT_S_TEST* _pkt = nullptr;
	SendBufferRef _sendBuffer;
	BufferWriter _bw;
public:
	using BuffsListItem = PKT_S_TEST::BuffsListItem;
	using BuffsList = PacketList<PKT_S_TEST::BuffsListItem>;
	using BuffVictimsList = PacketList<uint64>;
public:
	PKT_S_TEST_WRITE(uint64 id, uint32 hp, uint16 attack)
	{
		_sendBuffer = GSendBufferManager->Open(4096);
		_bw = BufferWriter(_sendBuffer->Buffer(), _sendBuffer->AllocSize());

		//���� ������ 
		_pkt = _bw.Reserve<PKT_S_TEST>();
		_pkt->packetSize = 0;				//TO FILL
		_pkt->packID = S_TEST;
		_pkt->id = id;
		_pkt->hp = hp;
		_pkt->attack = attack;
		_pkt->buffsOffset = 0;				//TO FILL
		_pkt->buffsCount = 0;				//TO FILL
	}
public:
	// ���� �����͸� ä���ִ� �κ� 
	// �ٴ� �ٸ� Ÿ���� ���� �����͸� �߰��ϸ� �޸� �󿡼� �߰��� ���̴� �κ��� �ֱ� ������
	// �̸� ������������ �����ŭ �Ҵ��ؼ� �� ������ ���� �����͸� ��� ü��� �ڿ��ٰ� �ٽ� �Ҵ��ϰ� �ϴ� ��
	BuffsList ReserveBuffList(uint16 buffCount)
	{
		BuffsListItem* firstBuffListItem = _bw.Reserve<BuffsListItem>(buffCount);

		//�׳� �� _pkt�� �ּҰ� buffsOffset�� �ּ� �ƴ�? - �̰� ���� �߰����� ���� ���� ���ݾ�
		_pkt->buffsOffset = (uint64)firstBuffListItem - (uint64)_pkt;
		_pkt->buffsCount = buffCount;

		return BuffsList(firstBuffListItem, buffCount);
	}

	BuffVictimsList ReserveBuffsVictimsList(BuffsListItem* buffsItem, uint16 victimsCount)
	{
		uint64* firstVictimsListItem = _bw.Reserve<uint64>(victimsCount);
		buffsItem->victimsOffset = (uint64)firstVictimsListItem - (uint64)_pkt;
		buffsItem->victimsCount = victimsCount;

		return BuffVictimsList(firstVictimsListItem, victimsCount);
	}

	SendBufferRef CloseAndReturn()
	{
		// ��Ŷ ������ ���
		_pkt->packetSize = _bw.WriteSize();

		_sendBuffer->Close(_bw.WriteSize());

		return _sendBuffer;
	}
};
#pragma pack()

//need protocol.pb.h
template<typename T>
SendBufferRef _MakeSendBuffer(T& pkt, uint16 pktID)
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
#else

// return type : bool / argument : PacketSession&, BYTE*, int32 �� �Լ� ������
// ���������� �޴� ������ refcount�� �Ѱ� �� �߰����� ��������
using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
// �� �Լ� �������� �迭
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	//PKT_S_TEST = 1,
	PKT_C_TEST = 1,
	PKT_C_MOVE = 2,
	PKT_S_TEST = 3,
	PKT_S_LOGIN = 4,
};

// TODO : �ڵ�ȭ
// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);

//bool Handle_S_TEST(PacketSessionRef& session, protocol::S_TEST& pkt);
bool Handle_C_TEST(PacketSessionRef& session, protocol::C_TEST& pkt);
bool Handle_C_MOVE(PacketSessionRef& session, protocol::C_MOVE& pkt);
bool Handle_S_TEST(PacketSessionRef& session, protocol::S_TEST& pkt);
bool Handle_S_LOGIN(PacketSessionRef& session, protocol::S_LOGIN& pkt);

//class ServerPacketHandler
class ServerPacketHandler
{
public: //�ܺ� ���(����?)

	// TODO : �ڵ�ȭ
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;

		//GPacketHandler[PKT_S_TEST] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		//{return HandlePacket<protocol::S_TEST>(Handle_S_TEST, session, buffer, len); };
		GPacketHandler[PKT_C_TEST] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{return HandlePacket<protocol::C_TEST>(Handle_C_TEST, session, buffer, len); };
		GPacketHandler[PKT_C_MOVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{return HandlePacket<protocol::C_MOVE>(Handle_C_MOVE, session, buffer, len); };
	}

	// C++���� ���ø� �Լ��� ��-���ø� �Լ��� ���� �̸��� ���� �� �ֽ��ϴ�. 
	// �� ���, �Լ� ȣ�� �� �����Ϸ��� ��-���ø� �Լ��� �켱������ �����մϴ�
	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		//session : � session�� �������� Ȯ�� �뵵
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
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
#endif // automation