#include "Deck.h"

Deck::Deck()
{
	DeckList.push_back(Strike);
	DeckList.push_back(Strike);
	DeckList.push_back(Strike);
	DeckList.push_back(Defend);
	DeckList.push_back(Defend);
	DeckList.push_back(Defend);
	DeckList.push_back(PommelStrike);
	DeckList.push_back(IronWave);
	DeckList.push_back(ShrugItOff);


	for (auto obj : DeckList)
	{
		RemainingCardList.push_back(obj);
	}

	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(RemainingCardList.begin(), RemainingCardList.end(), g);
}

void Deck::Draw(int num_of_draw)
{
	for (int i = 0; i < num_of_draw; i++)
	{
		if (RemainingCardList.empty() == true)
		{
			Shuffle();
		}

		int drawed_card = RemainingCardList.back();
		HandList.push_back(drawed_card);
		RemainingCardList.pop_back();
	}
}

void Deck::Shuffle()
{
	for (auto obj : DiscardList)
	{
		RemainingCardList.push_back(obj);
	}

	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(RemainingCardList.begin(), RemainingCardList.end(), g);

	DiscardList.clear();
}

void Deck::TurnEnd()
{	for (auto obj : HandList) { DiscardList.push_back(obj); }

	HandList.clear();
}

void Deck::Use(int handNum)
{
	DiscardList.push_back(HandList[handNum]);
	HandList.erase(HandList.begin() + handNum);
}
