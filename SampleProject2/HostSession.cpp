#include "MultiBattleScene.h"
#include "NetworkPch.h"
#include "HostSession.h"
#include "HostSessionManager.h"
#include "ServerPacketHandler.h"


void HostSession::OnConnected()
{
	GHostSessionManager.Add(static_pointer_cast<HostSession>(shared_from_this()));

	//protocol::S_CONNECT hostCon;
	//{
	//	protocol::Player hostPlayer;
	//	{
	//		hostPlayer.set_armor(multyserver->player1->armor);
	//		hostPlayer.set_cost(multyserver->player1->cost);
	//		hostPlayer.set_hp(multyserver->player1->hp);
	//		hostPlayer.set_maxcost(multyserver->player1->maxCost);
	//		hostPlayer.set_maxhp(multyserver->player1->maxHp);
	//	}
	//	protocol::Deck hostDeck;
	//	{
	//		*hostDeck.mutable_decklist() = { multyserver->MyDeck->DeckList.begin(), multyserver->MyDeck->DeckList.end() };
	//		*hostDeck.mutable_discardlist() = { multyserver->MyDeck->DiscardList.begin(), multyserver->MyDeck->DiscardList.end() };
	//		*hostDeck.mutable_handlist() = { multyserver->MyDeck->HandList.begin(), multyserver->MyDeck->HandList.end() };
	//		*hostDeck.mutable_remainingcardlist() = { multyserver->MyDeck->RemainingCardList.begin(),multyserver->MyDeck->RemainingCardList.end() };
	//	}
	//	//hostCon.set_allocated_hostplayer(&hostPlayer);
	//	//hostCon.set_allocated_hostdeck(&hostDeck);
	//	hostCon.mutable_hostplayer()->CopyFrom(hostPlayer);
	//	hostCon.mutable_hostdeck()->CopyFrom(hostDeck);
	//}
	//auto sendBuffer = ServerPacketHandler::MakeSendBuffer(hostCon);
	//Send(sendBuffer);

	protocol::S_CONNECT hostCon;
	hostCon.set_connect(true);

	auto sendBuf = ServerPacketHandler::MakeSendBuffer(hostCon);
	Send(sendBuf);
}

void HostSession::OnDisconnected()
{
	GHostSessionManager.Remove(static_pointer_cast<HostSession>(shared_from_this()));
}

void HostSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	ServerPacketHandler::HandlePacket(session, buffer, len);
}

void HostSession::OnSend(int32 len)
{
}

void HostSession::SendDrawnDeck()
{
	//protocol::S_DRAWCARD hostDraw;
	//*hostDraw.mutable_remainingcardlist() = { multyserver->MyDeck->RemainingCardList.begin(), multyserver->MyDeck->RemainingCardList.end() };
	//*hostDraw.mutable_handlist() = { multyserver->MyDeck->HandList.begin(), multyserver->MyDeck->HandList.end() };
	//*hostDraw.mutable_discardlist() = { multyserver->MyDeck->DiscardList.begin(), multyserver->MyDeck->DiscardList.end() };
	//
	//auto sendBuffer = ServerPacketHandler::MakeSendBuffer(hostDraw);
	//Send(sendBuffer);
}
