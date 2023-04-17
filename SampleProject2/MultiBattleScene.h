#pragma once
#include "BaseScene.h"
#include "Player.h"
#include "Enemy_1.h"
#include "Character.h"
#include "protocol.pb.h"

class MultiBattleScene : public BaseScene
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
	void MyTurnProcess();
	void TurnEndProcess();
	void EnemyTurnProcess();
	void EnemyAttackAnimProcess();
	void CardCheck();
	bool ManaCheck(int cost);
	void UpdateHand(int handSize);
	void UpdatePlayerState();
	void UpdateEnemyState();
	void DeadCheck();

public:
	int		playerNum = 1; //1p면 1, 2p면 2
	Player* player1;
	Player* player2;
	Player* CurrentPlayer;
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

	WidgetObject* Damage1;
	WidgetObject* Damage2;

public:
	Deck* MyDeck = nullptr;
	Deck* YourDeck = nullptr;
	std::vector<DXTexture*> CardTextureList;
	WidgetObject* CardList[3] = { 0, };
	std::vector<DXTexture*> NumberTextureList_Red;
	std::vector<DXTexture*> NumberTextureList_Black;

public:
	bool TurnState = true; // true면 내턴, false면 적턴
	bool TurnStart = true;
	int	 TurnNum = 1;
	bool MyTurnStart = true;
	int  WhoseTurn = 1;	// 1이면 1p턴, 2면 2p턴

// 서버 추가
public:
	Character* hostCharacter = nullptr;
	Character* clientCharacter = nullptr;
	bool initTriger = true;
public:
	//void temp(); //혁 테스트 용도
	//void Init_Chara2();
	//void OtherPlayerUsedCard();
public: //패킷 보내는 함수, S_ : server sends to client, C_ : client sends to server

public:
	int OtherPlayerUsedCardNum = 0; // 상대 플레이어가 사용한 카드 종류
	int OtherPlayerTargetEnemyNum = 0; // 상대 플레이어가 카드의 타겟으로 정한 적의 넘버, 방어류 카드라면 0
};