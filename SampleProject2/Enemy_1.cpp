#include "Enemy_1.h"

bool Enemy_1::Init()
{
    hp = 50;
    return true;
}

void Enemy_1::patern(Player* player, int turn)
{
    switch (((turn / 2)-1) % 3) // 0, 1, 2, 0, 1, 2, ....
    {

    case 0: 
    {
        player->hp -= 5;
    }break;

    case 1:
    {
        player->hp -= 10;
    }break;

    case 2:
    {
        player->hp -= 20;
    }break;

    }

    if (player->hp < 0) player->hp = 0;
}
