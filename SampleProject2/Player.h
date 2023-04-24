#pragma once
#include <map>
#include <string>
#include "Character.h"

class Player
{
public:
	int maxHp;
	int hp;
	int armor;
	std::map<std::wstring, int> stateMap;

	int maxCost = 3;
	int cost;

public:
	Character* chara;
	Vector3	 Forward;
};

