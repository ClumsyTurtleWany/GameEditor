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
#include "SocketComponent.h"
#include "AnimationComponent.h"
#include "UpdateAnimSystem.h"
#include "MovementSystem.h"
#include "Character.h"
#include "SelectAnimSystem.h"
#include "BoundingBoxComponent.h"
#include "CameraArmComponent.h"

//�߰�
#include "ColliderSystem.h"

///////////////////
//Effect Include
///////////////////
#include "EffectInclude/EffectSystem.h"

bool BattleScene::Init()
{
	ID = BATTLE;

	// ��� �÷��̾�� Ÿ��Ʋ���� �� �ʱ�ȭ�ϰ� ���°͵�..
	player = new Player;
	player->cost = player->maxCost;
	player->maxHp = 50;
	player->hp = 50;
	enemy1 = new Enemy_1;
	enemy1->pWorld = &TheWorld;
	enemy1->Init(); 
	enemy1->NumberTextureList_Red = NumberTextureList_Red;
	enemy1->NumberTextureList_Black = NumberTextureList_Black;
	EnemyList.push_back(enemy1);
	enemy2 = new Enemy_2;
	enemy2->pWorld = &TheWorld;
	enemy2->Init();
	enemy2->NumberTextureList_Red = NumberTextureList_Red;
	enemy2->NumberTextureList_Black = NumberTextureList_Black;
	EnemyList.push_back(enemy2);
	// �ؿ� ������ �ӽ�
	enemy2->maxHp = 30;
	enemy2->hp = 30;

	MainCameraSystem = new CameraSystem;
	MainCameraActor = new Actor;
	MainCamera = MainCameraActor->AddComponent<Camera>();
	//Vector3 pt = PlayerCharacter->GetComponent<TransformComponent>()->Translation; // player transform
	//Vector3 et = EnemyCharacter->GetComponent<TransformComponent>()->Translation; // enemy transform
	MainCamera->CreateViewMatrix(Vector3(0.0f, 50.0f, -70.0f), Vector3(150.0f, 20.0f, 50.0f), Vector3(0.0f, 1.0, 0.0f));
	MainCamera->CreateProjectionMatrix(1.0f, 10000.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));
	MainCamera->Roll += 30.0f;
	MainCameraSystem->MainCamera = MainCamera;
	TheWorld.AddEntity(MainCameraActor);

	Init_UI();
	Init_Map();
	Init_Chara();
	Init_Effect();
	Init_Sound();

	// ī�޶� �ý��� �� ������ �ý��� �߰�.
	TheWorld.AddSystem(MainCameraSystem);
	TheWorld.AddSystem(new ColliderSystem);
	MainRenderSystem = new RenderSystem;
	MainRenderSystem->MainCamera = MainCameraSystem->MainCamera;
	TheWorld.AddSystem(MainRenderSystem);

	TheWorld.AddSystem(new WidgetRenderSystem);
	TheWorld.AddSystem(new MovementSystem);
	TheWorld.AddSystem(new UpdateAnimSystem);
	// SelectAnimSystem �߰�
	TheWorld.AddSystem(new SelectAnimSystem);

	LightSystem* lightSystem = new LightSystem;
	lightSystem->MainCamera = MainCameraSystem->MainCamera;
	lightSystem->Initialize();
	TheWorld.AddSystem(lightSystem);

	//MainCameraSystem->MainCamera = PlayerCharacter->GetComponent<Camera>();
	MAIN_PICKER.SetPickingMode(PMOD_CHARACTER);

	return true;
}

bool BattleScene::Frame()
{
	BaseScene::Frame();

	//KeyState btnA = Input::GetInstance()->getKey('A');
	//if (btnA == KeyState::Hold || btnA == KeyState::Down)
	//{
	//	MainCamera->Pitch -= 0.1f;
	//}
	//KeyState btnD = Input::GetInstance()->getKey('D');
	//if (btnD == KeyState::Hold || btnD == KeyState::Down)
	//{
	//	MainCamera->Pitch += 0.1f;
	//}
	//KeyState btnW = Input::GetInstance()->getKey('W');
	//if (btnW == KeyState::Hold || btnW == KeyState::Down)
	//{
	//	MainCamera->Roll -= 0.1f;
	//}
	//KeyState btnS = Input::GetInstance()->getKey('S');
	//if (btnS == KeyState::Hold || btnS == KeyState::Down)
	//{
	//	MainCamera->Roll += 0.1f;
	//}
	//KeyState btnQ = Input::GetInstance()->getKey('Q');
	//if (btnQ == KeyState::Hold || btnQ == KeyState::Down)
	//{
	//	MainCamera->Pos += MainCamera->Look;
	//}
	//KeyState btnE = Input::GetInstance()->getKey('E');
	//if (btnE == KeyState::Hold || btnE == KeyState::Down)
	//{
	//	MainCamera->Pos -= MainCamera->Look;
	//}

	KeyState btnLC = Input::GetInstance()->getKey(VK_LBUTTON);
	if (btnLC == KeyState::Hold || btnLC == KeyState::Down)
	{
		float MinGamDoe = 0.3f; // ���߿� ���콺 �ΰ��� �ʿ��ϸ�?
		//MainCameraSystem->MainCamera->Pitch += MinGamDoe * Input::GetInstance()->m_ptOffset.x;
	}

	// ī�޶� ��ȯ ���ÿ�. �ʿ信 ���� ���� �ٶ�.
	KeyState btnZ = Input::GetInstance()->getKey('Z');
	if (btnZ == KeyState::Hold || btnZ == KeyState::Down)
	{
		MainCameraSystem->MainCamera = PlayerCharacter->GetComponent<Camera>();
	}
	KeyState btnX = Input::GetInstance()->getKey('X');
	if (btnX == KeyState::Hold || btnX == KeyState::Down)
	{
		MainCameraSystem->MainCamera = EnemyCharacter->GetComponent<Camera>();
	}
	KeyState btnC = Input::GetInstance()->getKey('C');
	if (btnC == KeyState::Hold || btnC == KeyState::Down)
	{
		MainCameraSystem->MainCamera = MainCamera;
	}

	// ��� �ϴ� �ֱ������� ������Ʈ �������.
	MainRenderSystem->MainCamera = MainCameraSystem->MainCamera;


	//Effect test
	if (Input::GetInstance()->getKey('U') == KeyState::Up)
	{
		PlayEffect(&TheWorld, L"Hit5", { { 20.0f, 20.0f, 0.0f }, Vector3(), {10.0f, 10.0f, 10.0f} }, { false, 1.0f, 1.0f, 1.0f });
	}


	PickedCharacter = (Character*)MAIN_PICKER.curSelect.pTarget;
	for (auto enemy : EnemyList) 
	{
		if (PickedCharacter == enemy->chara)
		{
			for (auto obj : enemy->ObjList) // ������ ���߿� ������ ���� ���� �����������.. ��
			{
				obj->m_bIsDead = false;
			}
			TargetEnemy = enemy;
		}
		else
		{
			if (enemy != TargetEnemy) 
			{
				for (auto obj : enemy->ObjList)
				{
					obj->m_bIsDead = true;
				}
			}
		}
	}

	if (PickedCharacter != nullptr) 
	{
		int a = 10;
	}

	return true;
}

bool BattleScene::Render()
{
	BaseScene::Render();

	// ���� ī�� Ȯ�� or ���� ī�� Ȯ�� ��ư Ŭ���� �� ��ȯ (ī�� ���� ������)
	if (RemainCardButton->m_bClicked){ SS = REMAINVIEW; RemainCardButton->m_bClicked = false; }
	if (DiscardButton->m_bClicked) { SS = DISCARDVIEW; DiscardButton->m_bClicked = false; }

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
	auto bc = UI->AddComponent<WidgetComponent>();
	Loader.FileLoad(bc, L"../resource/UI/Save/Battle.txt");

	TurnEndButton = bc->FindObj(L"TurnEnd");
	RemainCardButton = bc->FindObj(L"Remain");
	DiscardButton = bc->FindObj(L"Discard");
	MoveButton = bc->FindObj(L"MoveButton");

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
	//UpdatePlayerState();

	UpdateEnemyState();

	// ������, �ϴ� ������ ���ڸ���..
	Damage1 = bc->FindObj(L"Damage_1");
	Damage1->m_bIsDead = true;
	Damage2 = bc->FindObj(L"Damage_2");
	Damage2->m_bIsDead = true;

	// ���� ���忡 ���� �߰�.
	TheWorld.AddEntity(UI);


	// ī�� ��ġ���� ����, �ϴ� 1�� 2�� 3���϶���? ���߿� �ٲ�������� �𸣴� �� ���ϰ� �����صα�
	float CSS = 400.0f; // Card Space Start
	float CSW = 900.0f; // Card Space Width
	CardPosList[0].push_back({ CSS + CSW * 1 / 2, 700.0f });	// 1���϶� ��ġ
	CardPosList[0].push_back({ CSS + CSW * 1 / 3, 700.0f });	// 2���϶�
	CardPosList[0].push_back({ CSS + CSW * 1 / 4, 700.0f });	// 3���϶�
	CardPosList[1].push_back({ 0.0f, 0.0f });
	CardPosList[1].push_back({ CSS + CSW * 2 / 3, 700.0f });
	CardPosList[1].push_back({ CSS + CSW * 2 / 4, 700.0f });
	CardPosList[2].push_back({ 0.0f, 0.0f });
	CardPosList[2].push_back({ 0.0f, 0.0f });
	CardPosList[2].push_back({ CSS + CSW * 3 / 4, 700.0f });
}

void BattleScene::Init_Map()
{	
	//// ���� ���� �߰�.
	Landscape* landscape = new Landscape;
	auto landscapeComponents = landscape->GetComponent<LandscapeComponents>();
	landscapeComponents->Build(16, 16, 7, 10);
	landscapeComponents->SetCamera(MainCameraSystem->MainCamera);
	TheWorld.AddEntity(landscape);
	
	// Sky Dome �߰�.
	Actor* skyDomeActor = new Actor;
	auto skyDomeComp = skyDomeActor->AddComponent<SkyDomeComponent>();
	skyDomeComp->Scale = Vector3(5000.0f, 5000.0f, 5000.0f);
	TheWorld.AddEntity(skyDomeActor);
	SkyRenderSystem* skyRenderSystem = new SkyRenderSystem;
	skyRenderSystem->MainCamera = MainCameraSystem->MainCamera;
	TheWorld.AddSystem(skyRenderSystem);

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

	//Actor* containerShip = new Actor;
	//auto staticMesh = containerShip->AddComponent<StaticMeshComponent>();
	//auto boundBox = containerShip->AddComponent<BoundingBoxComponent>(Vector3(150.0f, 250.0f, 300.0f));
	//if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Map/CargoShip/CargoshipBLEND.fbx"))
	//{
	//	FBXLoader::GetInstance()->GenerateStaticMeshFromFileData(L"../resource/FBX/Map/CargoShip/CargoshipBLEND.fbx", staticMesh);
	//}
	//auto transform = containerShip->GetComponent<TransformComponent>();
	//transform->Scale = Vector3(10.0f, 10.2f, 10.2f);
	////transform->Rotation = Vector3(0.0f, 0.0f, 0.0f);
	////transform->Translation = Vector3(500.0f, 0.0f, -500.0f + static_cast<float>(cnt) * 125);
	//TheWorld.AddEntity(containerShip);

	
}

void BattleScene::Init_Chara()
{
	PlayerCharacter = new Character;
	player->chara = PlayerCharacter;
	auto playerCharMeshComp = PlayerCharacter->AddComponent<SkeletalMeshComponent>();


	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Adam_fbx/Adam.fbx")) 
	{
		FBXLoader::GetInstance()->GenerateSkeletalMeshFromFileData(L"../resource/FBX/Adam_fbx/Adam.fbx", playerCharMeshComp);
	}

	// GenerateAnimationFromFileData()���� �ִϸ��̼� ������Ʈ�� �ִϸ��̼� �߰��ϴ� ��� 
	// ClipList�� ����Ǹ� SetClipByName(name) �Լ��� ���氡�� <- name = Ȯ���ڸ� ������ ���ϸ�
	auto playerCharAnimComp = PlayerCharacter->AddComponent<AnimationComponent>();
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Adam_fbx/Adam_anim/Run.fbx"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Adam_fbx/Adam_anim/Run.fbx", playerCharAnimComp);				// �޸���
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Adam_fbx/Adam_anim/Idle.fbx"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Adam_fbx/Adam_anim/Idle.fbx", playerCharAnimComp);				// ���̵�
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Adam_fbx/Adam_anim/Shooting.fbx"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Adam_fbx/Adam_anim/Shooting.fbx", playerCharAnimComp,false);				// ����
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Adam_fbx/Adam_anim/Hit.fbx"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Adam_fbx/Adam_anim/Hit.fbx", playerCharAnimComp,false);		// �ǰ�
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Adam_fbx/Adam_anim/Dying.fbx"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Adam_fbx/Adam_anim/Dying.fbx", playerCharAnimComp, false);				// ���
	}

	playerCharAnimComp->SetClipByName(L"Idle");



	// ���� ������Ʈ �߰�
	auto socketComp = PlayerCharacter->AddComponent<SocketComponent>();
	// ���̷�Ż �޽� & �� �̸� �Ѱܼ� ���� ����
	socketComp->Attach(*playerCharMeshComp, "RightHand");
	// ������ ���� T R S
	socketComp->SetOffset(Vector3(2.0f, 2.0f, 0.0f), 
						  Vector3(-75.0f, -90.0f, -0.0f), 
						  Vector3(1.0f, 1.0f, 1.0f)); 

	// ����� �� ����ƽ �޽� �߰� - 
	//��ƼƼ�� ���� ������Ʈ�� ���������� ���� ��ġ�� ���� ����ƽ �޽��� ��ġ�� ������Ʈ �ǵ��� ���� �ý��� ����
	auto weaponMesh = PlayerCharacter->AddComponent<StaticMeshComponent>();

	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Adam_fbx/Pistol_fbx/Pistol.FBX"))
	{
		FBXLoader::GetInstance()->GenerateStaticMeshFromFileData(
							L"../resource/FBX/Adam_fbx/Pistol_fbx/Pistol.FBX", weaponMesh);
	}


	auto playerCharTransformComp = PlayerCharacter->GetComponent<TransformComponent>();
	playerCharTransformComp->Scale = Vector3(15.f, 15.f, 15.f);
	playerCharTransformComp->Rotation = Vector3(0.0f, -90.0f, 0.0f);
	playerCharTransformComp->Translation = Vector3(-100.0f, 0.0f, 0.0f);

	auto playerCharMovementComp = PlayerCharacter->GetComponent<MovementComponent>();
	playerCharMovementComp->Speed = 25.0f;
	PlayerCharacter->MoveTo(Vector3(-20.0f, 0.0f, 0.0f));

	//Picking Info Test
	playerCharMeshComp->Name = "player";

	////////////// Bounding Box Add /////////////////
	auto playerOBBComp = PlayerCharacter->AddComponent<BoundingBoxComponent>(Vector3(0.75f, 1.1f, 0.75f), Vector3(0.0f, 1.1f, 0.0f));

	// �÷��̾�� ī�޶� �� ī�޶� �� ����.
	auto playerCamera = PlayerCharacter->AddComponent<Camera>();
	auto playerCameraArm = PlayerCharacter->AddComponent<CameraArmComponent>();
	playerCameraArm->Distance = 100.0f;
	playerCameraArm->Roll = 35.0f;
	playerCameraArm->Pitch = 180.0f - 40.0f;
	playerCamera->CreateViewMatrix(Vector3(0.0f, 25.0f, -100.0f), Vector3(0.0f, 0.0f, 00.0f), Vector3(0.0f, 1.0, 0.0f));
	playerCamera->CreateProjectionMatrix(1.0f, 10000.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));
	
	//MainCamera = PlayerCharacter->AddComponent<Camera>();
	//MainCamera->CreateViewMatrix(Vector3(0.0f, 25.0f, -100.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0, 0.0f));
	//MainCamera->CreateProjectionMatrix(1.0f, 10000.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));
	
	TheWorld.AddEntity(PlayerCharacter);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////ĳ���� �߰�//////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Character* PlayerCharacter_B = new Character;
	enemy2->chara = PlayerCharacter_B;

	auto player_BCharMeshComp = PlayerCharacter_B->AddComponent<SkeletalMeshComponent>();
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Wolverine_fbx/WOLVERINE.FBX"))
	{
		FBXLoader::GetInstance()->GenerateSkeletalMeshFromFileData(L"../resource/FBX/Wolverine_fbx/WOLVERINE.FBX", player_BCharMeshComp);
	}

	auto player_BCharAnimComp = PlayerCharacter_B->AddComponent<AnimationComponent>();
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Wolverine_fbx/Wolverine_Anim/Run.FBX"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Wolverine_fbx/Wolverine_Anim/Run.FBX", player_BCharAnimComp);					// �޸���
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Wolverine_fbx/Wolverine_Anim/Idle.FBX"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Wolverine_fbx/Wolverine_Anim/Idle.FBX", player_BCharAnimComp);				// ���̵�
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Wolverine_fbx/Wolverine_Anim/Attack.FBX"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Wolverine_fbx/Wolverine_Anim/Attack.FBX", player_BCharAnimComp, false);				// ����
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Wolverine_fbx/Wolverine_Anim/Hit.FBX"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Wolverine_fbx/Wolverine_Anim/Hit.FBX", player_BCharAnimComp, false);			// �ǰ�
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Wolverine_fbx/Wolverine_Anim/Dying.FBX"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Wolverine_fbx/Wolverine_Anim/Dying.FBX", player_BCharAnimComp, false);				// ���
	}

	auto player_BCharTransformComp = PlayerCharacter_B->GetComponent<TransformComponent>();
	player_BCharTransformComp->Scale = Vector3(13.f, 13.f, 13.f);
	player_BCharTransformComp->Rotation = Vector3(0.0f, 90.0f, 0.0f);
	player_BCharTransformComp->Translation = Vector3(0.0f, 0.0f, 200.0f);

	auto player_BCharMovementComp = PlayerCharacter_B->GetComponent<MovementComponent>();
	player_BCharMovementComp->Speed = 25.0f;
	PlayerCharacter_B->MoveTo(Vector3(20.0f, 0.0f, 70.0f));

	/////////////// Bounding Box Add ////////////
	auto player_BOBBComp = PlayerCharacter_B->AddComponent<BoundingBoxComponent>(Vector3(0.5f, 0.9f, 0.5f), Vector3(0.0f, 0.9f, 0.0f));

	TheWorld.AddEntity(PlayerCharacter_B);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////// �� �׽�Ʈ //////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	EnemyCharacter = new Character;
	enemy1->chara = EnemyCharacter;

	auto enemyCharMeshComp = EnemyCharacter->AddComponent<SkeletalMeshComponent>();

	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Monster_fbx/Monster.fbx"))

	{

		FBXLoader::GetInstance()->GenerateSkeletalMeshFromFileData(L"../resource/FBX/Monster_fbx/Monster.fbx", enemyCharMeshComp);

	}

	auto enemyCharAnimComp = EnemyCharacter->AddComponent<AnimationComponent>();

	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Monster_fbx/MonsterAnim/Run.fbx"))

	{

		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Monster_fbx/MonsterAnim/Run.fbx", enemyCharAnimComp);			// �޸���

	}

	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Monster_fbx/MonsterAnim/Idle.fbx"))

	{

		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Monster_fbx/MonsterAnim/Idle.fbx", enemyCharAnimComp);			// ���̵�

	}

	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Monster_fbx/MonsterAnim/Kick.fbx"))

	{

		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Monster_fbx/MonsterAnim/Kick.fbx", enemyCharAnimComp, false);			// ����

	}

	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Monster_fbx/MonsterAnim/Hit.fbx"))

	{

		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Monster_fbx/MonsterAnim/Hit.fbx", enemyCharAnimComp, false);			// �ǰ�

	}

	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Monster_fbx/MonsterAnim/Dying.fbx"))

	{

		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Monster_fbx/MonsterAnim/Dying.fbx", enemyCharAnimComp);			// ���

	}
	enemyCharAnimComp->SetClipByName(L"Idle");

	auto enemyCharTransformComp = EnemyCharacter->GetComponent<TransformComponent>();


	enemyCharTransformComp->Scale = Vector3(10.f, 10.f, 10.f);
	enemyCharTransformComp->Rotation = Vector3(0.0f, 90.0f, 0.0f);
	enemyCharTransformComp->Translation = Vector3(100.0f, 0.0f, 0.0f);

	auto E1MC = EnemyCharacter->GetComponent<MovementComponent>();
	E1MC->Speed = 25.0f;
	EnemyCharacter->MoveTo(Vector3(20.0f, 0.0f, 0.0f));

	//auto enemyCharMovementComp = EnemyCharacter->GetComponent<MovementComponent>();
	//enemyCharMovementComp->Speed = 25.0f;
	//EnemyCharacter->MoveTo(Vector3(-20.0f, 0.0f, 0.0f));

	//Picking Info Test
	enemyCharMeshComp->Name = "Enemy";

	/////////////// Bounding Box Add ////////////
	auto enemyOBBComp = EnemyCharacter->AddComponent<BoundingBoxComponent>(Vector3(0.2f, 0.45f, 0.2f), Vector3(0.0f, 0.45f, 0.0f));

	// �� ĳ���� �� ī�޶� �� ī�޶� ��
	auto enemyCamera = EnemyCharacter->AddComponent<Camera>();
	auto enemyCameraArm = EnemyCharacter->AddComponent<CameraArmComponent>();
	enemyCameraArm->Distance = 100.0f;
	enemyCameraArm->Roll = 35.0f;
	enemyCameraArm->Pitch = 180.0f + 40.0f;
	enemyCamera->CreateViewMatrix(Vector3(0.0f, 25.0f, -100.0f), Vector3(0.0f, 0.0f, 00.0f), Vector3(0.0f, 1.0, 0.0f));
	enemyCamera->CreateProjectionMatrix(1.0f, 10000.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));

	TheWorld.AddEntity(EnemyCharacter);

}

void BattleScene::Init_Effect()
{	
	//Effect Test
	ECS::EffectSystem* ESystem = new ECS::EffectSystem;
	ESystem->init(&TheWorld);
	TheWorld.AddSystem(ESystem);
	
	//ParticleEffect* testEffect1 = new ParticleEffect(L"Fire", { true, 10.0, 0.0, 1.0 });
	//ParticleEffect* testEffect2 = new ParticleEffect(L"Smoke", { true, 0.5, 0.0, 1.0 });
	//ParticleEffect* testEffect3 = new ParticleEffect(L"Spark", { true, 0.5, 0.0, 1.0 });
	//ParticleEffect* testEffect4 = new ParticleEffect(L"Hit1", { true, 0.5, 0.0, 1.0 });
	//ParticleEffect* testEffect5 = new ParticleEffect(L"Hit2", { true, 0.5, 0.0, 1.0 });
	//ParticleEffect* testEffect6 = new ParticleEffect(L"Hit3", { true, 0.5, 0.0, 1.0 });
	//ParticleEffect* testEffect7 = new ParticleEffect(L"Hit4", { true, 0.5, 0.0, 1.0 });
	//ParticleEffect* testEffect8 = new ParticleEffect(L"Hit5", { true, 0.5, 0.0, 1.0 });

	//auto testEffectTransform1 = testEffect1->GetComponent<TransformComponent>();
	//auto testEffectTransform2 = testEffect2->GetComponent<TransformComponent>();
	//auto testEffectTransform3 = testEffect3->GetComponent<TransformComponent>();
	//auto testEffectTransform4 = testEffect4->GetComponent<TransformComponent>();
	//auto testEffectTransform5 = testEffect5->GetComponent<TransformComponent>();
	//auto testEffectTransform6 = testEffect6->GetComponent<TransformComponent>();
	//auto testEffectTransform7 = testEffect7->GetComponent<TransformComponent>();
	//auto testEffectTransform8 = testEffect8->GetComponent<TransformComponent>();

	//testEffectTransform1->Translation = { 10.0f, 50.0f, 0.0f };
	//testEffectTransform2->Translation = { 20.0f, 50.0f, 0.0f };
	//testEffectTransform3->Translation = { 30.0f, 50.0f, 0.0f };
	//testEffectTransform4->Translation = { 40.0f, 50.0f, 0.0f };
	//testEffectTransform5->Translation = { 50.0f, 50.0f, 0.0f };
	//testEffectTransform6->Translation = { 60.0f, 50.0f, 0.0f };
	//testEffectTransform7->Translation = { 70.0f, 50.0f, 0.0f };
	//testEffectTransform8->Translation = { 80.0f, 50.0f, 0.0f };

	//TheWorld.AddEntity(testEffect1);
	//TheWorld.AddEntity(testEffect2);
	//TheWorld.AddEntity(testEffect3);
	//TheWorld.AddEntity(testEffect4);
	//TheWorld.AddEntity(testEffect5);
	//TheWorld.AddEntity(testEffect6);
	//TheWorld.AddEntity(testEffect7);
	//TheWorld.AddEntity(testEffect8);

	//PlayEffect(&TheWorld, L"Hit5", { Vector3(), Vector3(), Vector3(100.0f, 100.0f, 100.0f) });
}

void BattleScene::Init_Sound()
{
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/Effect/Attack_Bludgeon.ogg", SoundType::BGM))
	{
		FMODSound* Hit1 = FMODSoundManager::GetInstance()->GetSound(L"Attack_Bludgeon.ogg");
		Hit1->Play();
		Hit1->SetVolume(0.5); // ���� 0 ~ 1 ���� ��.
		Hit1->SetLoop(false); // Effect���� Loop true ����.
		Hit1->Stop();
		SoundMap.insert(std::make_pair(L"Hit1", Hit1));
	}
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/Effect/Attack_Strike.ogg", SoundType::BGM))
	{
		FMODSound* Hit2 = FMODSoundManager::GetInstance()->GetSound(L"Attack_Strike.ogg");
		Hit2->Play();
		Hit2->SetVolume(0.5); // ���� 0 ~ 1 ���� ��.
		Hit2->SetLoop(false); // Effect���� Loop true ����.
		Hit2->Stop();
		SoundMap.insert(std::make_pair(L"Hit2", Hit2));
	}
}

void BattleScene::CameraMove(Vector3& eye, Vector3& target)
{
}

void BattleScene::BattleProcess()
{
	if (TurnStart) { TurnStartProcess(); }
	if (TurnEndButton->m_bClicked) { TurnEndProcess(); }
	if (MoveButton->m_bClicked) { IsMoving = true; MoveButton->m_bClicked = false; MAIN_PICKER.optPickingMode = PMOD_LAND; }
	
	EnemyAttackAnimProcess();
	CardCheck();
	DeadCheck();
	if (IsMoving) { MoveProcess(); }
}

void BattleScene::TurnStartProcess()
{
	TurnNum++;
	player->armor = 0;
	player->cost = player->maxCost;
	UpdatePlayerState();

	for (auto enemy : EnemyList) { enemy->SetIntentObj(TurnNum, enemy->IntentIcon, enemy->Intent1, enemy->Intent2); }
	
	int drawNum = 3;
	for (int i = 0; i < drawNum; i++) { CardList[i]->m_bIsDead = false; }

	Dick->Draw(drawNum);
	UpdateHand(drawNum);

	TurnStart = false;
}

void BattleScene::TurnEndProcess()
{
	TurnNum++;

	for (int card = 0; card < Dick->HandList.size(); card++)
	{
		CardList[card]->m_bIsDead = false;
		Vector2 AnimPos[2] = { CardList[card]->NtoP_Pos(CardList[card]->m_OriginalOriginPos), {1800.0f,700.0f} };
		CardList[card]->SetAnimation(AnimPos, 0.5f);
	}
	Dick->TurnEnd();

	EnemyTurnProcess();

	TurnEndButton->m_bClicked = false;
}

void BattleScene::EnemyTurnProcess()
{
	TurnState = false;

	InActionEnemy = EnemyList[0];
	EnemyList[0]->patern(player, TurnNum);

	UpdatePlayerState();
	UpdateEnemyState();
	PlayEffect(&TheWorld, L"Hit5", { {0.0f, 10.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {5.0f, 5.0f, 5.0f} }, { false, 0.5f, 0.2f, 1.0f });
	SoundMap.find(L"Hit2")->second->Play();
}

void BattleScene::EnemyAttackAnimProcess()
{	
	// �� ���̰�, ���� �ൿ���̾��� ���� �ൿ�� ������ ����
	if (!TurnState && !InActionEnemy->chara->GetComponent<AnimationComponent>()->IsInAction())
	{
		if (InActionEnemy == EnemyList[EnemyList.size() - 1]) // ���� �ൿ���� ���� ������ ���̶��  
		{
			TurnState = true;
			TurnStart = true;
			InActionEnemyNum = 0;
			return; 
		}

		InActionEnemyNum++;
		InActionEnemy = EnemyList[InActionEnemyNum];
		InActionEnemy->patern(player, TurnNum);
		UpdatePlayerState();
		UpdateEnemyState();
		PlayEffect(&TheWorld, L"Hit5", { {0.0f, 10.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {5.0f, 5.0f, 5.0f} }, { false, 0.5f, 0.2f, 1.0f });
		SoundMap.find(L"Hit2")->second->Play();
	}
}

static float MoveTimer = 0.0f;
void BattleScene::MoveProcess()
{
	// �������ַ���
	MoveTimer += Timer::GetInstance()->GetDeltaTime();

	if (MoveTimer >= 0.5f)
	{
		// ī�޶� �� �ٲ��ְ�

		// ��ŷ�� ���� ǥ�����ִ� ����Ʈ
		PlayEffect(&TheWorld, L"Hit5", { MAIN_PICKER.vIntersection, {0.0f, 0.0f, 0.0f}, {2.0f, 2.0f, 2.0f} }, { false, 0.5f, 0.2f, 1.0f });

		// �� Ŭ���ϸ� �۷� �̵�
		KeyState btnLC = Input::GetInstance()->getKey(VK_LBUTTON);
		if (btnLC == KeyState::Down) 
		{ 
			PlayerCharacter->MoveTo(MAIN_PICKER.vIntersection);
			IsMoving = false; 
			MoveTimer = 0.0f; 
			MAIN_PICKER.optPickingMode = PMOD_CHARACTER;
		}
	}
}

void BattleScene::CardCheck()
{
	//auto PAnime = PlayerCharacter->GetComponent<AnimationComponent>();
	//auto EAnime = EnemyCharacter->GetComponent<AnimationComponent>();
	//if (!PAnime->IsInAction()) { MainCameraSystem->MainCamera = PlayerCharacter->GetComponent<Camera>(); }

	if (TargetEnemy == nullptr) { return; } // Ÿ���� ���ٸ� ���त

	for (int cardNum=0; cardNum<Dick->HandList.size(); cardNum++) 
	{
		if (CardList[cardNum]->m_bClicked && CardList[cardNum]->m_OriginPos.y >= 0.5)
		{
			bool CanUse = false;

			CardList[cardNum]->m_bClicked = false;
			CardList[cardNum]->m_OriginPos = CardList[cardNum]->m_OriginalOriginPos;

			auto PAnime = PlayerCharacter->GetComponent<AnimationComponent>();
			auto EAnime = TargetEnemy->chara->GetComponent<AnimationComponent>();

			switch (Dick->HandList[cardNum])
			{

			case Strike:
			{
				if (ManaCheck(1))
				{
					// ������ ����Ʈ �ʾ�.. �� �����̱⵵ �ϰ� ��Ÿ���鼭 Ŀ���ٰ� �۾����ٰ��� �ؾ��ϴµ� �� ���� ���߿� �Լ���? ���� ��
					//Damage2->m_bIsDead = false;
					//Damage2->m_pCutInfoList[0]->tc = NumberTextureList_Red[6];
					TargetEnemy->hp -= 6;
					CanUse = true;

					PAnime->SetClipByName(L"Shooting");
					EAnime->SetClipByName(L"Hit"); // �� �ǰ� ���
					PlayEffect(&TheWorld, L"Hit5", { {10.0f, 10.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {5.0f, 5.0f, 5.0f} }, { false, 0.5f, 0.2f, 1.0f });
					SoundMap.find(L"Hit1")->second->Play();
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
					TargetEnemy->hp -= 9;
					Dick->Draw(1);
					CanUse = true;

					PAnime->SetClipByName(L"Shooting");
					EAnime->SetClipByName(L"Hit");
					PlayEffect(&TheWorld, L"Hit5", { {10.0f, 10.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {5.0f, 5.0f, 5.0f} }, { false, 0.5f, 0.2f, 1.0f });
					SoundMap.find(L"Hit1")->second->Play();
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
					TargetEnemy->hp -= 5;
					player->armor += 5;
					CanUse = true;

					PAnime->SetClipByName(L"Shooting");
					EAnime->SetClipByName(L"Hit");
					PlayEffect(&TheWorld, L"Hit5", { {10.0f, 10.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {5.0f, 5.0f, 5.0f} }, { false, 0.5f, 0.2f, 1.0f });
					SoundMap.find(L"Hit1")->second->Play();
				}
			}break;

			}

			if (CanUse) 
			{
				Dick->Use(cardNum);
				UpdateHand(Dick->HandList.size(), cardNum);
				UpdatePlayerState();
				UpdateEnemyState();

				MainCameraSystem->MainCamera = MainCamera;
			}
			else {} // ���⼭ ������� ���

		}
	}
}

bool BattleScene::ManaCheck(int cost)
{
	if (player->cost - cost < 0) return false;
	else { player->cost -= cost; return true; }
}

// �� ���۽�
void BattleScene::UpdateHand(int handSize)
{
	for (int card = 0; card < handSize; card++)
	{
		for (int ci = 0; ci < 4; ci++)
		{
			CardList[card]->m_pCutInfoList[ci]->tc = CardTextureList[Dick->HandList[card]];
		}
		CardList[card]->m_bIsDead = false;

		// Vector2 AnimPos[2] = { CardList[card]->NtoP_Pos(CardList[card]->m_OriginalOriginPos), CardPosList[card][handSize - 1] };
		Vector2 AnimPos[2] = { { -200.0f, 700.0f }, CardPosList[card][handSize - 1] };
		CardList[card]->SetAnimation(AnimPos, 0.5f);
	}
	
	for (int card = handSize; card < 3; card++) // for (int card = handSize; card < CardList.size(); card++)
	{
		CardList[card]->m_bIsDead = true;
	}
}

// ī�� ������ 
void BattleScene::UpdateHand(int handSize, int UsedCard)
{
	for (int card = 0; card < handSize; card++)
	{
		for (int ci = 0; ci < 4; ci++)
		{
			CardList[card]->m_pCutInfoList[ci]->tc = CardTextureList[Dick->HandList[card]];
		}
		CardList[card]->m_bIsDead = false;

		Vector2 AnimPos[2] = { CardList[card]->NtoP_Pos(CardList[card]->m_OriginalOriginPos), CardPosList[card][handSize - 1] };
		if (card >= UsedCard) { AnimPos[0] = CardList[card]->NtoP_Pos(CardList[card+1]->m_OriginalOriginPos); }
		CardList[card]->SetAnimation(AnimPos, 0.5f);
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

	if (player->hp <= 0)
	{
		auto PAnime = PlayerCharacter->GetComponent<AnimationComponent>();
		if (PAnime->CurrentClipName != L"Dying") { PAnime->SetClipByName(L"Dying"); }  // �������� �ִϸ��̼� �� �� ������� �ʵ���
	}


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
			PlayerArmor1->m_bIsDead = true;
			PlayerArmor2->m_OriginPos = { -0.680, -0.795 };
		}
	}
}

void BattleScene::UpdateEnemyState()
{
	for (auto enemy : EnemyList) { enemy->UpdateState(); }
}

void BattleScene::DeadCheck()
{
	if (player->hp <= 0 && !PlayerCharacter->GetComponent<AnimationComponent>()->IsInAction())
	{
		SS = GAMEOVER;
	}
	else 
	{
		for (auto enemy : EnemyList) 
		{
			if (enemy->hp > 0 || enemy->chara->GetComponent<AnimationComponent>()->IsInAction()) return; // ���� �ϳ��� ����ְų� �״� �ִϸ��̼����̶�� Ż��!
		}
		SS = CLEAR;
	}
}