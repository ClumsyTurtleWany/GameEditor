#include "BaseEnemy.h"

bool BaseEnemy::Init()
{	
	UI_Loader Loader;

	wa = new Actor;
	wc = wa->AddComponent<WidgetComponent>();
	Loader.FileLoad(wc, L"../resource/UI/Save/EnemyState.txt");

	BG = wc->FindObj(L"EnemyInfoBG");
	CurrentHP1 = wc->FindObj(L"EnemyCurrentHp_1");
	CurrentHP2 = wc->FindObj(L"EnemyCurrentHp_2");
	MaxHP1 = wc->FindObj(L"EnemyMaxHp_1");
	MaxHP2 = wc->FindObj(L"EnemyMaxHp_2");
	IntentIcon = wc->FindObj(L"EnemyIntent");
	Intent1 = wc->FindObj(L"EnemyIntent_1");
	Intent2 = wc->FindObj(L"EnemyIntent_2");
	Slash = wc->FindObj(L"Slash2");

	ObjList.push_back(CurrentHP1);
	ObjList.push_back(CurrentHP2);
	ObjList.push_back(MaxHP1);
	ObjList.push_back(MaxHP2);
	ObjList.push_back(IntentIcon);
	ObjList.push_back(Intent1);
	ObjList.push_back(Intent2);
	ObjList.push_back(BG);
	ObjList.push_back(Slash);

	// ㅠㅠ... 빌보드 쓰고싶었는데,,
	//for (auto obj : ObjList) 
	//{
	//	obj->MainCamera = MainCamera;
	//	obj->m_b3D = true;
	//	obj->scale = 20.0f;
	//}

	(*pWorld).AddEntity(wa);

	return true;
}

int BaseEnemy::patern(Player* player, int turn)
{
	return 0;
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

void BaseEnemy::SetIntentObj(int turn, WidgetObject* image, WidgetObject* intent1, WidgetObject* intent2)
{
}

void BaseEnemy::UpdateState()
{
	if (hp <= 0)
	{
		// 적 격파 이벤트 발생
		hp = 0;
		auto EAnime = chara->GetComponent<AnimationComponent>();
		EAnime->SetClipByName(L"Dying");
	}

	CurrentHP1->m_pCutInfoList[0]->tc = NumberTextureList_Black[hp / 10];
	CurrentHP2->m_pCutInfoList[0]->tc = NumberTextureList_Black[hp % 10];
	MaxHP1->m_pCutInfoList[0]->tc = NumberTextureList_Black[maxHp / 10];
	MaxHP2->m_pCutInfoList[0]->tc = NumberTextureList_Black[maxHp % 10];

	// 방어도 부분, 좀 나중에..
	/*if (player->armor <= 0)
	{
		PlayerArmorIcon->m_bIsDead = true;
		PlayerArmor1->m_bIsDead = true;
		PlayerArmor2->m_bIsDead = true;
	}
	else
	{
		PlayerArmorIcon->m_bIsDead = false;
		PlayerArmor2->m_bIsDead = false;
		PlayerArmor2->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->armor % 10];
		if ((player->armor / 10) >= 1)
		{
			PlayerArmor1->m_OriginPos = { -0.714, -0.693 };
			PlayerArmor2->m_OriginPos = { -0.693, -0.693 };

			PlayerArmor1->m_bIsDead = false;
			PlayerArmor1->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->armor / 10];
		}
		else
		{
			PlayerArmor2->m_OriginPos = { -0.703, -0.693 };
		}
	}*/
}

