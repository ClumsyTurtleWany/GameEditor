#include "Enemy_1.h"
#include "AnimationComponent.h"

bool Enemy_1::Init()
{
    hp = 50;
    maxHp = 50;
    armor = 0;
    return true;
}

void Enemy_1::patern(Player* player, int turn)
{
    auto PAnime = player->chara->GetComponent<AnimationComponent>();
    auto EAnime = chara->GetComponent<AnimationComponent>();

    switch (((turn / 2)-1) % 3) // 0, 1, 2, 0, 1, 2, ....
    {

    case 0: 
    {
        attack(player, 5);
        PAnime->SetClipByName(L"Stomach_Hit");
        EAnime->SetClipByName(L"Kick");
    }break;

    case 1:
    {
        attack(player, 10);
        PAnime->SetClipByName(L"Stomach_Hit");
        EAnime->SetClipByName(L"Kick");
    }break;

    case 2:
    {
        attack(player, 20);
        PAnime->SetClipByName(L"Stomach_Hit");
        EAnime->SetClipByName(L"Kick");
    }break;

    }

    if (player->hp < 0) player->hp = 0;
    // 플레이어 사망 이벤트?
}
