#include "BaseEnemy.h"

bool BaseEnemy::Init()
{
	return true;
}

void BaseEnemy::patern(Player* player, int turn)
{
}

void BaseEnemy::attack(Player* player, int damage)
{
	if (player->armor > damage) { player->armor -= damage; }

	else
	{
		player->hp += (player->armor - damage); 
		player->armor = 0;
	}
}
