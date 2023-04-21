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
//추가
#include "SocketComponent.h"
#include "AnimationComponent.h"
#include "UpdateAnimSystem.h"
#include "MovementSystem.h"
#include "Character.h"
#include "SelectAnimSystem.h"
#include "BoundingBoxComponent.h"
#include "CameraArmComponent.h"
#include "OscillationComponent.h"
#include "AnimationNotifier.h"
#include "NotifySystem.h"

//추가
#include "ColliderSystem.h"

///////////////////
//Effect Include
///////////////////
#include "EffectInclude/EffectSystem.h"

bool BattleScene::Init()
{
	ID = BATTLE;

	// 사실 플레이어는 타이틀에서 함 초기화하고 가는것도..
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
	MainCamera->Pitch += 30.0f;
	MainCameraSystem->MainCamera = MainCamera;
	MainCameraActor->AddComponent<OscillationComponent>();

	TheWorld.AddEntity(MainCameraActor);

	Init_UI();
	Init_Map();
	Init_Sound();
	Init_Chara();
	Init_Effect();
	// ADD NOTIFY SYSTEM
	TheWorld.AddSystem(new ECS::NotifySystem);

	// 카메라 시스템 및 랜더링 시스템 추가.
	TheWorld.AddSystem(MainCameraSystem);
	TheWorld.AddSystem(new ColliderSystem);
	TheWorld.AddSystem(new MovementSystem);
	TheWorld.AddSystem(new UpdateAnimSystem);
	// SelectAnimSystem 추가
	TheWorld.AddSystem(new SelectAnimSystem);

	LightSystem* lightSystem = new LightSystem;
	lightSystem->MainCamera = MainCameraSystem->MainCamera;
	lightSystem->Initialize();
	TheWorld.AddSystem(lightSystem);

	MainRenderSystem = new RenderSystem;
	MainRenderSystem->MainCamera = MainCameraSystem->MainCamera;
	TheWorld.AddSystem(MainRenderSystem);

	SkyRenderSystem* skyRenderSystem = new SkyRenderSystem;
	skyRenderSystem->MainCamera = MainCameraSystem->MainCamera;
	TheWorld.AddSystem(skyRenderSystem);

	TheWorld.AddSystem(new WidgetRenderSystem);

	MainCameraSystem->MainCamera = PlayerCharacter->GetComponent<Camera>();
	MAIN_PICKER.SetPickingMode(PMOD_CHARACTER);



	return true;
}

bool BattleScene::Frame()
{
	BaseScene::Frame();

	KeyState btnA = Input::GetInstance()->getKey('A');
	if (btnA == KeyState::Hold || btnA == KeyState::Down)
	{
		MainCamera->Yaw -= 0.5f;
	}
	KeyState btnD = Input::GetInstance()->getKey('D');
	if (btnD == KeyState::Hold || btnD == KeyState::Down)
	{
		MainCamera->Yaw += 0.5f;
	}
	KeyState btnW = Input::GetInstance()->getKey('W');
	if (btnW == KeyState::Hold || btnW == KeyState::Down)
	{
		MainCamera->Pitch -= 0.5f;
	}
	KeyState btnS = Input::GetInstance()->getKey('S');
	if (btnS == KeyState::Hold || btnS == KeyState::Down)
	{
		MainCamera->Pitch += 0.5f;
	}
	KeyState btnQ = Input::GetInstance()->getKey('Q');
	if (btnQ == KeyState::Hold || btnQ == KeyState::Down)
	{
		MainCamera->Pos += MainCamera->Look;
	}
	KeyState btnE = Input::GetInstance()->getKey('E');
	if (btnE == KeyState::Hold || btnE == KeyState::Down)
	{
		MainCamera->Pos -= MainCamera->Look;
	}
	KeyState btnR = Input::GetInstance()->getKey('R');
	if (btnR == KeyState::Hold || btnR == KeyState::Down)
	{
		MainCamera->Roll -= 0.5f;
	}
	KeyState btnT = Input::GetInstance()->getKey('T');
	if (btnT == KeyState::Hold || btnT == KeyState::Down)
	{
		MainCamera->Roll += 0.5f;
	}

	KeyState btnF = Input::GetInstance()->getKey('F');
	if (btnF == KeyState::Hold || btnF == KeyState::Down)
	{
		auto oscillation = MainCameraActor->GetComponent<OscillationComponent>();
		auto camera = MainCameraActor->GetComponent<Camera>();
		if ((oscillation != nullptr) && (camera != nullptr))
		{
			oscillation->Damped(camera->Pos, camera->Look, 20.0f, 10.0f, 10.0f, 10.0f);
		}
	}

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
		MainCameraSystem->TargetCamera = PlayerCharacter->GetComponent<Camera>();
	}
	KeyState btnX = Input::GetInstance()->getKey('X');
	if (btnX == KeyState::Hold || btnX == KeyState::Down)
	{
		MainCameraSystem->TargetCamera = EnemyCharacter->GetComponent<Camera>();
	}
	KeyState btnC = Input::GetInstance()->getKey('C');
	if (btnC == KeyState::Hold || btnC == KeyState::Down)
	{
		MainCameraSystem->TargetCamera = nullptr;
	}

	// 얘는 일단 주기적으로 업데이트 해줘야함.
	// MainRenderSystem->MainCamera = MainCameraSystem->MainCamera;

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
			for (auto obj : enemy->ObjList) // 이쪽은 나중에 빌보드 띄우고 나면 쓸모없을지도.. 흠
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

	// 남은 카드 확인 or 버린 카드 확인 버튼 클릭시 씬 전환 (카드 보는 씬으로)
	if (RemainCardButton->m_bClicked) { SS = REMAINVIEW; RemainCardButton->m_bClicked = false; }
	if (DiscardButton->m_bClicked) { SS = DISCARDVIEW; DiscardButton->m_bClicked = false; }

	//대충 여기서 뭔가 돌아가면 될성싶은디
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

	// Actor 생성
	Actor* UI = new Actor;
	auto bc = UI->AddComponent<WidgetComponent>();
	Loader.FileLoad(bc, L"../resource/UI/Save/Battle.txt");

	TurnEndButton = bc->FindObj(L"TurnEnd");
	RemainCardButton = bc->FindObj(L"Remain");
	DiscardButton = bc->FindObj(L"Discard");
	MoveButton = bc->FindObj(L"MoveButton");

	//TurnEndButton->SetOrginStandard(false); // 테스트용
	//RemainCardButton->SetOrginStandard(false);
	//DiscardButton->SetOrginStandard(false);

	// 카드 오브젝트 생성 후 리스트에 넣어줌
	CardList[0] = bc->FindObj(L"Card1");
	CardList[1] = bc->FindObj(L"Card2");
	CardList[2] = bc->FindObj(L"Card3");
	CardList[0]->m_bDraggable = true;
	CardList[1]->m_bDraggable = true;
	CardList[2]->m_bDraggable = true;
	CardList[0]->m_bIsDead = true;		// 드로우 전까진 안보이게
	CardList[1]->m_bIsDead = true;
	CardList[2]->m_bIsDead = true;

	// 플레이어 거시기 상태 표시용 
	PlayerCurrenHP1 = bc->FindObj(L"PlayerCurrentHP_1");
	PlayerCurrenHP2 = bc->FindObj(L"PlayerCurrentHP_2");
	PlayerMaxHP1 = bc->FindObj(L"PlayerMaxHP_1");
	PlayerMaxHP2 = bc->FindObj(L"PlayerMaxHP_2");
	PlayerHPBar = bc->FindObj(L"PlayerHpBar_Full");
	PlayerArmorIcon = bc->FindObj(L"PlayerArmor_Icon");
	PlayerArmor1 = bc->FindObj(L"PlayerArmor_1");
	PlayerArmor2 = bc->FindObj(L"PlayerArmor_2");
	CurrentMana = bc->FindObj(L"Mana_current");
	MaxMana = bc->FindObj(L"Mana_max");

	// 프로그레스 바를 위해서 좌상단 원점으로 바꿔줌
	PlayerHPBar->SetOrginStandard(false);
	PlayerHPBar->m_OriginWH = PlayerHPBar->m_pCutInfoList[0]->pxWH;
	PlayerHPBar->m_OriginUV = PlayerHPBar->m_pCutInfoList[0]->uv;
	UpdateEnemyState();

	// 데미지, 일단 공용인 두자리만..
	Damage1 = bc->FindObj(L"Damage_1");
	Damage2 = bc->FindObj(L"Damage_2");
	Damage1->m_bIsDead = true;
	Damage2->m_bIsDead = true;

	// 툴팁 보드&텍스트, 보드 크기를 바꾸는건 굳이?같긴한데 암튼 텍스트는 자주 갈아끼워야 할듯
	ToolTipBoard = bc->FindObj(L"ToolTipBoard");
	ToolTipText	 = bc->FindObj(L"ToolTipText");
	ToolTipBoard->m_fAlpha = 0.7f;
	ToolTipText->m_fAlpha = 0.5f;
	ToolTipBoard->SetOrginStandard(false);
	ToolTipText->SetOrginStandard(false);
	ToolTipBoard->m_bIsDead = true;
	ToolTipText->m_bIsDead = true;





	// 메인 월드에 액터 추가.
	TheWorld.AddEntity(UI);

	// 카드 위치정보 셋팅, 일단 1장 2장 3장일때만? 나중에 바꿔야할지도 모르니 좀 편하게 세팅해두기
	float CSS = 400.0f; // Card Space Start
	float CSW = 900.0f; // Card Space Width
	float CH = 700.0f;	// Card Height
	Vector2 CFS = { 0.0f, 700.0f };	// Card Flying Start, 카드 날라오는 시작점 

	CardPosList[0].push_back({ CSS + CSW * 1 / 2, CH });	// 1장일때 위치
	CardPosList[0].push_back({ CSS + CSW * 1 / 3, CH });	// 2장일때
	CardPosList[0].push_back({ CSS + CSW * 1 / 4, CH });	// 3장일때
	CardPosList[1].push_back(CFS);
	CardPosList[1].push_back({ CSS + CSW * 2 / 3, CH });
	CardPosList[1].push_back({ CSS + CSW * 2 / 4, CH });
	CardPosList[2].push_back(CFS);
	CardPosList[2].push_back(CFS);
	CardPosList[2].push_back({ CSS + CSW * 3 / 4, CH });
}

void BattleScene::Init_Map()
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

	DirectionalLight* light = new DirectionalLight;
	auto lightComp = light->GetComponent<DirectionalLightComponent>();
	lightComp->Color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	lightComp->Direction = Vector4(1.0f, -1.0f, 1.0f, 1.0f);
	auto lightCompTransform = light->GetComponent<TransformComponent>();
	lightCompTransform->Translation = Vector3(-500.0f, 500.0f, -500.0f);
	TheWorld.AddEntity(light);

	/*DirectionalLight* light2 = new DirectionalLight;
	auto lightComp2 = light2->GetComponent<DirectionalLightComponent>();
	lightComp2->Color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	lightComp2->Direction = Vector4(-1.0f, 1.0f, -1.0f, 1.0f);
	auto lightCompTransform2 = light2->GetComponent<TransformComponent>();
	lightCompTransform2->Translation = Vector3(500.0f, 0.0f, 500.0f);
	TheWorld.AddEntity(light2);*/

	DirectionalLight* light3 = new DirectionalLight;
	auto lightComp3 = light3->GetComponent<DirectionalLightComponent>();
	lightComp3->Color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	lightComp3->Direction = Vector4(1.0f, -1.0f, 1.0f, 1.0f);
	auto lightCompTransform3 = light3->GetComponent<TransformComponent>();
	lightCompTransform3->Translation = Vector3(-250.0f, 500.0f, -500.0f);
	TheWorld.AddEntity(light3);

	for (int cnt = 0; cnt < 4; cnt++)
	{
		Actor* backgroundBuliding = new Actor;
		auto backgroundBulidingStaticMesh = backgroundBuliding->AddComponent<StaticMeshComponent>();
		auto boundBox = backgroundBuliding->AddComponent<BoundingBoxComponent>(Vector3(1.2f, 0.5f, 1.0f));
		FBXLoader::GetInstance()->LoadStaticMesh("../resource/FBX/warehouse/warehouse.stm", backgroundBulidingStaticMesh);

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
		FBXLoader::GetInstance()->LoadStaticMesh("../resource/FBX/warehouse/warehouse.stm", backgroundBulidingStaticMesh);

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
		FBXLoader::GetInstance()->LoadStaticMesh("../resource/FBX/Shipping_Container_A_-_Model/Shipping_Container_A_-_Model.stm", staticMesh);

		auto transform = container->GetComponent<TransformComponent>();
		transform->Scale = Vector3(0.2f, 0.2f, 0.2f);
		transform->Rotation = Vector3(0.0f, 0.0f, 0.0f);
		transform->Translation = Vector3(500.0f, 0.0f, -500.0f + static_cast<float>(cnt) * 125);
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
	FBXLoader::GetInstance()->LoadSkeletalMesh("../resource/FBX/Adam/Adam.skm", playerCharMeshComp);

	// ClipList에 저장되며 SetClipByName(name) 함수로 변경가능 <- name = 확장자명 제외한 파일명
	auto playerCharAnimComp = PlayerCharacter->AddComponent<AnimationComponent>();

	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Run.clp", playerCharAnimComp);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Idle.clp", playerCharAnimComp);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Shooting.clp", playerCharAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Hit.clp", playerCharAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Dying.clp", playerCharAnimComp, false);


	playerCharAnimComp->SetClipByName(L"Idle");

	/////////////// NOTIFIER 사용 예제/////////////////
	// 1. 노티파이어 매니저 컴포넌트를 추가하고
	auto notiMgr = PlayerCharacter->AddComponent<NotifierMgrComponent>();

	// 2. 노티파이어 생성 (여러개 생성 가능하며 한 노티파이어에서 사운드, 이펙트 동시에 발생시킬 수 있음)
	//		 생성시 인자는 노티파이어 이름 , 발생시키고자 하는 프레임(정수) , 노티파이어 수명(프레임, 정수) <- 아무값도 안넣으면 1이 기본
	Notifier* newNoti = notiMgr->CreateNotifier(L"step1", 7);
	// 3. 이펙트 발생을 위해 노티파이어에 이펙트를 달아주기
	//		 사운드, 이펙트 각각 두가지 방법으로 달 수 있음
	// 
	//		 MakeFmodSound(노티파이어 이름 , FmodSound*) <- 기존 사운드맵을 활용할 수 있지만 second로 바로 접근하기때문에 로드 안된 거 잘못 넣으면 뻑나니 주의
	notiMgr->MakeFmodSound(L"step1", SoundMap.find(L"Hit2")->second);

	//		 MakeEffect(노티파이어 이름, 이펙트 키 이름 , 이펙트 위치(트랜스폼 컴포넌트) , 파티클 속성)
	//		 이때 이펙트 위치에 Vector3 값만 넣어도 됨 (오버로딩)
	notiMgr->MakeEffect(L"step1", L"Hit5", { {0.0f, 10.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {5.0f, 5.0f, 5.0f} }, { false, 0.5f, 0.2f, 1.0f });

	Notifier* newNoti2 = notiMgr->CreateNotifier(L"step2", 20);

	//		 MakeSound(노티파이어 이름 , 기존에 로드했던 사운드 파일(FmodSoundManager의 사운드 맵에서 받아옴) , 볼륨, 루프 상태)
	notiMgr->MakeSound(L"step2", L"Attack_Bludgeon.ogg", 0.5f, false);

	// 4. 만들어진 노티파이어를 애니메이션 컴포넌트에 붙여주기
	//		 AddNotifier(애니메이션 컴포넌트 , 해당 컴포넌트가 들고 있는 애니메이션 클립의 이름 , 생성한 노티파이어)
	notiMgr->AddNotifier(*playerCharAnimComp, L"Run", newNoti);
	notiMgr->AddNotifier(*playerCharAnimComp, L"Run", newNoti2);


	// 소켓 컴포넌트 추가
	auto socketComp = PlayerCharacter->AddComponent<SocketComponent>();
	// 스켈레탈 메시 & 본 이름 넘겨서 소켓 부착
	socketComp->Attach(*playerCharMeshComp, "RightHand");
	// 오프셋 조정 T R S
	socketComp->SetOffset(Vector3(2.0f, 2.0f, 0.0f),
		Vector3(-75.0f, -90.0f, -0.0f),
		Vector3(1.0f, 1.0f, 1.0f));

	// 무기로 쓸 스태틱 메시 추가 - 
	//엔티티가 소켓 컴포넌트를 갖고있으면 소켓 위치에 따라 스태틱 메시의 위치가 업데이트 되도록 렌더 시스템 설정
	auto weaponMesh = PlayerCharacter->AddComponent<StaticMeshComponent>();
	FBXLoader::GetInstance()->LoadStaticMesh("../resource/FBX/Pistol/Pistol.stm", weaponMesh);

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
	playerCameraArm->Pitch = 35.0f;
	playerCameraArm->Yaw = 180.0f - 40.0f;
	playerCamera->CreateViewMatrix(Vector3(0.0f, 25.0f, -100.0f), Vector3(0.0f, 0.0f, 00.0f), Vector3(0.0f, 1.0, 0.0f));
	
	TheWorld.AddEntity(PlayerCharacter);

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////캐릭터 추가//////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	Character* PlayerCharacter_B = new Character;
	enemy2->chara = PlayerCharacter_B;

	auto player_BCharMeshComp = PlayerCharacter_B->AddComponent<SkeletalMeshComponent>();
	FBXLoader::GetInstance()->LoadSkeletalMesh("../resource/FBX/Wolverine/WOLVERINE.skm", player_BCharMeshComp);


	auto player_BCharAnimComp = PlayerCharacter_B->AddComponent<AnimationComponent>();

	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Wolverine/Wolverine_anim/Run.clp", player_BCharAnimComp);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Wolverine/Wolverine_anim/Idle.clp", player_BCharAnimComp);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Wolverine/Wolverine_anim/Attack.clp", player_BCharAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Wolverine/Wolverine_anim/Hit.clp", player_BCharAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Wolverine/Wolverine_anim/Dying.clp", player_BCharAnimComp, false);


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
	FBXLoader::GetInstance()->LoadSkeletalMesh("../resource/FBX/Monster/Monster.skm", enemyCharMeshComp);

	auto enemyCharAnimComp = EnemyCharacter->AddComponent<AnimationComponent>();
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Monster/Monster_anim/Run.clp", enemyCharAnimComp);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Monster/Monster_anim/Idle.clp", enemyCharAnimComp);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Monster/Monster_anim/Kick.clp", enemyCharAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Monster/Monster_anim/Hit.clp", enemyCharAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Monster/Monster_anim/Dying.clp", enemyCharAnimComp, false);

	enemyCharAnimComp->SetClipByName(L"Idle");

	auto enemyCharTransformComp = EnemyCharacter->GetComponent<TransformComponent>();


	enemyCharTransformComp->Scale = Vector3(10.f, 10.f, 10.f);
	enemyCharTransformComp->Rotation = Vector3(0.0f, 90.0f, 0.0f);
	enemyCharTransformComp->Translation = Vector3(100.0f, 0.0f, 0.0f);

	auto E1MC = EnemyCharacter->GetComponent<MovementComponent>();
	E1MC->Speed = 25.0f;
	EnemyCharacter->MoveTo(Vector3(20.0f, 0.0f, 0.0f));

	//Picking Info Test
	enemyCharMeshComp->Name = "Enemy";

	/////////////// Bounding Box Add ////////////
	auto enemyOBBComp = EnemyCharacter->AddComponent<BoundingBoxComponent>(Vector3(0.2f, 0.45f, 0.2f), Vector3(0.0f, 0.45f, 0.0f));

	// 적 캐릭터 용 카메라 및 카메라 암
	auto enemyCamera = EnemyCharacter->AddComponent<Camera>();
	auto enemyCameraArm = EnemyCharacter->AddComponent<CameraArmComponent>();
	enemyCameraArm->Distance = 100.0f;
	enemyCameraArm->Pitch = 35.0f;
	enemyCameraArm->Yaw = 180.0f + 40.0f;
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

void BattleScene::CameraMove(Vector3& eye, Vector3& target)
{
}

void BattleScene::BattleProcess()
{
	if (TurnStart) { TurnStartProcess(); }
	if (TurnEndButton->m_bClicked) { TurnEndProcess(); }
	if (MoveButton->m_bClicked) { IsMoving = true; MoveButton->m_bClicked = false; MAIN_PICKER.optPickingMode = PMOD_LAND; }
	if (IsMoving) { MoveProcess(); }

	EnemyAttackAnimProcess();
	CardCheck();
	DeadCheck();
	ToolTipCheck();
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
	TurnEndButton->m_bDisable = false;
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
	TurnEndButton->m_bDisable = true;
}

void BattleScene::EnemyTurnProcess()
{
	TurnState = false;

	InActionEnemy = EnemyList[0];
	int damage = EnemyList[0]->patern(player, TurnNum);
	DamageAnimation(damage, false);

	UpdatePlayerState();
	UpdateEnemyState();

	//VVVVVVVVVVVVVVVVVV카메라 테스트 추가부VVVVVVVVVVVVVVVVVVVVVVVV
	auto HitCam = player->chara->GetComponent<Camera>();
	if (HitCam)
	{
		MainCameraSystem->TargetCamera = HitCam;
	}
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	PlayEffect(&TheWorld, L"Hit5", { {0.0f, 10.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {5.0f, 5.0f, 5.0f} }, { false, 0.5f, 0.2f, 1.0f });
	SoundMap.find(L"Hit2")->second->Play();
}

void BattleScene::EnemyAttackAnimProcess()
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
		int damage = InActionEnemy->patern(player, TurnNum);
		DamageAnimation(damage, false);
		UpdatePlayerState();
		UpdateEnemyState();
		PlayEffect(&TheWorld, L"Hit5", { {0.0f, 10.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {5.0f, 5.0f, 5.0f} }, { false, 0.5f, 0.2f, 1.0f });
		SoundMap.find(L"Hit2")->second->Play();
	}
}

static float MoveTimer = 0.0f;
void BattleScene::MoveProcess()
{
	// 딜레이주려고
	MoveTimer += Timer::GetInstance()->GetDeltaTime();

	if (MoveTimer >= 0.5f)
	{
		// 카메라 좀 바꿔주고

		// 피킹될 지점 표시해주는 이펙트
		PlayEffect(&TheWorld, L"Hit5", { MAIN_PICKER.vIntersection, {0.0f, 0.0f, 0.0f}, {2.0f, 2.0f, 2.0f} }, { false, 0.5f, 0.2f, 1.0f });

		// 땅 클릭하면 글로 이동
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
	if (TargetEnemy == nullptr) { return; } // 타겟이 없다면 실행ㄴ

	for (int cardNum = 0; cardNum < Dick->HandList.size(); cardNum++)
	{
		if (CardList[cardNum]->m_bClicked && CardList[cardNum]->m_OriginPos.y >= 0.5)	// 카드를 드래그해서 일정 높이 이상으로 올라갔다면
		{
			bool CanUse = false;
			int	 DrawedCard = 0;

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
					//VVVVVVVVVVVVVVVVVV카메라 테스트 추가부VVVVVVVVVVVVVVVVVVVVVVVV
					auto HitCam = TargetEnemy->chara->GetComponent<Camera>();
					if (HitCam)
					{
						MainCameraSystem->TargetCamera = HitCam;
					}
					//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

					TargetEnemy->hp -= 6;
					DamageAnimation(6, true);
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
					player->armor += 5;
					CanUse = true;
				}
			}break;

			case PommelStrike:
			{
				if (ManaCheck(1))
				{
					//VVVVVVVVVVVVVVVVVV카메라 테스트 추가부VVVVVVVVVVVVVVVVVVVVVVVV
					auto HitCam = TargetEnemy->chara->GetComponent<Camera>();
					if (HitCam)
					{
						MainCameraSystem->TargetCamera = HitCam;
					}
					//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

					TargetEnemy->hp -= 9;
					DamageAnimation(9, true);
					Dick->Draw(1);
					DrawedCard = 1;
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
					DrawedCard = 1;
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
					//VVVVVVVVVVVVVVVVVV카메라 테스트 추가부VVVVVVVVVVVVVVVVVVVVVVVV
					auto HitCam = TargetEnemy->chara->GetComponent<Camera>();
					if (HitCam)
					{
						MainCameraSystem->TargetCamera = HitCam;
					}
					//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

					TargetEnemy->hp -= 5;
					DamageAnimation(5, true);
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
				UpdateHand(Dick->HandList.size(), cardNum, DrawedCard);
				UpdatePlayerState();
				UpdateEnemyState();
			}
			else {} // 여기서 마나부족 경고문구 출력

		}
	}
}

// 데미지 애니메이션, 
void BattleScene::DamageAnimation(int damage, bool hitEnemy)
{
	// 아군 캐릭터가 적 공격, 뭐 큰차인 없고 걍 데미지 날라가는 방향
	if (hitEnemy) 
	{
		// 데미지가 10 이상일 경우, 두자리수 ㅇㅇ
		if (damage/10 > 0) 
		{
			Damage1->m_bIsDead = false;
			Damage1->m_pCutInfoList[0]->tc = NumberTextureList_Red[damage/10];
			Vector2 AnimPos[2] = { {1000.0f, 450.0f},{1200.0f, 350.0f} };
			Damage1->SetAnimation( AnimPos, 0.5f, true);
		}
		Damage2->m_bIsDead = false;
		Damage2->m_pCutInfoList[0]->tc = NumberTextureList_Red[damage % 10];
		Vector2 AnimPos[2] = { {1100.0f, 450.0f},{1300.0f, 350.0f} };
		Damage2->SetAnimation(AnimPos, 0.5f, true);
	}

	// 적이 아군 공격
	else 
	{	// 데미지가 10 이상일 경우, 두자리수 ㅇㅇ
		if (damage / 10 > 0)
		{
			Damage1->m_bIsDead = false;
			Damage1->m_pCutInfoList[0]->tc = NumberTextureList_Red[damage / 10];
			Vector2 AnimPos[2] = { {400.0f, 450.0f},{200.0f, 350.0f} };
			Damage1->SetAnimation(AnimPos, 0.5f, true);
		}
		Damage2->m_bIsDead = false;
		Damage2->m_pCutInfoList[0]->tc = NumberTextureList_Red[damage % 10];
		Vector2 AnimPos[2] = { {500.0f, 450.0f},{300.0f, 350.0f} };
		Damage2->SetAnimation(AnimPos, 0.5f, true);
	}
}

bool BattleScene::ManaCheck(int cost)
{
	if (player->cost - cost < 0) return false;
	else { player->cost -= cost; return true; }
}

// 턴 시작시
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

// 카드 썼을때 
void BattleScene::UpdateHand(int handSize, int UsedCard, int DrawedCard)
{
	for (int card = 0; card < handSize; card++)
	{
		for (int ci = 0; ci < 4; ci++)
		{
			CardList[card]->m_pCutInfoList[ci]->tc = CardTextureList[Dick->HandList[card]];
		}
		CardList[card]->m_bIsDead = false;

		Vector2 AnimPos[2] = { CardList[card]->NtoP_Pos(CardList[card]->m_OriginalOriginPos), CardPosList[card][handSize - 1] };
		float AnimTime = 0.15f;
		if (card >= UsedCard) // 써진 카드 위치부터 한칸씩 밀어서 애니메이션 시작점을 줘야 멀쩡해보임
		{
			// 드로우카드를 썼을 때, 벡터 마지막+1을 참조하면 안되니까
			if (DrawedCard > 0 && card + DrawedCard >= handSize)
			{
				AnimPos[0] = { -100.0f, 700.0f };
				AnimTime = 0.5f;
			}	// 드로우된 카드는 멀리서 날라오니까 애니메이션 시간 좀 더줌

// 드로우X
			else { AnimPos[0] = CardList[card]->NtoP_Pos(CardList[card + 1]->m_OriginalOriginPos); }
		}
		CardList[card]->SetAnimation(AnimPos, AnimTime);
	}

	for (int card = handSize; card < 3; card++) // for (int card = handSize; card < CardList.size(); card++)
	{
		CardList[card]->m_bIsDead = true;
	}
}

void BattleScene::UpdatePlayerState()
{
	// 마나 (코스트)
	CurrentMana->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->cost];
	MaxMana->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->maxCost];

	// 
	PlayerCurrenHP1->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->hp / 10];
	PlayerCurrenHP2->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->hp % 10];
	PlayerMaxHP1->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->maxHp / 10];
	PlayerMaxHP2->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->maxHp % 10];

	// 프로그레스 바 (HP바) 
	float hpPercentage = (float)player->hp / (float)player->maxHp;
	PlayerHPBar->m_pCutInfoList[0]->pxWH.x = PlayerHPBar->m_OriginWH.x * hpPercentage;	// X축만(가로로만)줄어들어야하니까
	PlayerHPBar->m_pCutInfoList[0]->uv = PlayerHPBar->m_OriginUV * hpPercentage;			// x축만 ㅇㅇ

	if (player->hp <= 0)
	{
		auto PAnime = PlayerCharacter->GetComponent<AnimationComponent>();
		if (PAnime->CurrentClipName != L"Dying") { PAnime->SetClipByName(L"Dying"); }  // 쓰러지는 애니메이션 두 번 재생하지 않도록
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
		if ((player->armor / 10) >= 1)
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
			if (enemy->hp > 0 || enemy->chara->GetComponent<AnimationComponent>()->IsInAction()) return; // 적이 하나라도 살아있거나 죽는 애니메이션중이라면 탈출!
		}
		SS = CLEAR;
	}
}

void BattleScene::ToolTipCheck()
{
	POINT cp;
	::GetCursorPos(&cp);
	::ScreenToClient(DXDevice::g_hWnd, &cp);
	float mouseNdcX = (cp.x / DXDevice::g_ViewPort.Width) * 2.0f - 1.0f;
	float mouseNdcY = -((cp.y / DXDevice::g_ViewPort.Height) * 2.0f - 1.0f);
	ToolTipBoard->m_OriginPos = { mouseNdcX,mouseNdcY };
	ToolTipText->m_OriginPos = { mouseNdcX,mouseNdcY };

	if (RemainCardButton->m_bHover) // 남은 카드 버튼
	{
		ToolTipText->m_pCutInfoList[0]->tc = TextTextureList.find(L"remain")->second;
		ToolTipBoard->m_bIsDead = false;
		ToolTipText->m_bIsDead = false;
	}
	else if (DiscardButton->m_bHover) // 버린 카드 버튼
	{
		ToolTipText->m_pCutInfoList[0]->tc = TextTextureList.find(L"discard")->second;
		// 오른쪽 끝이라서 왼쪽으로 땡겨줌
		ToolTipBoard->m_OriginPos.x = mouseNdcX - ToolTipBoard->PtoN(ToolTipBoard->m_pCutInfoList[0]->pxWH).x;	
		ToolTipText->m_OriginPos.x = mouseNdcX - ToolTipText->PtoN(ToolTipText->m_pCutInfoList[0]->pxWH).x;
		ToolTipBoard->m_bIsDead = false;
		ToolTipText->m_bIsDead = false;
	}
	else if (MoveButton->m_bHover) // 이동 버튼
	{
		ToolTipText->m_pCutInfoList[0]->tc = TextTextureList.find(L"move")->second;
		ToolTipBoard->m_bIsDead = false;
		ToolTipText->m_bIsDead = false;
	}
	else 
	{
		ToolTipBoard->m_bIsDead = true;
		ToolTipText->m_bIsDead = true;
	}
}
