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
	void Init_Sound();

public:
	void CameraMove(Vector3& eye, Vector3& target);

public:
	void BattleProcess();
	void TurnStartProcess();
	void TurnEndProcess();
	void EnemyTurnProcess();
	void EnemyAttackAnimProcess();
	void MoveProcess();
	void CardCheck();
	bool ManaCheck(int cost);
	void UpdateHand(int handSize);					// 턴 시작시
	void UpdateHand(int handSize, int UsedCard);	// 카드 썼을때 
	void UpdatePlayerState();
	void UpdateEnemyState();
	void DeadCheck();

public:
	Player* player;
	std::vector<BaseEnemy*> EnemyList;
	Enemy_1* enemy1;
	Enemy_2* enemy2;

	Character* PlayerCharacter = nullptr;
	Character* EnemyCharacter = nullptr;
	Character* PickedCharacter = nullptr;
	BaseEnemy* TargetEnemy = nullptr;
	BaseEnemy* InActionEnemy = nullptr;
	int		   InActionEnemyNum = 0;

public:
	Camera* CurrentCamera;
	Camera* MoveCamera;
	Camera* CloseupCamera;
	
	CameraSystem* MainCameraSystem = nullptr;
	RenderSystem* MainRenderSystem = nullptr;
	Actor* MainCameraActor = nullptr;

public:
	std::map<std::wstring, FMODSound*> SoundMap;

public:
	WidgetObject* TurnEndButton;
	WidgetObject* RemainCardButton;
	WidgetObject* DiscardButton;
	WidgetObject* MoveButton;

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

	//std::vector<WidgetObject*> EnemyStateObjectList;
	//WidgetObject* EnemyCurrentHP1;
	//WidgetObject* EnemyCurrentHP2;
	//WidgetObject* EnemyMaxHP1;
	//WidgetObject* EnemyMaxHP2;
	//// 적 방어도도 해야하는데.. 귀찮으니까 좀 나중에
	////WidgetObject* EnemyArmorIcon;
	////WidgetObject* EnemyArmor1;
	////WidgetObject* EnemyArmor2;
	//WidgetObject* EnemyIntentIcon;
	//WidgetObject* EnemyIntent1;
	//WidgetObject* EnemyIntent2;

	WidgetObject* Damage1;
	WidgetObject* Damage2;

public:
	Deck* Dick;
	std::vector<DXTexture*> CardTextureList;
	WidgetObject* CardList[3] = { 0, };
	std::vector<Vector2> CardPosList[10];	// 카드가 각각 1~10장일 때의 위치값
	std::vector<DXTexture*> NumberTextureList_Red;
	std::vector<DXTexture*> NumberTextureList_Black;

public:
	bool TurnState = true; // true면 내턴, false면 적턴
	bool TurnStart = true;
	int	 TurnNum = 1;
	bool IsMoving = false;
};

