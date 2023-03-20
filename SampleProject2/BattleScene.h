#pragma once
#include "BaseScene.h"
#include "Player.h"
#include "Enemy_1.h"
#include "Character.h"

class BattleScene : public BaseScene
{
public:
	virtual bool Init() override;
	virtual bool Frame() override;
	virtual bool Render() override;
	virtual bool Release() override;

public:
	void Init_UI();
	void Init_Map();
	void Init_Chara();
	void Init_Effect();

public:
	void BattleProcess();
	void TurnStartProcess();
	void TurnEndProcess();
	void EnemyTurnProcess();
	void CardCheck();
	void UpdateHand(int handSize);

public:
	Player* player;
	Enemy_1* enemy;

	Character* PlayerCharacter = nullptr;

public:
	Camera* MoveCamera;
	Camera* CloseupCamera;

//public:
//	MapComponent* Map;
//	
//public:
//	CharacterComponent* Chara_1P;
//	CharacterComponent* Chara_2P;
//	CharacterComponent* Enemy1;
//	CharacterComponent* Enemy2;

public:
	WidgetObject* TurnEndButton;
	WidgetObject* RemainCardButton;
	WidgetObject* DiscardButton;

	WidgetObject* PlayerCurrenHP1;
	WidgetObject* PlayerCurrenHP2;
	WidgetObject* PlayerMaxHP1;
	WidgetObject* PlayerMaxHP2;

public:
	Deck* Dick;
	std::vector<DXTexture*> CardTextureList;
	WidgetObject* CardList[3] = { 0, };
	std::vector<DXTexture*> NumberTextureList;

public:
	bool TurnState = true; // true면 내턴, false면 적턴
	bool TurnStart = true;
	int	 TurnNum = 1;
};

