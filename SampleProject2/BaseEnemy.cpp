#include "BaseEnemy.h"

bool BaseEnemy::Init()
{	
	UI_Loader Loader;

	wa = new Actor;
	wc = wa->AddComponent<WidgetComponent>();
	Loader.FileLoad(wc, L"../resource/UI/Save/EnemyState_new.txt");

	BG = wc->FindObj(L"EnemyInfoBG");
	Name = wc->FindObj(L"EnemyName");
	HPBar_Empty = wc->FindObj(L"HPBar_Empty");
	HPBar_Full = wc->FindObj(L"HPBar_Full");
	CurrentHP1 = wc->FindObj(L"EnemyCurrentHp_1");
	CurrentHP2 = wc->FindObj(L"EnemyCurrentHp_2");
	MaxHP1 = wc->FindObj(L"EnemyMaxHp_1");
	MaxHP2 = wc->FindObj(L"EnemyMaxHp_2");
	IntentIcon = wc->FindObj(L"EnemyIntent");
	Intent1 = wc->FindObj(L"EnemyIntent_1");
	Intent2 = wc->FindObj(L"EnemyIntent_2");
	Slash = wc->FindObj(L"Slash2");

	ObjList.push_back(BG);
	ObjList.push_back(Name);
	ObjList.push_back(HPBar_Empty);
	ObjList.push_back(HPBar_Full);
	ObjList.push_back(CurrentHP1);
	ObjList.push_back(CurrentHP2);
	ObjList.push_back(MaxHP1);
	ObjList.push_back(MaxHP2);
	ObjList.push_back(IntentIcon);
	ObjList.push_back(Intent1);
	ObjList.push_back(Intent2);
	ObjList.push_back(Slash);

	//for (auto obj : ObjList) 
	//{
	//	obj->MainCamera = MainCamera;
	//	obj->m_b3D = true;
	//	obj->scale = 30.0f;
	//}

	for (auto obj : ObjList) 
	{
		obj->m_OriginalOriginPos.y += 0.7f;
		obj->m_OriginPos.y += 0.7f;
	}
	// 빌보드 쓸놈들 처리
	Loader.FileLoad(wc, L"../resource/UI/Save/HpBar.txt");
	HpEmpty = wc->FindObj(L"Empty");
	HpFull = wc->FindObj(L"Full");
	HpEmpty->MainCamera = MainCamera;
	///HpEmpty->m_b3D = true;
	HpEmpty->scale = 5.0f;
	HpFull->MainCamera = MainCamera;
	//HpFull->m_b3D = true;
	HpFull->scale = 5.0f;

	// 프로그레스 바를 위해서 좌상단 원점으로 바꿔줌
	HPBar_Full->SetOrginStandard(false);
	HPBar_Full->m_OriginWH = HPBar_Full->m_pCutInfoList[0]->pxWH;
	HPBar_Full->m_OriginUV = HPBar_Full->m_pCutInfoList[0]->uv;

	HpFull->SetOrginStandard(false);
	HpFull->m_OriginWH = HpFull->m_pCutInfoList[0]->pxWH;
	HpFull->m_OriginUV = HpFull->m_pCutInfoList[0]->uv;

	(*pWorld).AddEntity(wa);

	return true;
}

void BaseEnemy::move(Player* player, int turn)
{
}

void BaseEnemy::action(Player* player, int turn)
{
}


int BaseEnemy::patern(Player* player, int turn)
{
	return 0;
}

void BaseEnemy::attackCalculate(Player* player, int damage)
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

	// 프로그레스 바 (HP바) 
	float hpPercentage = (float)hp / (float)maxHp;
	HPBar_Full->m_pCutInfoList[0]->pxWH.x = HPBar_Full->m_OriginWH.x * hpPercentage;	// X축만(가로로만)줄어들어야하니까
	HPBar_Full->m_pCutInfoList[0]->uv =		HPBar_Full->m_OriginUV * hpPercentage;			// x축만 ㅇㅇ
	HpFull->m_pCutInfoList[0]->pxWH.x = HpFull->m_OriginWH.x * hpPercentage;	
	HpFull->m_pCutInfoList[0]->uv =		HpFull->m_OriginUV * hpPercentage;			

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

