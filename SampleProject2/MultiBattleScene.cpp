#include "MultiBattleScene.h"
#include "LightSystem.h"
#include "DirectionalLight.h"
#include "SkyBoxComponent.h"
#include "FBXLoader.hpp"
#include "Landscape.h"
#include "SkeletalMeshComponent.h"
#include "MaterialManager.h"
#include "SkyRenderSystem.h"
#include "SkyDomeComponent.h"
//추가
#include "AnimationComponent.h"
#include "UpdateAnimSystem.h"
#include "MovementSystem.h"
#include "Character.h"
#include "SelectAnimSystem.h"
#include "BoundingBoxComponent.h"
#include "CameraArmComponent.h"

//추가
#include "ColliderSystem.h"

///////////////////
//Effect Include
///////////////////
#include "EffectInclude/EffectSystem.h"

bool MultiBattleScene::Init()
{
	ID = MULTIBATTLE;

	// 이쪽은 map들어갈때쯤 거기서 초기화해주는게 낫겠는데.. 아님 샘플에서?
	player1 = new Player;
	player1->cost = player1->maxCost;
	player1->maxHp = 50;
	player1->hp = 50;
	player2 = new Player;
	player2->cost = player2->maxCost;
	player2->maxHp = 50;
	player2->hp = 50;

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
	// 밑에 두줄은 임시
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

	// 카메라 시스템 및 랜더링 시스템 추가.
	TheWorld.AddSystem(MainCameraSystem);
	TheWorld.AddSystem(new ColliderSystem);
	MainRenderSystem = new RenderSystem;
	MainRenderSystem->MainCamera = MainCameraSystem->MainCamera;
	TheWorld.AddSystem(MainRenderSystem);

	TheWorld.AddSystem(new WidgetRenderSystem);
	TheWorld.AddSystem(new MovementSystem);
	TheWorld.AddSystem(new UpdateAnimSystem);
	// SelectAnimSystem 추가
	TheWorld.AddSystem(new SelectAnimSystem);

	LightSystem* lightSystem = new LightSystem;
	lightSystem->MainCamera = MainCameraSystem->MainCamera;
	lightSystem->Initialize();
	TheWorld.AddSystem(lightSystem);

	//MainCameraSystem->MainCamera = PlayerCharacter->GetComponent<Camera>();

	return true;
}

bool MultiBattleScene::Frame()
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
		float MinGamDoe = 0.3f; // 나중에 마우스 민감도 필요하면?
		//MainCameraSystem->MainCamera->Pitch += MinGamDoe * Input::GetInstance()->m_ptOffset.x;
	}

	// 카메라 전환 예시용. 필요에 따라 수정 바람.
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

	// 얘는 일단 주기적으로 업데이트 해줘야함.
	MainRenderSystem->MainCamera = MainCameraSystem->MainCamera;

	//PlayerCharacter->MoveTo(MAIN_PICKER.Intersection);

	//Effect test
	if (Input::GetInstance()->getKey('U') == KeyState::Up)
	{
		PlayEffect(&TheWorld, L"Hit5", { { 20.0f, 20.0f, 0.0f }, Vector3(), {10.0f, 10.0f, 10.0f} }, { false, 1.0f, 1.0f, 1.0f });
	}

	// 선택된 적만 상태창 출력, 이쪽은 나중에 빌보드 띄우고 나면 쓸모없을지도.. 흠
	PickedCharacter = (Character*)MAIN_PICKER.lastSelect.pTarget;
	for (auto enemy : EnemyList) 
	{
		if (PickedCharacter == enemy->chara)
		{
			for (auto obj : enemy->ObjList) 
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

	return true;
}

bool MultiBattleScene::Render()
{
	BaseScene::Render();

	// 남은 카드 확인 or 버린 카드 확인 버튼 클릭시 씬 전환 (카드 보는 씬으로)
	if (RemainCardButton->m_bClicked){ SS = REMAINVIEW; RemainCardButton->m_bClicked = false; }
	if (DiscardButton->m_bClicked) { SS = DISCARDVIEW; DiscardButton->m_bClicked = false; }

	//대충 여기서 뭔가 돌아가면 될성싶은디
	BattleProcess();

	return true;
}

bool MultiBattleScene::Release()
{
	BaseScene::Release();
    return true;
}

void MultiBattleScene::Init_UI()
{
	UI_Loader Loader;

	// Actor 생성
	Actor* UI = new Actor;
	auto bc = UI->AddComponent<WidgetComponent>();
	Loader.FileLoad(bc, L"../resource/UI/Save/Battle.txt");

	TurnEndButton = bc->FindObj(L"TurnEnd");
	RemainCardButton = bc->FindObj(L"Remain");
	DiscardButton = bc->FindObj(L"Discard");

	// 카드 오브젝트 생성 후 리스트에 넣어줌
	CardList[0] = bc->FindObj(L"Card1");
	CardList[0]->m_bDraggable = true;
	CardList[1] = bc->FindObj(L"Card2");
	CardList[1]->m_bDraggable = true;
	CardList[2] = bc->FindObj(L"Card3");
	CardList[2]->m_bDraggable = true;

	// 플레이어 거시기 표시용 
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

	// 적 상태, BaseEnemy의 Init으로 넘어감
	//EnemyCurrentHP1 = bc->FindObj(L"EnemyCurrentHp_1");
	//EnemyCurrentHP2 = bc->FindObj(L"EnemyCurrentHp_2");
	//EnemyMaxHP1 = bc->FindObj(L"EnemyMaxHp_1");
	//EnemyMaxHP2 = bc->FindObj(L"EnemyMaxHp_2");
	//EnemyIntentIcon = bc->FindObj(L"EnemyIntent");
	//EnemyIntent1 = bc->FindObj(L"EnemyIntent_1");
	//EnemyIntent2 = bc->FindObj(L"EnemyIntent_2");
	//EnemyStateObjectList.push_back(EnemyCurrentHP1);
	//EnemyStateObjectList.push_back(EnemyCurrentHP2);
	//EnemyStateObjectList.push_back(EnemyMaxHP1);
	//EnemyStateObjectList.push_back(EnemyMaxHP2);
	//EnemyStateObjectList.push_back(EnemyIntentIcon);
	//EnemyStateObjectList.push_back(EnemyIntent1);
	//EnemyStateObjectList.push_back(EnemyIntent2);
	//EnemyStateObjectList.push_back(bc->FindObj(L"EnemyInfoBG"));
	//EnemyStateObjectList.push_back(bc->FindObj(L"Slash2"));
	UpdateEnemyState();

	// 데미지, 일단 공용인 두자리만..
	Damage1 = bc->FindObj(L"Damage_1");
	Damage1->m_bIsDead = true;
	Damage2 = bc->FindObj(L"Damage_2");
	Damage2->m_bIsDead = true;

	// 메인 월드에 액터 추가.
	TheWorld.AddEntity(UI);
}

void MultiBattleScene::Init_Map()
{	
	//// 지형 액터 추가.
	Landscape* landscape = new Landscape;
	auto landscapeComponents = landscape->GetComponent<LandscapeComponents>();
	landscapeComponents->Build(16, 16, 7, 10);
	landscapeComponents->SetCamera(MainCameraSystem->MainCamera);
	TheWorld.AddEntity(landscape);
	
	// Sky Dome 추가.
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

void MultiBattleScene::Init_Chara()
{
	//PlayerCharacter = new Character;
	//player->chara = PlayerCharacter;
	//auto playerCharMeshComp = PlayerCharacter->AddComponent<SkeletalMeshComponent>();
	//
	//if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Hulk_fbx/HULK.FBX")) //hulk_removeTwist
	//{
	//	FBXLoader::GetInstance()->GenerateSkeletalMeshFromFileData(L"../resource/FBX/Hulk_fbx/HULK.FBX", playerCharMeshComp);
	//}

	//// GenerateAnimationFromFileData()에서 애니메이션 컴포넌트에 애니메이션 추가하는 방식 
	//// ClipList에 저장되며 SetClipByName(name) 함수로 변경가능 <- name = 확장자명 제외한 파일명
	//auto playerCharAnimComp = PlayerCharacter->AddComponent<AnimationComponent>();
	//if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Hulk_fbx/Run.FBX"))
	//{
	//	FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Hulk_fbx/Run.FBX", playerCharAnimComp);				// 달리기
	//}
	//if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Hulk_fbx/Idle.FBX"))
	//{
	//	FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Hulk_fbx/Idle.FBX", playerCharAnimComp);				// 아이들
	//}
	//if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Hulk_fbx/Punch.FBX"))
	//{
	//	FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Hulk_fbx/Punch.FBX", playerCharAnimComp);				// 공격
	//}
	//if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Hulk_fbx/Stomach_Hit.FBX"))
	//{
	//	FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Hulk_fbx/Stomach_Hit.FBX", playerCharAnimComp);		// 피격
	//}
	//if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Hulk_fbx/Dying.FBX"))
	//{
	//	FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Hulk_fbx/Dying.FBX", playerCharAnimComp);				// 사망
	//}
	//
	//playerCharAnimComp->SetClipByName(L"Punch");
	//playerCharAnimComp->CurrentClip->LoopState = false;
	//playerCharAnimComp->SetClipByName(L"Stomach_Hit");
	//playerCharAnimComp->CurrentClip->LoopState = false;
	//playerCharAnimComp->SetClipByName(L"Idle");

	//auto weaponMeshComp = PlayerCharacter->AddComponent<WeaponMeshComponent>();

	////weaponMeshComp->Attach(*playerCharMeshComp, "Bip001 L Hand");
	//weaponMeshComp->Attach(*playerCharMeshComp, "Bip001 R Hand");

	//if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Map/Warehouse/Warehouse.FBX")) 
	//{
	//	FBXLoader::GetInstance()->GenerateWeaponMeshFromFileData(L"../resource/FBX/Map/Warehouse/Warehouse.FBX", weaponMeshComp);
	//}

	////////////////////////////////test////////////////////////
	

	PlayerCharacter = new Character;
	player1->chara = PlayerCharacter;
	auto playerCharMeshComp = PlayerCharacter->AddComponent<SkeletalMeshComponent>();


	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Adam_fbx/Adam.fbx")) 
	{
		FBXLoader::GetInstance()->GenerateSkeletalMeshFromFileData(L"../resource/FBX/Adam_fbx/Adam.fbx", playerCharMeshComp);
	}

	// GenerateAnimationFromFileData()에서 애니메이션 컴포넌트에 애니메이션 추가하는 방식 
	// ClipList에 저장되며 SetClipByName(name) 함수로 변경가능 <- name = 확장자명 제외한 파일명
	auto playerCharAnimComp = PlayerCharacter->AddComponent<AnimationComponent>();
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Adam_fbx/Adam_anim/Run.fbx"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Adam_fbx/Adam_anim/Run.fbx", playerCharAnimComp);				// 달리기
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Adam_fbx/Adam_anim/Idle.fbx"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Adam_fbx/Adam_anim/Idle.fbx", playerCharAnimComp);				// 아이들
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Adam_fbx/Adam_anim/Shooting.fbx"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Adam_fbx/Adam_anim/Shooting.fbx", playerCharAnimComp,false);				// 공격
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Adam_fbx/Adam_anim/Hit.fbx"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Adam_fbx/Adam_anim/Hit.fbx", playerCharAnimComp,false);		// 피격
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Adam_fbx/Adam_anim/Dying.fbx"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Adam_fbx/Adam_anim/Dying.fbx", playerCharAnimComp, false);				// 사망
	}

	playerCharAnimComp->SetClipByName(L"Idle");

	//auto weaponMeshComp = PlayerCharacter->AddComponent<WeaponMeshComponent>();

	////weaponMeshComp->Attach(*playerCharMeshComp, "Bip001 L Hand");
	//weaponMeshComp->Attach(*playerCharMeshComp, "Bip001 R Hand");

	//if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Map/Warehouse/Warehouse.FBX"))
	//{
	//	FBXLoader::GetInstance()->GenerateWeaponMeshFromFileData(L"../resource/FBX/Map/Warehouse/Warehouse.FBX", weaponMeshComp);
	//}



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

	// 플레이어용 카메라 및 카메라 암 설정.
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
	///////////////////////////////2P 캐릭터 추가//////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Character* Chara_2P = new Character;
	player2->chara = Chara_2P;

	auto Char2PMeshComp = Chara_2P->AddComponent<SkeletalMeshComponent>();
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/WinterSoldier_fbx/WINTERSOLDIER.fbx"))
	{
		FBXLoader::GetInstance()->GenerateSkeletalMeshFromFileData(L"../resource/FBX/WinterSoldier_fbx/WINTERSOLDIER.fbx", Char2PMeshComp);
	}

	auto Char2PAnimComp = Chara_2P->AddComponent<AnimationComponent>();
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/WinterSoldier_fbx/WS_Anim/Run.FBX"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/WinterSoldier_fbx/WS_Anim/Run.FBX", Char2PAnimComp);					// 달리기
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/WinterSoldier_fbx/WS_Anim/Idle.FBX"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/WinterSoldier_fbx/WS_Anim/Idle.FBX", Char2PAnimComp);				// 아이들
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/WinterSoldier_fbx/WS_Anim/Punch.FBX"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/WinterSoldier_fbx/WS_Anim/Punch.FBX", Char2PAnimComp, false);				// 공격
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/WinterSoldier_fbx/WS_Anim/Hit.FBX"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/WinterSoldier_fbx/WS_Anim/Hit.FBX", Char2PAnimComp, false);			// 피격
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/WinterSoldier_fbx/WS_Anim/Dying.FBX"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/WinterSoldier_fbx/WS_Anim/Dying.FBX", Char2PAnimComp, false);				// 사망
	}

	auto Char2PTransformComp = Chara_2P->GetComponent<TransformComponent>();
	Char2PTransformComp->Scale = Vector3(13.f, 13.f, 13.f);
	Char2PTransformComp->Rotation = Vector3(0.0f, 90.0f, 0.0f);
	Char2PTransformComp->Translation = Vector3(0.0f, 0.0f, 200.0f);

	auto Char2PMovementComp = Chara_2P->GetComponent<MovementComponent>();
	Char2PMovementComp->Speed = 25.0f;
	Chara_2P->MoveTo(Vector3(-20.0f, 0.0f, 40.0f));

	/////////////// Bounding Box Add ////////////
	auto BBComp = Chara_2P->AddComponent<BoundingBoxComponent>(Vector3(0.5f, 0.9f, 0.5f), Vector3(0.0f, 0.9f, 0.0f));

	TheWorld.AddEntity(Chara_2P);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////캐릭터 추가//////////////////////////////////////////////////////////////////
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
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Wolverine_fbx/Wolverine_Anim/Run.FBX", player_BCharAnimComp);					// 달리기
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Wolverine_fbx/Wolverine_Anim/Idle.FBX"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Wolverine_fbx/Wolverine_Anim/Idle.FBX", player_BCharAnimComp);				// 아이들
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Wolverine_fbx/Wolverine_Anim/Attack.FBX"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Wolverine_fbx/Wolverine_Anim/Attack.FBX", player_BCharAnimComp, false);				// 공격
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Wolverine_fbx/Wolverine_Anim/Hit.FBX"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Wolverine_fbx/Wolverine_Anim/Hit.FBX", player_BCharAnimComp, false);			// 피격
	}
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Wolverine_fbx/Wolverine_Anim/Dying.FBX"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Wolverine_fbx/Wolverine_Anim/Dying.FBX", player_BCharAnimComp, false);				// 사망
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
	/////////////////////////////////////////// 적 테스트 //////////////////////////////////////////////////////
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

		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Monster_fbx/MonsterAnim/Run.fbx", enemyCharAnimComp);			// 달리기

	}

	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Monster_fbx/MonsterAnim/Idle.fbx"))

	{

		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Monster_fbx/MonsterAnim/Idle.fbx", enemyCharAnimComp);			// 아이들

	}

	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Monster_fbx/MonsterAnim/Kick.fbx"))

	{

		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Monster_fbx/MonsterAnim/Kick.fbx", enemyCharAnimComp, false);			// 공격

	}

	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Monster_fbx/MonsterAnim/Hit.fbx"))

	{

		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Monster_fbx/MonsterAnim/Hit.fbx", enemyCharAnimComp, false);			// 피격

	}

	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Monster_fbx/MonsterAnim/Dying.fbx"))

	{

		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Monster_fbx/MonsterAnim/Dying.fbx", enemyCharAnimComp);			// 사망

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

	// 적 캐릭터 용 카메라 및 카메라 암
	auto enemyCamera = EnemyCharacter->AddComponent<Camera>();
	auto enemyCameraArm = EnemyCharacter->AddComponent<CameraArmComponent>();
	enemyCameraArm->Distance = 100.0f;
	enemyCameraArm->Roll = 35.0f;
	enemyCameraArm->Pitch = 180.0f + 40.0f;
	enemyCamera->CreateViewMatrix(Vector3(0.0f, 25.0f, -100.0f), Vector3(0.0f, 0.0f, 00.0f), Vector3(0.0f, 1.0, 0.0f));
	enemyCamera->CreateProjectionMatrix(1.0f, 10000.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));

	TheWorld.AddEntity(EnemyCharacter);

}

void MultiBattleScene::Init_Effect()
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

void MultiBattleScene::Init_Sound()
{
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/Effect/Attack_Bludgeon.ogg", SoundType::BGM))
	{
		FMODSound* Hit1 = FMODSoundManager::GetInstance()->GetSound(L"Attack_Bludgeon.ogg");
		Hit1->Play();
		Hit1->SetVolume(0.5); // 볼륨 0 ~ 1 사이 값.
		Hit1->SetLoop(false); // Effect여도 Loop true 가능.
		Hit1->Stop();
		SoundMap.insert(std::make_pair(L"Hit1", Hit1));
	}
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/Effect/Attack_Strike.ogg", SoundType::BGM))
	{
		FMODSound* Hit2 = FMODSoundManager::GetInstance()->GetSound(L"Attack_Strike.ogg");
		Hit2->Play();
		Hit2->SetVolume(0.5); // 볼륨 0 ~ 1 사이 값.
		Hit2->SetLoop(false); // Effect여도 Loop true 가능.
		Hit2->Stop();
		SoundMap.insert(std::make_pair(L"Hit2", Hit2));
	}
}

void MultiBattleScene::CameraMove(Vector3& eye, Vector3& target)
{
}

void MultiBattleScene::BattleProcess()
{
	if (TurnStart) { TurnStartProcess(); }
	if (WhoseTurn == playerNum && MyTurnStart) { MyTurnProcess(); }
	if (TurnEndButton->m_bClicked) { TurnEndProcess(); }

	if(WhoseTurn == playerNum && TurnState)CardCheck();
	DeadCheck(); 

	EnemyAttackAnimProcess();
}

void MultiBattleScene::TurnStartProcess()
{
	TurnNum++;
	player1->armor = 0;
	player1->cost = player1->maxCost;
	player2->armor = 0;
	player2->cost = player2->maxCost;

	for (auto enemy : EnemyList) { enemy->SetIntentObj(TurnNum, enemy->IntentIcon, enemy->Intent1, enemy->Intent2); }

	TurnState = true;
	TurnStart = false;
	MyTurnStart = true;
	WhoseTurn = 1;
}

void MultiBattleScene::MyTurnProcess()
{
	UpdatePlayerState();

	int drawNum = 3;
	for (int i = 0; i < drawNum; i++) { CardList[i]->m_bIsDead = false; }

	Dick->Draw(drawNum);
	UpdateHand(drawNum); 

	MyTurnStart = false;
}

void MultiBattleScene::TurnEndProcess()
{
	Dick->TurnEnd();

	//일단 임시로 1p가 2p플레이까지 같이하는 버전으로
	if (playerNum == 1) { playerNum++;  WhoseTurn = 2; CurrentPlayer = player2; MyTurnStart = true; }     // 자신이 1p라면 2p에게 턴을 넘겨줌
	else { playerNum = 1;  CurrentPlayer = player1; TurnNum++; EnemyTurnProcess(); }				// 자신이 2p라면 적에게 턴을 넘겨줌

	TurnEndButton->m_bClicked = false;
}

void MultiBattleScene::EnemyTurnProcess()
{
	TurnState = false;

	InActionEnemy = EnemyList[0];
	EnemyList[0]->patern(player1, TurnNum); // 일단 플레이어 1 고정, 근데 뭐 가까운 적 or 피가 더 많/적은놈 타겟 일케바꿀수도..

	UpdatePlayerState();
	UpdateEnemyState();
	PlayEffect(&TheWorld, L"Hit5", { {0.0f, 10.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {5.0f, 5.0f, 5.0f} }, { false, 0.5f, 0.2f, 1.0f });
	SoundMap.find(L"Hit2")->second->Play();
}

void MultiBattleScene::EnemyAttackAnimProcess()
{	
	// 적 턴이고, 현재 행동중이었던 적의 행동이 끝나면 잡힘
	if (!TurnState && !InActionEnemy->chara->GetComponent<AnimationComponent>()->IsInAction())
	{
		if (InActionEnemy == EnemyList[EnemyList.size() - 1]) // 지금 행동중인 적이 마지막 적이라면  
		{
			TurnState = true;
			TurnStart = true;
			InActionEnemyNum = 0;
			return; 
		}

		InActionEnemyNum++;
		InActionEnemy = EnemyList[InActionEnemyNum];
		InActionEnemy->patern(player1, TurnNum);
		UpdatePlayerState();
		UpdateEnemyState();
		PlayEffect(&TheWorld, L"Hit5", { {0.0f, 10.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {5.0f, 5.0f, 5.0f} }, { false, 0.5f, 0.2f, 1.0f });
		SoundMap.find(L"Hit2")->second->Play();
	}
}

void MultiBattleScene::CardCheck()
{
	//auto PAnime = PlayerCharacter->GetComponent<AnimationComponent>();
	//auto EAnime = EnemyCharacter->GetComponent<AnimationComponent>();
	//if (!PAnime->IsInAction()) { MainCameraSystem->MainCamera = PlayerCharacter->GetComponent<Camera>(); }

	if (TargetEnemy == nullptr) { return; } // 타겟이 없다면 실행ㄴ

	for (int cardNum=0; cardNum<Dick->HandList.size(); cardNum++) 
	{
		if (CardList[cardNum]->m_bClicked && CardList[cardNum]->m_OriginPos.y >= 0.5)
		{
			bool CanUse = false;

			CardList[cardNum]->m_bClicked = false;
			CardList[cardNum]->m_OriginPos = CardList[cardNum]->m_OriginalOriginPos;

			auto PAnime = CurrentPlayer->chara->GetComponent<AnimationComponent>();
			auto EAnime = TargetEnemy->chara->GetComponent<AnimationComponent>();

			switch (Dick->HandList[cardNum])
			{

			case Strike:
			{
				if (ManaCheck(1))
				{
					// 데미지 UI 초안.. 막 움직이기도 하고 나타나면서 커졌다가 작아졌다가도 해야하는데 아 몰랑 나중에 함수로? 하지 뭐
					//Damage2->m_bIsDead = false;
					//Damage2->m_pCutInfoList[0]->tc = NumberTextureList_Red[6];
					TargetEnemy->hp -= 6;
					CanUse = true;

					PAnime->SetClipByName(L"Shooting");
					EAnime->SetClipByName(L"Hit"); // 적 피격 모션
					PlayEffect(&TheWorld, L"Hit5", { {10.0f, 10.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {5.0f, 5.0f, 5.0f} }, { false, 0.5f, 0.2f, 1.0f });
					SoundMap.find(L"Hit1")->second->Play();
				}
			}break;

			case Defend:
			{
				if (ManaCheck(1)) 
				{
					CurrentPlayer->armor += 5;
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
					CurrentPlayer->armor += 8;
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
					CurrentPlayer->armor += 5;
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
				UpdateHand(Dick->HandList.size());
				UpdatePlayerState();
				UpdateEnemyState();

				MainCameraSystem->MainCamera = MainCamera;
			}
			else {} // 여기서 경고문구 출력

		}
	}
}

bool MultiBattleScene::ManaCheck(int cost)
{
	if (CurrentPlayer->cost - cost < 0) return false;
	else { CurrentPlayer->cost -= cost; return true; }
}

void MultiBattleScene::UpdateHand(int handSize)
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

void MultiBattleScene::UpdatePlayerState()
{
	CurrentMana->m_pCutInfoList[0]->tc = NumberTextureList_Black[CurrentPlayer->cost];
	MaxMana->m_pCutInfoList[0]->tc = NumberTextureList_Black[CurrentPlayer->maxCost];

	PlayerCurrenHP1->m_pCutInfoList[0]->tc = NumberTextureList_Black[CurrentPlayer->hp / 10];
	PlayerCurrenHP2->m_pCutInfoList[0]->tc = NumberTextureList_Black[CurrentPlayer->hp % 10];
	PlayerMaxHP1->m_pCutInfoList[0]->tc = NumberTextureList_Black[CurrentPlayer->maxHp / 10];
	PlayerMaxHP2->m_pCutInfoList[0]->tc = NumberTextureList_Black[CurrentPlayer->maxHp % 10];

	if (CurrentPlayer->hp <= 0)
	{
		auto PAnime = PlayerCharacter->GetComponent<AnimationComponent>();
		if (PAnime->CurrentClipName != L"Dying") { PAnime->SetClipByName(L"Dying"); }  // 쓰러지는 애니메이션 두 번 재생하지 않도록
	}


	if (CurrentPlayer->armor <= 0)
	{
		PlayerArmorIcon->m_bIsDead = true;
		PlayerArmor1->m_bIsDead = true;
		PlayerArmor2->m_bIsDead = true;
	}
	else 
	{
		PlayerArmorIcon->m_bIsDead = false;
		PlayerArmor2->m_bIsDead = false;
		PlayerArmor2->m_pCutInfoList[0]->tc = NumberTextureList_Black[CurrentPlayer->armor % 10];
		if ((player1->armor/10) >= 1) 
		{
			PlayerArmor1->m_OriginPos = { -0.692, -0.795 };
			PlayerArmor2->m_OriginPos = { -0.667, -0.795 };

			PlayerArmor1->m_bIsDead = false;
			PlayerArmor1->m_pCutInfoList[0]->tc = NumberTextureList_Black[CurrentPlayer->armor / 10];
		}
		else
		{
			PlayerArmor1->m_bIsDead = true;
			PlayerArmor2->m_OriginPos = { -0.680, -0.795 };
		}
	}
}

void MultiBattleScene::UpdateEnemyState()
{
	for (auto enemy : EnemyList) { enemy->UpdateState(); }
}

void MultiBattleScene::DeadCheck()
{
	if (CurrentPlayer->hp <= 0 && !PlayerCharacter->GetComponent<AnimationComponent>()->IsInAction())
	{
		SS = GAMEOVER;
	}
	else 
	{
		for (auto enemy : EnemyList) 
		{
			if (enemy->hp > 0 || enemy->chara->GetComponent<AnimationComponent>()->IsInAction()) return; // 적이 하나라도 살아있거나 죽는 애니메이션중이라면 탈출!
		}
		SS = CLEAR;
	}
}
