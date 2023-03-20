#pragma once
#include "BaseEnemy.h"
class Enemy_1 : public BaseEnemy
{
public:
	virtual bool Init() override;
	virtual void patern(Player* player, int turn) override;
};

