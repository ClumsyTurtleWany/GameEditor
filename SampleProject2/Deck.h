#pragma once
#include <vector>
#include <algorithm>
#include <random>

enum CardNum
{
	Strike = 0,
	Defend = 1,
	PommelStrike = 2,
	ShrugItOff = 3,
	Hemokinesis = 4,
	Bludgeon = 5,
	IronWave = 6,
};

class Deck
{
public:
	Deck();

public:
		void	Draw(int num_of_draw);
		void	Shuffle();
		void	TurnEnd();
		void	Use(int handNum);
public:
	std::vector<int> DeckList;
	std::vector<int> RemainingCardList;
	std::vector<int> HandList;
	std::vector<int> DiscardList;
};

