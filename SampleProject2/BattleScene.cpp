#include "BattleScene.h"
#include "LightSystem.h"
#include "DirectionalLight.h"
#include "SkyBoxComponent.h"
#include "FBXLoader.hpp"
#include "Landscape.h"
#include "SkeletalMeshComponent.h"
#include "MaterialManager.h"
#include "SkyRenderSystem.h"
#include "SkyDomeComponent.h"
//�߰�
#include "AnimationComponent.h"
#include "UpdateAnimSystem.h"
#include "MovementSystem.h"
#include "Character.h"
#include "SelectAnimSystem.h"
#include "BoundingBoxComponent.h"

//�߰�
#include "ColliderSystem.h"

///////////////////
//Effect Include
///////////////////
#include "EffectInclude/EffectSystem.h"

bool BattleScene::Init()
{
	ID = battle;

	// ��� �÷��̾�� Ÿ��Ʋ���� �� �ʱ�ȭ�ϰ� ���°͵�..
	player = new Player;
	player->cost = player->maxCost;
	player->maxHp = 50;
	player->hp = 50; 
	enemy = new Enemy_1;
	enemy->Init();

	Init_UI();
	Init_Map();
	Init_Chara();
	Init_Effect();

	// ī�޶� �ý��� �� ������ �ý��� �߰�.
	TheWorld.AddSystem(new CameraSystem);
	TheWorld.AddSystem(new ColliderSystem);
	TheWorld.AddSystem(new RenderSystem);
	TheWorld.AddSystem(new WidgetRenderSystem);
	TheWorld.AddSystem(new MovementSystem);
	TheWorld.AddSystem(new UpdateAnimSystem);
	// SelectAnimSystem �߰�
	TheWorld.AddSystem(new SelectAnimSystem);

	LightSystem* lightSystem = new LightSystem;
	lightSystem->Initialize();
	TheWorld.AddSystem(lightSystem);

	return true;
}

bool BattleScene::Frame()
{
	BaseScene::Frame();

	KeyState btnA = Input::GetInstance()->getKey('A');
	if (btnA == KeyState::Hold || btnA == KeyState::Down)
	{
		MainCamera->Yaw -= 0.001f;
	}

	KeyState btnD = Input::GetInstance()->getKey('D');
	if (btnD == KeyState::Hold || btnD == KeyState::Down)
	{
		MainCamera->Yaw += 0.001f;
	}

	KeyState btnW = Input::GetInstance()->getKey('W');
	if (btnW == KeyState::Hold || btnW == KeyState::Down)
	{
		MainCamera->Pitch -= 0.001f;
	}

	KeyState btnS = Input::GetInstance()->getKey('S');
	if (btnS == KeyState::Hold || btnS == KeyState::Down)
	{
		MainCamera->Pitch += 0.001f;
	}

	KeyState btnQ = Input::GetInstance()->getKey('Q');
	if (btnQ == KeyState::Hold || btnQ == KeyState::Down)
	{
		MainCamera->Pos.z += 0.01f;
	}

	KeyState btnE = Input::GetInstance()->getKey('E');
	if (btnE == KeyState::Hold || btnE == KeyState::Down)
	{
		MainCamera->Pos.z -= 0.01f;
	}

	PlayerCharacter->MoveTo(MAIN_PICKER.Intersection);
	return true;
}

bool BattleScene::Render()
{
	BaseScene::Render();

	// ���� ī�� Ȯ�� or ���� ī�� Ȯ�� ��ư Ŭ���� �� ��ȯ (ī�� ���� ������)
	if (RemainCardButton->m_bClicked){ SS = remainView; RemainCardButton->m_bClicked = false; }
	if (DiscardButton->m_bClicked) { SS = discardView; DiscardButton->m_bClicked = false; }

	//���� ���⼭ ���� ���ư��� �ɼ�������
	BattleProcess();

	return true;
}

bool BattleScene::Release()
{
	BaseScene::Release();
    return true;
}

void BattleScene::Init_UI()
{
	UI_Loader Loader;

	// Actor ����
	Actor* UI = new Actor;
	//UI->Name = L"Battle";
	auto bc = UI->AddComponent<WidgetComponent>();
	Loader.FileLoad(bc, L"../resource/UI/Save/Battle.txt");

	TurnEndButton = bc->FindObj(L"TurnEnd");
	RemainCardButton = bc->FindObj(L"Remain");
	DiscardButton = bc->FindObj(L"Discard");

	// ī�� ������Ʈ ���� �� ����Ʈ�� �־���
	CardList[0] = bc->FindObj(L"Card1");
	CardList[0]->m_bDraggable = true;
	CardList[1] = bc->FindObj(L"Card2");
	CardList[1]->m_bDraggable = true;
	CardList[2] = bc->FindObj(L"Card3");
	CardList[2]->m_bDraggable = true;

	// �÷��̾� �Žñ� ǥ�ÿ� 
	PlayerCurrenHP1 = bc->FindObj(L"PlayerCurrentHP_1");
	PlayerCurrenHP2 = bc->FindObj(L"PlayerCurrentHP_2");
	PlayerMaxHP1 = bc->FindObj(L"PlayerMaxHP_1");
	PlayerMaxHP2 = bc->FindObj(L"PlayerMaxHP_2");
	PlayerArmorIcon = bc->FindObj(L"PlayerArmor_Icon");
	PlayerArmor1 = bc->FindObj(L"PlayerArmor_1");
	PlayerArmor2 = bc->FindObj(L"PlayerArmor_2");
	CurrentMana = bc->FindObj(L"Mana_current");
	MaxMana = bc->FindObj(L"Mana_max");
	UpdatePlayerState();

	// �� ����
	EnemyCurrentHP1 = bc->FindObj(L"EnemyCurrentHp_1");
	EnemyCurrentHP2 = bc->FindObj(L"EnemyCurrentHp_2");
	EnemyMaxHP1 = bc->FindObj(L"EnemyMaxHp_1");
	EnemyMaxHP2 = bc->FindObj(L"EnemyMaxHp_2");
	UpdateEnemyState();

	// ���� ���忡 ���� �߰�.
	TheWorld.AddEntity(UI);
}

void BattleScene::Init_Map()
{	
	//// ���� ���� �߰�.
	Landscape* landscape = new Landscape;
	auto landscapeComponents = landscape->GetComponent<LandscapeComponents>();
	landscapeComponents->Build(16, 16, 7, 10);
	landscapeComponents->SetCamera(MainCamera);
	TheWorld.AddEntity(landscape);
	
	// Sky Dome �߰�.
	Actor* skyDomeActor = new Actor;
	auto skyDomeComp = skyDomeActor->AddComponent<SkyDomeComponent>();
	skyDomeComp->Scale = Vector3(5000.0f, 5000.0f, 5000.0f);
	TheWorld.AddEntity(skyDomeActor);
	TheWorld.AddSystem(new SkyRenderSystem);

	DirectionalLight* light = new DirectionalLight;
	auto lightComp = light->GetComponent<DirectionalLightComponent>();
	lightComp->Color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	lightComp->Direction = Vector4(1.0f, -1.0f, 1.0f, 1.0f);
	TheWorld.AddEntity(light);

	DirectionalLight* light2 = new DirectionalLight;
	auto lightComp2 = light2->GetComponent<DirectionalLightComponent>();
	lightComp2->Color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	lightComp2->Direction = Vector4(-1.0f, 1.0f, 1.0f, 1.0f);
	TheWorld.AddEntity(light2);

	for (int cnt = 0; cnt < 4; cnt++)
	{
		Actor* backgroundBuliding = new Actor;
		auto backgroundBulidingStaticMesh = backgroundBuliding->AddComponent<StaticMeshComponent>();
		auto boundBox = backgroundBuliding->AddComponent<BoundingBoxComponent>(Vector3(1.2f, 0.5f, 1.0f));
		if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Map/Warehouse/Warehouse.FBX"))
		{
			FBXLoader::GetInstance()->GenerateStaticMeshFromFileData(L"../resource/FBX/Map/Warehouse/Warehouse.FBX", backgroundBulidingStaticMesh);
		}
		auto backgroundBulidingTransform = backgroundBuliding->GetComponent<TransformComponent>();
		backgroundBulidingTransform->Scale = Vector3(100.0f, 100.0f, 100.0f);
		backgroundBulidingTransform->Rotation = Vector3(0.0f, 0.0f, 0.0f);
		backgroundBulidingTransform->Translation = Vector3(-500.0f + static_cast<float>(cnt) * 250, 40.0f, 300.0f);
		TheWorld.AddEntity(backgroundBuliding);
	}

	for (int cnt = 0; cnt < 4; cnt++)
	{
		Actor* backgroundBuliding = new Actor;
		auto backgroundBulidingStaticMesh = backgroundBuliding->AddComponent<StaticMeshComponent>();
		auto boundBox = backgroundBuliding->AddComponent<BoundingBoxComponent>(Vector3(1.2f, 0.5f, 1.0f));
		if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Map/Warehouse/Warehouse.FBX"))
		{
			FBXLoader::GetInstance()->GenerateStaticMeshFromFileData(L"../resource/FBX/Map/Warehouse/Warehouse.FBX", backgroundBulidingStaticMesh);
		}
		auto backgroundBulidingTransform = backgroundBuliding->GetComponent<TransformComponent>();
		backgroundBulidingTransform->Scale = Vector3(100.0f, 100.0f, 100.0f);
		backgroundBulidingTransform->Rotation = Vector3(0.0f, 0.0f, 0.0f);
		backgroundBulidingTransform->Translation = Vector3(-500.0f + static_cast<float>(cnt) * 250, 40.0f, -300.0f);
		TheWorld.AddEntity(backgroundBuliding);
	}

	for (int cnt = 0; cnt < 8; cnt++)
	{
		Actor* container = new Actor;
		auto staticMesh = container->AddComponent<StaticMeshComponent>();
		auto boundBox = container->AddComponent<BoundingBoxComponent>(Vector3(150.0f, 250.0f, 300.0f));
		if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Map/Container/Shipping_Container_A_-_Model.FBX"))
		{
			FBXLoader::GetInstance()->GenerateStaticMeshFromFileData(L"../resource/FBX/Map/Container/Shipping_Container_A_-_Model.FBX", staticMesh);
		}
		auto transform = container->GetComponent<TransformComponent>();
		transform->Scale = Vector3(0.2f, 0.2f, 0.2f);
		transform->Rotation = Vector3(0.0f, 0.0f, 0.0f);
		transform->Translation = Vector3(500.0f , 0.0f, -500.0f + static_cast<float>(cnt) * 125);
		TheWorld.AddEntity(container);
	}

	for (int cnt = 0; cnt < 8; cnt++)
	{
		Actor* container = new Actor;
		auto staticMesh = container->AddComponent<StaticMeshComponent>();
		auto boundBox = container->AddComponent<BoundingBoxComponent>(Vector3(150.0f, 250.0f, 300.0f));
		if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Map/Container/Shipping_Container_A_-_Model.FBX"))
		{
			FBXLoader::GetInstance()->GenerateStaticMeshFromFileData(L"../resource/FBX/Map/Container/Shipping_Container_A_-_Model.FBX", staticMesh);
		}
		auto transform = container->GetComponent<TransformComponent>();
		transform->Scale = Vector3(0.2f, 0.2f, 0.2f);
		transform->Rotation = Vector3(0.0f, 0.0f, 0.0f);
		transform->Translation = Vector3(500.0f, 0.0f, -500.0f + static_cast<float>(cnt) * 125);
		TheWorld.AddEntity(container);
	}

	/*Actor* cargoShip = new Actor;
	auto staticMesh = cargoShip->AddComponent<StaticMeshComponent>();
	auto boundBox = cargoShip->AddComponent<BoundingBoxComponent>(Vector3(150.0f, 250.0f, 300.0f));
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Map/CargoshipBlend.fbx"))
	{
		FBXLoader::GetInstance()->GenerateStaticMeshFromFileData(L"../resource/FBX/Map/CargoshipBlend.fbx", staticMesh);
	}
	auto transform = cargoShip->GetComponent<TransformComponent>();
	transform->Scale = Vector3(1.0f, 1.0f, 1.0f);
	transform->Rotation = Vector3(0.0f, 0.0f, 0.0f);
	transform->Translation = Vector3(0.0f, 0.0f, -0.0f);
	TheWorld.AddEntity(cargoShip);*/	
}

void BattleScene::Init_Chara()
{
	PlayerCharacter = new Character;
	auto playerCharMeshComp = PlayerCharacter->AddComponent<SkeletalMeshComponent>();
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Hulk_fbx/HULK.FBX")) //hulk_removeTwist
	{
		FBXLoader::GetInstance()->GenerateSkeletalMeshFromFileData(L"../resource/FBX/Hulk_fbx/HULK.FBX", playerCharMeshComp);
	}

	// GenerateAnimationFromFileData()���� �ִϸ��̼� ������Ʈ�� �ִϸ��̼� �߰��ϴ� ��� 
	// ClipList�� ����Ǹ� SetClipByName(name) �Լ��� ���氡�� <- name = Ȯ���ڸ� ������ ���ϸ�
	auto playerCharAnimComp = PlayerCharacter->AddComponent<AnimationComponent>();
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Hulk_fbx/Run.FBX"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Hulk_fbx/Run.FBX", playerCharAnimComp);
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Hulk_fbx/Idle.FBX"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Hulk_fbx/Idle.FBX", playerCharAnimComp);
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Hulk_fbx/Punch.FBX"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Hulk_fbx/Punch.FBX", playerCharAnimComp);
	}
	// �̷������� ���� �ٲ� �� ���� (�ΰ��ӿ��� �ٲٴ� �� �۾� �� �ؾ���)
	//playerCharAnimComp->SetClipByName(L"Punch");
	



	auto playerCharTransformComp = PlayerCharacter->GetComponent<TransformComponent>();
	playerCharTransformComp->Scale = Vector3(10.f, 10.f, 10.f);
	playerCharTransformComp->Rotation = Vector3(0.0f, 90.0f, 0.0f);
	playerCharTransformComp->Translation = Vector3(0.0f, 0.0f, 0.0f);

	auto playerCharMovementComp = PlayerCharacter->GetComponent<MovementComponent>();
	playerCharMovementComp->Speed = 25.0f;

	
	PlayerCharacter->MoveTo(Vector3(-10.0f, 0.0f, 0.0f));
	
	 
	MainCamera = PlayerCharacter->AddComponent<Camera>();
	MainCamera->CreateViewMatrix(Vector3(0.0f, 25.0f, -100.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0, 0.0f));
	MainCamera->CreateProjectionMatrix(1.0f, 10000.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));
	
	TheWorld.AddEntity(PlayerCharacter);


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////�� �׽�Ʈ ///////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Character* EnemyCharacter = new Character;

	auto enemyCharMeshComp = EnemyCharacter->AddComponent<SkeletalMeshComponent>();
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/WinterSoldier_fbx/WINTERSOLDIER.fbx")) 
	{
		FBXLoader::GetInstance()->GenerateSkeletalMeshFromFileData(L"../resource/FBX/WinterSoldier_fbx/WINTERSOLDIER.fbx", enemyCharMeshComp);
	}

	auto enemyCharAnimComp = EnemyCharacter->AddComponent<AnimationComponent>();
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/WinterSoldier_fbx/WS_Anim/Run.fbx"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/WinterSoldier_fbx/WS_Anim/Run.fbx", enemyCharAnimComp);
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/WinterSoldier_fbx/WS_Anim/Idle.fbx"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/WinterSoldier_fbx/WS_Anim/Idle.fbx", enemyCharAnimComp);
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/WinterSoldier_fbx/WS_Anim/Kick.fbx"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/WinterSoldier_fbx/WS_Anim/Kick.fbx", enemyCharAnimComp);
	}

	auto enemyCharTransformComp = EnemyCharacter->GetComponent<TransformComponent>();
	// ��� �� �۾Ƽ� 30�� scale ����
	enemyCharTransformComp->Scale = Vector3(30.f, 30.f, 30.f);
	enemyCharTransformComp->Rotation = Vector3(0.0f, -90.0f, 0.0f);
	enemyCharTransformComp->Translation = Vector3(50.0f, 0.0f, 200.0f);

	auto enemyCharMovementComp = EnemyCharacter->GetComponent<MovementComponent>();
	enemyCharMovementComp->Speed = 10.0f;


	EnemyCharacter->MoveTo(Vector3(-10.0f, 0.0f, 0.0f));


	//MainCamera = EnemyCharacter->AddComponent<Camera>();
	//MainCamera->CreateViewMatrix(Vector3(0.0f, 25.0f, -100.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0, 0.0f));
	//MainCamera->CreateProjectionMatrix(1.0f, 10000.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));

	TheWorld.AddEntity(EnemyCharacter);




}

void BattleScene::Init_Effect()
{	
	//Effect Test
	ParticleEffect* testEffect1 = new ParticleEffect(L"Fire");
	ParticleEffect* testEffect2 = new ParticleEffect(L"Smoke");
	ParticleEffect* testEffect3 = new ParticleEffect(L"Spark");
	auto testEffectTransform1 = testEffect1->GetComponent<TransformComponent>();
	auto testEffectTransform2 = testEffect2->GetComponent<TransformComponent>();
	auto testEffectTransform3 = testEffect3->GetComponent<TransformComponent>();
	testEffectTransform1->Translation = { 10.0f, 0.0f, 0.0f };
	testEffectTransform2->Translation = { 20.0f, 0.0f, 0.0f };
	testEffectTransform3->Translation = { 30.0f, 0.0f, 0.0f };
	TheWorld.AddEntity(testEffect1);
	TheWorld.AddEntity(testEffect2);
	TheWorld.AddEntity(testEffect3);

	ECS::EffectSystem* ESystem = new ECS::EffectSystem;
	ESystem->init(&TheWorld);
	TheWorld.AddSystem(ESystem);
}

void BattleScene::BattleProcess()
{
	if (TurnStart) { TurnStartProcess(); }
	if (TurnEndButton->m_bClicked) { TurnEndProcess(); }
	CardCheck();
}

void BattleScene::TurnStartProcess()
{
	TurnNum++;
	player->armor = 0;
	player->cost = player->maxCost;
	UpdatePlayerState();

	int drawNum = 3;
	for (int i = 0; i < drawNum; i++) { CardList[i]->m_bIsDead = false; }

	Dick->Draw(drawNum);
	UpdateHand(drawNum);

	TurnStart = false;
}

void BattleScene::TurnEndProcess()
{
	TurnNum++;
	Dick->TurnEnd();
	EnemyTurnProcess();

	TurnEndButton->m_bClicked = false;
}

void BattleScene::EnemyTurnProcess()
{
	enemy->patern(player, TurnNum);
	UpdatePlayerState();
	UpdateEnemyState();

	TurnStart = true;
}

void BattleScene::CardCheck()
{
	for (int cardNum=0; cardNum<Dick->HandList.size(); cardNum++) 
	{
		if (CardList[cardNum]->m_bClicked && CardList[cardNum]->m_OriginPos.y >= 0.5)
		{
			CardList[cardNum]->m_bClicked = false;
			CardList[cardNum]->m_OriginPos = CardList[cardNum]->m_OriginalOriginPos;

			bool CanUse = false;

			switch (Dick->HandList[cardNum])
			{

			case Strike:
			{
				if (ManaCheck(1))
				{
					enemy->hp -= 6;
					CanUse = true;
				}
			}break;

			case Defend:
			{
				if (ManaCheck(1)) 
				{
					player->armor += 5;
					CanUse = true;
				}
			}break;

			case PommelStrike:
			{
				if (ManaCheck(1)) 
				{
					enemy->hp -= 9;
					Dick->Draw(1);
					CanUse = true;
				}
			}break;

			case ShrugItOff:
			{
				if (ManaCheck(1)) 
				{
					player->armor += 8;
					Dick->Draw(1);
					CanUse = true;
				}
			}break;

			case Hemokinesis:
			{
				
			}break;

			case Bludgeon:
			{
				
			}break;

			case IronWave:
			{
				if (ManaCheck(1)) 
				{
					enemy->hp -= 5;
					player->armor += 5;
					CanUse = true;
				}
			}break;

			}

			if (CanUse) 
			{
				Dick->Use(cardNum);
				UpdateHand(Dick->HandList.size());
				UpdatePlayerState();
				UpdateEnemyState();
			}
			else {} // ���⼭ ����� ���

		}
	}
}

bool BattleScene::ManaCheck(int cost)
{
	if (player->cost - cost < 0) return false;
	else { player->cost -= cost; return true; }
}

void BattleScene::UpdateHand(int handSize)
{
	for (int card = 0; card < handSize; card++)
	{
		for (int ci = 0; ci < 4; ci++)
		{
			CardList[card]->m_pCutInfoList[ci]->tc = CardTextureList[Dick->HandList[card]];
		}
		CardList[card]->m_bIsDead = false;
	}
	
	for (int card = handSize; card < 3; card++) // for (int card = handSize; card < CardList.size(); card++)
	{
		CardList[card]->m_bIsDead = true;
	}
}

void BattleScene::UpdatePlayerState()
{
	CurrentMana->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->cost];
	MaxMana->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->maxCost];

	PlayerCurrenHP1->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->hp / 10];
	PlayerCurrenHP2->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->hp % 10];
	PlayerMaxHP1->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->maxHp / 10];
	PlayerMaxHP2->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->maxHp % 10];

	if (player->armor <= 0) 
	{
		PlayerArmorIcon->m_bIsDead = true;
		PlayerArmor1->m_bIsDead = true;
		PlayerArmor2->m_bIsDead = true;
	}
	else 
	{
		PlayerArmorIcon->m_bIsDead = false;
		PlayerArmor2->m_bIsDead = false;
		PlayerArmor2->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->armor % 10];
		if ((player->armor/10) >= 1) 
		{
			PlayerArmor1->m_OriginPos = { -0.692, -0.795 };
			PlayerArmor2->m_OriginPos = { -0.667, -0.795 };

			PlayerArmor1->m_bIsDead = false;
			PlayerArmor1->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->armor / 10];
		}
		else
		{
			PlayerArmor2->m_OriginPos = { -0.680, -0.795 };
		}
	}
}

void BattleScene::UpdateEnemyState()
{
	if (enemy->hp <= 0) 
	{
		// �� ���� �̺�Ʈ �߻�
		enemy->hp = 0;
	}

	EnemyCurrentHP1->m_pCutInfoList[0]->tc = NumberTextureList_Red[enemy->hp / 10];
	EnemyCurrentHP2->m_pCutInfoList[0]->tc = NumberTextureList_Red[enemy->hp % 10];
	EnemyMaxHP1->m_pCutInfoList[0]->tc = NumberTextureList_Red[enemy->maxHp / 10];
	EnemyMaxHP2->m_pCutInfoList[0]->tc = NumberTextureList_Red[enemy->maxHp % 10];

	// �� �κ�, �� ���߿�..
	/*if (player->armor <= 0)
	{
		PlayerArmorIcon->m_bIsDead = true;
		PlayerArmor1->m_bIsDead = true;
		PlayerArmor2->m_bIsDead = true;
	}
	else
	{
		PlayerArmorIcon->m_bIsDead = false;
		PlayerArmor2->m_bIsDead = false;
		PlayerArmor2->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->armor % 10];
		if ((player->armor / 10) >= 1)
		{
			PlayerArmor1->m_OriginPos = { -0.714, -0.693 };
			PlayerArmor2->m_OriginPos = { -0.693, -0.693 };

			PlayerArmor1->m_bIsDead = false;
			PlayerArmor1->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->armor / 10];
		}
		else
		{
			PlayerArmor2->m_OriginPos = { -0.703, -0.693 };
		}
	}*/
}
