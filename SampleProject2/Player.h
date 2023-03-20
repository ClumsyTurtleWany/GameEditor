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
	// Charactor* chara;
};

