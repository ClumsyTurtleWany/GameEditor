#include "Enemy_1.h"
#include "AnimationComponent.h"

bool Enemy_1::Init()
{
    BaseEnemy::Init();

    maxHp = 50;
    hp = maxHp;
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
        PAnime->SetClipByName(L"Hit");
        EAnime->SetClipByName(L"Kick");
    }break;

    case 1:
    {
        attack(player, 10);
        PAnime->SetClipByName(L"Hit");
        EAnime->SetClipByName(L"Kick");
    }break;

    case 2:
    {
        attack(player, 20);
        PAnime->SetClipByName(L"Hit");
        EAnime->SetClipByName(L"Kick");
    }break;

    }

    if (player->hp < 0) player->hp = 0;
}

void Enemy_1::SetIntentObj(int turn, WidgetObject* image, WidgetObject* intent1, WidgetObject* intent2)
{
    switch ((((turn+1) / 2) - 1) % 3) 
    {
    case 0: 
    {
        //image->m_pCutInfoList[0]->tc = IntentImageList[0];
        intent1->m_pCutInfoList[0]->tc = NumberTextureList_Red[0];
        intent2->m_pCutInfoList[0]->tc = NumberTextureList_Red[5];
    }break;

    case 1:
    {
        //image->m_pCutInfoList[0]->tc = IntentImageList[0];
        intent1->m_pCutInfoList[0]->tc = NumberTextureList_Red[1];
        intent2->m_pCutInfoList[0]->tc = NumberTextureList_Red[0];
    }break;

    case 2:
    {
        //image->m_pCutInfoList[0]->tc = IntentImageList[0];
        intent1->m_pCutInfoList[0]->tc = NumberTextureList_Red[2];
        intent2->m_pCutInfoList[0]->tc = NumberTextureList_Red[0];
    }break;
    }
}


bool Enemy_2::Init()
{
    BaseEnemy::Init();

    maxHp = 30;
    hp = maxHp;
    armor = 0;
    return true;
}

void Enemy_2::patern(Player* player, int turn)
{
    auto PAnime = player->chara->GetComponent<AnimationComponent>();
    auto EAnime = chara->GetComponent<AnimationComponent>();

    switch (((turn / 2) - 1) % 3) // 0, 1, 2, 0, 1, 2, ....
    {

    case 0:
    {
        attack(player, 4);
        PAnime->SetClipByName(L"Hit");
        EAnime->SetClipByName(L"Attack");
    }break;

    case 1:
    {
        attack(player, 6);
        PAnime->SetClipByName(L"Hit");
        EAnime->SetClipByName(L"Attack");
    }break;

    case 2:
    {
        attack(player, 12);
        PAnime->SetClipByName(L"Hit");
        EAnime->SetClipByName(L"Attack");
    }break;

    }

    if (player->hp < 0) player->hp = 0;
}

void Enemy_2::SetIntentObj(int turn, WidgetObject* image, WidgetObject* intent1, WidgetObject* intent2)
{
    switch ((((turn + 1) / 2) - 1) % 3)
    {
    case 0:
    {
        //image->m_pCutInfoList[0]->tc = IntentImageList[0];
        intent1->m_pCutInfoList[0]->tc = NumberTextureList_Red[0];
        intent2->m_pCutInfoList[0]->tc = NumberTextureList_Red[4];
    }break;

    case 1:
    {
        //image->m_pCutInfoList[0]->tc = IntentImageList[0];
        intent1->m_pCutInfoList[0]->tc = NumberTextureList_Red[0];
        intent2->m_pCutInfoList[0]->tc = NumberTextureList_Red[6];
    }break;

    case 2:
    {
        //image->m_pCutInfoList[0]->tc = IntentImageList[0];
        intent1->m_pCutInfoList[0]->tc = NumberTextureList_Red[1];
        intent2->m_pCutInfoList[0]->tc = NumberTextureList_Red[2];
    }break;
    }
}
