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

int Enemy_1::patern(Player* player, int turn)
{
    auto PAnime = player->chara->GetComponent<AnimationComponent>();
    auto EAnime = chara->GetComponent<AnimationComponent>();
    int  damage = 0;

    bool canAttack = false;
    Vector3 ToPlayer = player->chara->Transform->Translation - chara->Transform->Translation;
    float Distance = ToPlayer.Length();
    if (Distance <= 20.0f) { canAttack = true; }


    switch (((turn / 2)-1) % 3) // 0, 1, 2, 0, 1, 2, ....
    {

    case 0: 
    {
        if (canAttack)
        {
            attackCalculate(player, 5);
            damage = 5;
            PAnime->SetClipByName(L"Hit");
            EAnime->SetClipByName(L"Kick");
            //EAnime->SetClipByName(L"Attack");
        }
    }break;

    case 1:
    {
        if (canAttack)
        {
            attackCalculate(player, 10);
            damage = 10;
            PAnime->SetClipByName(L"Hit");
            EAnime->SetClipByName(L"Kick");
            //EAnime->SetClipByName(L"Attack");
        }
    }break;

    case 2:
    {
        if (canAttack)
        {
            attackCalculate(player, 15);
            damage = 15;
            PAnime->SetClipByName(L"Hit");
            EAnime->SetClipByName(L"Kick");
            //EAnime->SetClipByName(L"Attack");
        }
    }break;

    }

    if (player->hp < 0) 
    {
        player->hp = 0;
        player->isDead = true;
    }
    doAction = true;
    return  damage;
}

void Enemy_1::move(Player* player, int turn)
{
    Vector3 ToPlayer = player->chara->Transform->Translation - chara->Transform->Translation;
    float Distance = ToPlayer.Length();
    ToPlayer.Normalize();

    // 플레이어랑 거리가 20 미만일 경우, 안움직임
    if (Distance < 20.0f) { doMove = true; }
    // 거리가 20 이상 120 미만일 경우, 19.9까지 움직임(혹시 미묘한 오차땜에 안때릴까봐)
    else if (Distance >= 20.0f && Distance < 120.0f) 
    {
        chara->MoveTo(chara->Transform->Translation + ToPlayer * (Distance - 19.9));
        doMove = true;
    }
    // 거리가 120 이상일 경우, 100만큼만 움직임
    else 
    {
        chara->MoveTo(chara->Transform->Translation + ToPlayer * 100.0f);
        doMove = true;
    }
    // doMove = true; // 아 걍 여따가 한번만넣을걸ㅋㅋ
}

void Enemy_1::action(Player* player, int turn)
{
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
        intent1->m_pCutInfoList[0]->tc = NumberTextureList_Red[1];
        intent2->m_pCutInfoList[0]->tc = NumberTextureList_Red[5];
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

int Enemy_2::patern(Player* player, int turn)
{
    auto PAnime = player->chara->GetComponent<AnimationComponent>();
    auto EAnime = chara->GetComponent<AnimationComponent>();
    int  damage = 0;

    bool canAttack = false;
    Vector3 ToPlayer = player->chara->Transform->Translation - chara->Transform->Translation;
    float Distance = ToPlayer.Length();
    if (Distance <= 20.0f) { canAttack = true; }

    switch (((turn / 2) - 1) % 3) // 0, 1, 2, 0, 1, 2, ....
    {

    case 0:
    {
        if (canAttack) 
        {
            attackCalculate(player, 4);
            damage = 4;
            PAnime->SetClipByName(L"Hit");
            EAnime->SetClipByName(L"Attack");
        }
    }break;

    case 1:
    {
        if (canAttack)
        {
            attackCalculate(player, 6);
            damage = 6;
            PAnime->SetClipByName(L"Hit");
            EAnime->SetClipByName(L"Attack");
        }
    }break;

    case 2:
    {
        if (canAttack)
        {
            attackCalculate(player, 12);
            damage = 12;
            PAnime->SetClipByName(L"Hit");
            EAnime->SetClipByName(L"Attack");
        }
    }break;

    }

    if (player->hp < 0)
    {
        player->hp = 0;
        player->isDead = true;
    }
    doAction = true;
    return damage;
}

void Enemy_2::move(Player* player, int turn)
{
    Vector3 ToPlayer = player->chara->Transform->Translation - chara->Transform->Translation;
    float Distance = ToPlayer.Length();
    ToPlayer.Normalize();

    // 플레이어랑 거리가 20 미만일 경우, 안움직임
    if (Distance < 20.0f) { doMove = true; }
    // 거리가 20 이상 120 미만일 경우, 19.9까지 움직임(혹시 미묘한 오차땜에 안때릴까봐)
    else if (Distance >= 20.0f && Distance < 120.0f)
    {
        chara->MoveTo(chara->Transform->Translation + ToPlayer * (Distance - 19.9));
        doMove = true;
    }
    // 거리가 120 이상일 경우, 100만큼만 움직임
    else
    {
        chara->MoveTo(chara->Transform->Translation + ToPlayer * 100.0f);
        doMove = true;
    }
}

void Enemy_2::action(Player* player, int turn)
{
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
