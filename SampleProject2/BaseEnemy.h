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
	virtual int	 patern(Player* player, int turn);
	virtual void move(Player* player, int turn);
	virtual void action(Player* player, int turn);
	virtual void attackCalculate(Player* player, int damage);
	virtual void SetIntentObj(int turn,  WidgetObject* image, WidgetObject* intent1, WidgetObject* intent2);
	virtual void UpdateState();

public:
	Camera* MainCamera;

public:
	int hp;
	int maxHp;
	int armor;
	Character* chara;

public:
	bool doMove = false;
	bool doAction = false;

public:
	ECS::World* pWorld;
	Actor* wa;
	WidgetComponent* wc;

public:
	std::vector<WidgetObject*> ObjList;
	WidgetObject* BG;
	WidgetObject* Name;
	WidgetObject* HPBar_Empty;
	WidgetObject* HPBar_Full;
	WidgetObject* CurrentHP1;
	WidgetObject* CurrentHP2;
	WidgetObject* MaxHP1;
	WidgetObject* MaxHP2; 
	WidgetObject* Slash;

	// 이쪽은 빌보드
	WidgetObject* HpEmpty;
	WidgetObject* HpFull;

	// 방어도도 해야하는데.. 귀찮으니까 좀 나중에
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

