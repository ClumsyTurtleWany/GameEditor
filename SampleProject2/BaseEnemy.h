#pragma once
#include "Player.h"
#include "Character.h"
class BaseEnemy
{	
public:
	virtual bool Init();
	virtual void patern(Player* player, int turn);
	virtual void attack(Player* player, int damage);

public:
	int hp;
	int maxHp;
	int armor;
	Character* chara;
};

