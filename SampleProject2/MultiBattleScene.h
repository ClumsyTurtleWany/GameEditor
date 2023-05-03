#pragma once
#include "BaseScene.h"
#include "Player.h"
#include "Enemy_1.h"
#include "Character.h"
#include "ActionCamera.h"
#include "SaveLoadSystem.h"
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
	void UpdateCameraPos();
	void NotifierCheck();

public:
	void BattleProcess();
	void TurnStartProcess();
	void MyTurnProcess();
	void TurnEndProcess();
	void EnemyTurnProcess();
	void EnemyAnimProcess();
	void MoveProcess();
	void CardCheck();
	void DamageAnimation(int damage, bool hitEnemy);
	bool ManaCheck(int cost);
	void UpdateHand(int handSize);									// 턴 시작시
	void UpdateHand(int handSize, int UsedCard, int DrawedCard);	// 카드 썼을때 
	void UpdatePlayerState();
	void UpdateEnemyState();
	void DeadCheck();
	void ToolTipCheck();

	void Reaction();

public:
	int		playerNum = 1; //1p면 1, 2p면 2
	Player* player1;
	Player* player2;
	std::vector<Player*> PlayerList;
	Player* CurrentPlayer;
	Player* TargetPlayer;
	std::vector<BaseEnemy*> EnemyList;
	std::vector<BaseEnemy*> AllEnemyList;
	Enemy_1* enemy1;
	Enemy_2* enemy2;

	Character* PlayerCharacter = nullptr;
	Character* EnemyCharacter = nullptr;
	Character* PickedCharacter = nullptr;
	BaseEnemy* TargetEnemy = nullptr;
	BaseEnemy* InActionEnemy = nullptr;
	int		   InActionEnemyNum = 0;

	int	PlayerDamage = 0;
	int	EnemyDamage = 0;

public:
	Camera* CurrentCamera;
	Camera* MoveCamera;
	Camera* CloseupCamera;
	ActionCamera* TestActionCamera;

	CameraSystem* MainCameraSystem = nullptr;
	RenderSystem* MainRenderSystem = nullptr;
	SaveLoadSystem* MainSaveLoadSystem = nullptr;
	Actor* MainCameraActor = nullptr;

	Vector3 PlayerCameraPos;

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
	WidgetObject* PlayerHPBar;
	WidgetObject* PlayerArmorIcon;
	WidgetObject* PlayerArmor1;
	WidgetObject* PlayerArmor2;
	WidgetObject* ManaIcon;
	WidgetObject* CurrentMana;
	WidgetObject* MaxMana;

	WidgetObject* Damage1;
	WidgetObject* Damage2;

	WidgetObject* ToolTipBoard;
	WidgetObject* ToolTipText;

public:
	WidgetRenderSystem* WRS; // UI 숨길라고 따로 멤버로 만들어둠

public:
	Deck* MyDeck = nullptr;
	Deck* YourDeck = nullptr;
	std::vector<DXTexture*> CardTextureList;
	WidgetObject* CardList[3] = { 0, };
	std::vector<Vector2> CardPosList[10];	// 카드가 각각 1~10장일 때의 위치값
	std::vector<DXTexture*> NumberTextureList_Red;
	std::vector<DXTexture*> NumberTextureList_Black;
	std::vector<DXTexture*> NumberTextureList_Damage;
	std::map<std::wstring, DXTexture*> TextTextureList;

public:
	bool TurnState = true; // true면 내턴, false면 적턴
	bool TurnStart = true;
	int	 TurnNum = 1;
	bool IsMoving = false;

public:
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
	int OtherPlayerUsedCardNum = 999; // 상대 플레이어가 사용한 카드 종류
	int OtherPlayerTargetEnemyNum = 999; // 상대 플레이어가 카드의 타겟으로 정한 적의 넘버, 방어류 카드라면 999
	bool hostTurnEnd = false;
	bool clientTurnEnd = false;
public:
	bool MoveClickCheck = false;
	bool OtherPlayerIsMoving = false;
	Vector3 MoveLocation = { 99999.0f, 99999.0f, 99999.0f };  // 호오오옥시 0이되는 경우를 대비해서 입력이 없었을 경우 99999로 세팅
};