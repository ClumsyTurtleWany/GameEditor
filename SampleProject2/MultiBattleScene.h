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
	int		playerNum = 1; //1p�� 1, 2p�� 2
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
	bool TurnState = true; // true�� ����, false�� ����
	bool TurnStart = true;
	int	 TurnNum = 1;
	bool MyTurnStart = true;
	int  WhoseTurn = 1;	// 1�̸� 1p��, 2�� 2p��

// ���� �߰�
public:
	Character* hostCharacter = nullptr;
	Character* clientCharacter = nullptr;
	bool initTriger = true;
public:
	//void temp(); //�� �׽�Ʈ �뵵
	//void Init_Chara2();
	//void OtherPlayerUsedCard();
public: //��Ŷ ������ �Լ�, S_ : server sends to client, C_ : client sends to server

public:
	int OtherPlayerUsedCardNum = 0; // ��� �÷��̾ ����� ī�� ����
	int OtherPlayerTargetEnemyNum = 0; // ��� �÷��̾ ī���� Ÿ������ ���� ���� �ѹ�, ���� ī���� 0
};