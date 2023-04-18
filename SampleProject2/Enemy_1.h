#pragma once
#include "BaseEnemy.h"

class Enemy_1 : public BaseEnemy
{
public:
	virtual bool Init() override;
	virtual int patern(Player* player, int turn) override;
	virtual void SetIntentObj(int turn, WidgetObject* image, WidgetObject* intent1, WidgetObject* intent2);
};

class Enemy_2 : public BaseEnemy
{
public:
	virtual bool Init() override;
	virtual int patern(Player* player, int turn) override;
	virtual void SetIntentObj(int turn, WidgetObject* image, WidgetObject* intent1, WidgetObject* intent2);
};
