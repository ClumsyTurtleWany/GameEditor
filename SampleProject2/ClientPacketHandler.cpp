
#include "ClientPacketHandler.h"
#include "bufferReader.h"
#include "protocol.pb.h"

	PacketHandlerFunc client::GPacketHandler[UINT16_MAX];

	bool client::Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		//TODO : LOG

		return false;
	}

	bool client::Handle_C_TEST(PacketSessionRef& session, protocol::C_TEST& pkt)
	{
		return false;
	}

	bool client::Handle_C_MOVE(PacketSessionRef& session, protocol::C_MOVE& pkt)
	{
		return false;
	}

	bool client::Handle_S_TEST(PacketSessionRef& session, protocol::S_TEST& pkt)
	{
		cout << pkt.id() << " " << pkt.hp() << " " << pkt.attack() << endl;
		cout << "BuffSize : " << pkt.buffs_size() << endl;

		for (auto& buf : pkt.buffs())
		{
			cout << "BuffInfo : " << buf.buffid() << " " << buf.remaintime() << endl;
			cout << "Victims : " << buf.victims_size() << endl;
			for (auto& vic : buf.victims())
				cout << vic << " ";
			cout << endl;
		}
		return true;
	}
	bool client::Handle_S_LOGIN(PacketSessionRef& session, protocol::S_LOGIN& pkt)
	{
		return false;
	}
