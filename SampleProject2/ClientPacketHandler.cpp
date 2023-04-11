#include"MultiBattleScene.h"
#include "ClientPacketHandler.h"
#include "bufferReader.h"
#include "protocol.pb.h"
#include "Client.h"

MultiBattleScene* multyclient;
PacketHandlerFunc client::GPacketHandler[UINT16_MAX];

bool client::Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	//TODO : LOG

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

bool client::Handle_S_CONNECT(PacketSessionRef& session, protocol::S_CONNECT& pkt)
{
	//host player
	multyclient->player2->armor = pkt.hostplayer().armor();
	multyclient->player2->cost = pkt.hostplayer().cost();
	multyclient->player2->hp = pkt.hostplayer().hp();
	multyclient->player2->maxCost = pkt.hostplayer().maxcost();
	multyclient->player2->maxHp = pkt.hostplayer().maxhp();

	//host deck
	for (auto& dklist : pkt.hostdeck().decklist())
		multyclient->YourDeck->DeckList.push_back(dklist);
	for (auto& remCardList : pkt.hostdeck().remainingcardlist())
		multyclient->YourDeck->RemainingCardList.push_back(remCardList);
	for (auto& hdList : pkt.hostdeck().handlist())
		multyclient->YourDeck->HandList.push_back(hdList);
	for (auto& disList : pkt.hostdeck().discardlist())
		multyclient->YourDeck->DiscardList.push_back(disList);

	return true;
}

bool client::Handle_S_DECK(PacketSessionRef& session, protocol::S_DECK& pkt)
{
	return false;
}

bool client::Handle_S_USECARD(PacketSessionRef& session, protocol::S_USECARD& pkt)
{
	return false;
}

bool client::Handle_S_DRAWCARD(PacketSessionRef& session, protocol::S_DRAWCARD& pkt)
{
	multyclient->YourDeck->RemainingCardList.clear();
	for (auto& remainCard : pkt.remainingcardlist())
		multyclient->YourDeck->RemainingCardList.push_back(remainCard);

	multyclient->YourDeck->HandList.clear();
	for (auto& handCard : pkt.handlist())
		multyclient->YourDeck->HandList.push_back(handCard);

	multyclient->YourDeck->DiscardList.clear();
	for (auto& discard : pkt.discardlist())
		multyclient->YourDeck->DiscardList.push_back(discard);

	return true;
}

bool client::Handle_S_TURNEND(PacketSessionRef& session, protocol::S_TURNEND& pkt)
{
	multyclient->YourDeck->DiscardList.clear();
	for (auto& discard : pkt.discardlist())
		multyclient->YourDeck->DiscardList.push_back(discard);

	return true;
}