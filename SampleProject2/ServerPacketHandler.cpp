
#include "MultiBattleScene.h"
#include "ServerPacketHandler.h"
#include "bufferReader.h"
#include "bufferWriter.h"


MultiBattleScene* multyserver;
PacketHandlerFunc server::GPacketHandler[UINT16_MAX];

// Á÷Á¢ ÄÁÅÙÃ÷ ÀÛ¾÷ÀÚ 

bool server::Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	//TODO : LOG

	return false;
}

//bool server::Handle_C_TEST(PacketSessionRef& session, protocol::C_TEST& pkt)
//{
//	return false;
//}

//bool server::Handle_C_MOVE(PacketSessionRef& session, protocol::C_MOVE& pkt)
//{
//	return false;
//}

bool server::Handle_C_CONNECT(PacketSessionRef& session, protocol::C_CONNECT& pkt)
{
	//multyserver->player2->armor = pkt.clientplayer().armor();
	//multyserver->player2->cost = pkt.clientplayer().cost();
	//multyserver->player2->hp = pkt.clientplayer().hp();
	//multyserver->player2->maxCost = pkt.clientplayer().maxcost();
	//multyserver->player2->maxHp = pkt.clientplayer().maxhp();
	//
	////host deck
	//for (auto& dklist : pkt.clientdeck().decklist())
	//	multyserver->YourDeck->DeckList.push_back(dklist);
	//for (auto& remCardList : pkt.clientdeck().remainingcardlist())
	//	multyserver->YourDeck->RemainingCardList.push_back(remCardList);
	//for (auto& hdList : pkt.clientdeck().handlist())
	//	multyserver->YourDeck->HandList.push_back(hdList);
	//for (auto& disList : pkt.clientdeck().discardlist())
	//	multyserver->YourDeck->DiscardList.push_back(disList);
	bool& connect = gpHost->SetConnect();
	connect = pkt.connect();

	return true;
}

//bool server::Handle_C_DECK(PacketSessionRef& session, protocol::C_DECK& pkt)
//{
//	return false;
//}

bool server::Handle_C_USECARD(PacketSessionRef& session, protocol::C_USECARD& pkt)
{
	multyserver->OtherPlayerUsedCardNum = pkt.usedcardnum();
	multyserver->OtherPlayerTargetEnemyNum = pkt.targetenemynum();

	return true;
}

//bool server::Handle_C_DRAWCARD(PacketSessionRef& session, protocol::C_DRAWCARD& pkt)
//{
//	multyserver->YourDeck->RemainingCardList.clear();
//	for (auto& remainCard : pkt.remainingcardlist())
//		multyserver->YourDeck->RemainingCardList.push_back(remainCard);
//
//	multyserver->YourDeck->HandList.clear();
//	for (auto& handCard : pkt.handlist())
//		multyserver->YourDeck->HandList.push_back(handCard);
//	  
//	return true;
//}

bool server::Handle_C_TURNEND(PacketSessionRef& session, protocol::C_TURNEND& pkt)
{
	//multyserver->YourDeck->DiscardList.clear();
	//for (auto& discard : pkt.discardlist())
	//	multyserver->YourDeck->DiscardList.push_back(discard);

	multyserver->clientTurnEnd = pkt.bturnend();
	    
	return true;
}

bool server::Handle_C_MOVECLICKED(PacketSessionRef& session, protocol::C_MOVECLICKED& pkt)
{
	multyserver->MoveClickCheck = pkt.bmoveclick();

	return true;
}

bool server::Handle_C_MOVETOPOINT(PacketSessionRef& session, protocol::C_MOVETOPOINT& pkt)
{
	multyserver->MoveLocation.x = pkt.vecx();
	multyserver->MoveLocation.y = pkt.vecy();
	multyserver->MoveLocation.z = pkt.vecz();

	return true;
}