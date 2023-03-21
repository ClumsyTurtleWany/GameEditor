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
	bool ManaCheck(int cost);
	void UpdateHand(int handSize);
	void UpdatePlayerState();
	void UpdateEnemyState();

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
	WidgetObject* PlayerArmorIcon;
	WidgetObject* PlayerArmor1;
	WidgetObject* PlayerArmor2;
	WidgetObject* ManaIcon;
	WidgetObject* CurrentMana;
	WidgetObject* MaxMana;

	WidgetObject* EnemyCurrentHP1;
	WidgetObject* EnemyCurrentHP2;
	WidgetObject* EnemyMaxHP1;
	WidgetObject* EnemyMaxHP2;
	// 적 방어도도 해야하는데.. 귀찮으니까 좀 나중에
	//WidgetObject* EnemyArmorIcon;
	//WidgetObject* EnemyArmor1;
	//WidgetObject* EnemyArmor2;

public:
	Deck* Dick;
	std::vector<DXTexture*> CardTextureList;
	WidgetObject* CardList[3] = { 0, };
	std::vector<DXTexture*> NumberTextureList_Red;
	std::vector<DXTexture*> NumberTextureList_Black;

public:
	bool TurnState = true; // true면 내턴, false면 적턴
	bool TurnStart = true;
	int	 TurnNum = 1;
};

