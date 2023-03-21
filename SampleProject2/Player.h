#pragma once
#include <map>
#include <string>

class Player
{
public:
	int maxHp;
	int hp;
	int armor;
	std::map<std::wstring, int> stateMap;

	int maxCost = 3;
	int cost;
	// Charactor* chara;
};

