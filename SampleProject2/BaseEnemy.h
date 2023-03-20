#pragma once
#include "Player.h"

class BaseEnemy
{	
	virtual bool Init();
	virtual void patern(Player* player, int turn);

public:
	int hp;
	// Character* chara;
};

