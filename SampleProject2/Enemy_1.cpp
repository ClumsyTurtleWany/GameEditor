#include "Enemy_1.h"

bool Enemy_1::Init()
{
    hp = 50;
    maxHp = 50;
    armor = 0;
    return true;
}

void Enemy_1::patern(Player* player, int turn)
{
    switch (((turn / 2)-1) % 3) // 0, 1, 2, 0, 1, 2, ....
    {

    case 0: 
    {
        attack(player, 5);
    }break;

    case 1:
    {
        attack(player, 10);
    }break;

    case 2:
    {
        attack(player, 20);
    }break;

    }

    if (player->hp < 0) player->hp = 0;
    // 플레이어 사망 이벤트?
}
