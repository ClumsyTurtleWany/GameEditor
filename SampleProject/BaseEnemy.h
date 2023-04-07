#pragma once
#include "Player.h"
#include "Character.h"
#include "WidgetComponent.h"
#include "UI_Loader.h"
#include "Actor.h"
#include "AnimationComponent.h"
#include "World.hpp"

class BaseEnemy
{	
public:
	virtual bool Init();
	virtual void patern(Player* player, int turn);
	virtual void attack(Player* player, int damage);
	virtual void SetIntentObj(int turn,  WidgetObject* image, WidgetObject* intent1, WidgetObject* intent2);
	virtual void UpdateState();

public:
	int hp;
	int maxHp;
	int armor;
	Character* chara;

public:
	ECS::World* pWorld;
	Actor* wa;
	WidgetComponent* wc;

public:
	std::vector<WidgetObject*> ObjList;
	WidgetObject* CurrentHP1;
	WidgetObject* CurrentHP2;
	WidgetObject* MaxHP1;
	WidgetObject* MaxHP2;
	// ���� �ؾ��ϴµ�.. �������ϱ� �� ���߿�
	//WidgetObject* ArmorIcon;
	//WidgetObject* Armor1;
	//WidgetObject* Armor2;
	WidgetObject* IntentIcon;
	WidgetObject* Intent1;
	WidgetObject* Intent2;

public:
	std::vector<DXTexture*> IntentImageList;
	std::vector<DXTexture*> NumberTextureList_Red;
	std::vector<DXTexture*> NumberTextureList_Black;
};

