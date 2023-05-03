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
	void UpdateHand(int handSize);									// �� ���۽�
	void UpdateHand(int handSize, int UsedCard, int DrawedCard);	// ī�� ������ 
	void UpdatePlayerState();
	void UpdateEnemyState();
	void DeadCheck();
	void ToolTipCheck();

	void Reaction();

public:
	int		playerNum = 1; //1p�� 1, 2p�� 2
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
	WidgetRenderSystem* WRS; // UI ������ ���� ����� ������

public:
	Deck* MyDeck = nullptr;
	Deck* YourDeck = nullptr;
	std::vector<DXTexture*> CardTextureList;
	WidgetObject* CardList[3] = { 0, };
	std::vector<Vector2> CardPosList[10];	// ī�尡 ���� 1~10���� ���� ��ġ��
	std::vector<DXTexture*> NumberTextureList_Red;
	std::vector<DXTexture*> NumberTextureList_Black;
	std::vector<DXTexture*> NumberTextureList_Damage;
	std::map<std::wstring, DXTexture*> TextTextureList;

public:
	bool TurnState = true; // true�� ����, false�� ����
	bool TurnStart = true;
	int	 TurnNum = 1;
	bool IsMoving = false;

public:
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
	int OtherPlayerUsedCardNum = 999; // ��� �÷��̾ ����� ī�� ����
	int OtherPlayerTargetEnemyNum = 999; // ��� �÷��̾ ī���� Ÿ������ ���� ���� �ѹ�, ���� ī���� 999
	bool hostTurnEnd = false;
	bool clientTurnEnd = false;
public:
	bool MoveClickCheck = false;
	bool OtherPlayerIsMoving = false;
	Vector3 MoveLocation = { 99999.0f, 99999.0f, 99999.0f };  // ȣ�������� 0�̵Ǵ� ��츦 ����ؼ� �Է��� ������ ��� 99999�� ����
};