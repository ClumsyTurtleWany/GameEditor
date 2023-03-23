#pragma once
#include "Player.h"
#include "Character.h"
#include "WidgetObject.h"
class BaseEnemy
{	
public:
	virtual bool Init();
	virtual void patern(Player* player, int turn);
	virtual void attack(Player* player, int damage);
	virtual void SetIntentObj(int turn,  WidgetObject* image, WidgetObject* intent1, WidgetObject* intent2);

public:
	int hp;
	int maxHp;
	int armor;
	Character* chara;

public:
	std::vector<DXTexture*> IntentImageList;
	std::vector<DXTexture*> NumberTextureList_Red;
};

