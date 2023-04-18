#include "MultiBattleScene.h"
#include "ClientSession.h"
#include "ClientPacketHandler.h"
#include "Service.h"

ClientSession::~ClientSession()
{
#ifdef _DEBUG
	cout << "~ClientSession" << endl;
	OutputDebugString(L"~ClientSession()");
#endif // _DEBUG

}

void ClientSession::OnConnected()
{
#ifdef _DEBUG
	cout << "Connected To Server" << endl;
	OutputDebugString(L"Connected To Server");
#endif // _DEBUG
	if (gpClient != nullptr)
	{
		auto clientService = gpClient->GetClientservice();
		for(int i = 0;i<clientService->sessionsForConnect.size(); i++)
		{
			// 이 부분 햇갈리는게 맞나?
			// 이거 나중에 연결 됬을때 sessionsForConnect에 있는 sessionRef 지울려고 하는거임
			// 안되면 다시 체크 해봐야 함 
			// 4월13일 - 역시 안댐 ㅆㅂ
			if (shared_from_this() == clientService->sessionsForConnect[i])
			{
				clientService->sessionsForConnect.erase(clientService->sessionsForConnect.begin() + i);
				break;
			}
			
		}
	}
	//protocol::Player clientPlayer;
	//{
	//	clientPlayer.set_armor(multyclient->player1->armor);
	//	clientPlayer.set_cost(multyclient->player1->cost);
	//	clientPlayer.set_hp(multyclient->player1->hp);
	//	clientPlayer.set_maxcost(multyclient->player1->maxCost);
	//	clientPlayer.set_maxhp(multyclient->player1->maxHp);
	//}
	//protocol::Deck clientDeck;
	//{
	//	*clientDeck.mutable_decklist() = { multyclient->MyDeck->DeckList.begin(), multyclient->MyDeck->DeckList.end() };
	//	*clientDeck.mutable_discardlist() = { multyclient->MyDeck->DiscardList.begin(), multyclient->MyDeck->DiscardList.end() };
	//	*clientDeck.mutable_handlist() = { multyclient->MyDeck->HandList.begin(), multyclient->MyDeck->HandList.end() };
	//	*clientDeck.mutable_remainingcardlist() = { multyclient->MyDeck->RemainingCardList.begin(), multyclient->MyDeck->RemainingCardList.end() };
	//}
	//protocol::C_CONNECT clientCon;
	//{
	//	//clientCon.set_allocated_clientplayer(&clientPlayer);
	//	//clientCon.set_allocated_clientdeck(&clientDeck);
	//	clientCon.mutable_clientdeck()->CopyFrom(clientDeck);
	//	clientCon.mutable_clientplayer()->CopyFrom(clientPlayer);
	//}
	//auto sendBuffer = ClientPacketHandler::MakeSendBuffer(clientCon);
	//Send(sendBuffer);

	protocol::C_CONNECT clientCon;
	clientCon.set_connect(true);

	auto sendBuf = ClientPacketHandler::MakeSendBuffer(clientCon);
	Send(sendBuf);
}

void ClientSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	ClientPacketHandler::HandlePacket(session, buffer, len);
}

void ClientSession::OnSend(int32 len)
{
#ifdef _DEBUG
	std::wstring length = L"OnSend Len = ";
	length += to_wstring(len);
	OutputDebugString(length.c_str());
#endif // _DEBUG

}

void ClientSession::OnDisconnected()
{
#ifdef _DEBUG
	OutputDebugString(L"Disconnected");
#endif // _DEBUG
}
