#include "MultiBattleScene.h"
#include "LightSystem.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "SkyBoxComponent.h"
#include "FBXLoader.hpp"
#include "Landscape.h"
#include "SkeletalMeshComponent.h"
#include "MaterialManager.h"
#include "SkyRenderSystem.h"
#include "SkyDomeComponent.h"
//추가
#include "TimelineComponent.h"
#include "TimelineSystem.h"

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
#include "CloudDomeComponent.h"
#include "ActionCameraComponent.h"

//추가
#include "ColliderSystem.h"

///////////////////
//Effect Include
///////////////////
#include "EffectInclude/EffectSystem.h"

//서버 추가

#include"ServerPacketHandler.h"
#include"ClientPacketHandler.h"
#include"protocol.pb.h"
#include"HostSession.h"
#include"Service.h"


bool MultiBattleScene::Init()
{
	ID = MULTIBATTLE;

	MainSaveLoadSystem = new SaveLoadSystem;
	TheWorld.AddSystem(MainSaveLoadSystem);

	// 메인 카메라
	MainCameraSystem = new CameraSystem;
	MainCameraActor = new Actor;
	MainCamera = MainCameraActor->AddComponent<Camera>();
	MainCamera->CreateViewMatrix(Vector3(0.0f, 300.0f, -600.0f), Vector3(150.0f, 20.0f, 50.0f), Vector3(0.0f, 1.0, 0.0f));
	MainCamera->CreateProjectionMatrix(1.0f, 10000.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));
	MainCameraSystem->MainCamera = MainCamera;
	MainCameraActor->AddComponent<OscillationComponent>();
	TheWorld.AddEntity(MainCameraActor);

	// 타겟 카메라로 쓰기 위한 놈
	MoveCamera = new Camera;
	MoveCamera->CreateProjectionMatrix(1.0f, 10000.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));

	// 액션 카메라, 지금은 그냥 lerp함수로 거리구하기용
	TestActionCamera = new ActionCamera;
	TestActionCamera->Create(Vector3(0.0f, 50.0f, -70.0f), Vector3(0.0f, 20.0f, 50.0f), 1.0f, 10000.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));
	TheWorld.AddEntity(TestActionCamera);

	// 이쪽은 map들어갈때쯤 거기서 초기화해주는게 낫겠는데.. 아님 샘플에서?
	player1 = new Player;
	player1->cost = player1->maxCost;
	player1->maxHp = 40;
	player1->hp = 40;
	PlayerList.push_back(player1);

	player2 = new Player;
	player2->cost = player2->maxCost;
	player2->maxHp = 50;
	player2->hp = 50;
	PlayerList.push_back(player2);

	enemy1 = new Enemy_1;
	enemy1->pWorld = &TheWorld;
	enemy1->MainCamera = MainCamera;
	enemy1->Init();
	enemy1->NumberTextureList_Red = NumberTextureList_Red;
	enemy1->NumberTextureList_Black = NumberTextureList_Black;
	enemy1->Name->m_pCutInfoList[0]->tc = TextTextureList.find(L"enemy1")->second;
	EnemyList.push_back(enemy1);
	AllEnemyList.push_back(enemy1);
	enemy2 = new Enemy_2;
	enemy2->pWorld = &TheWorld;
	enemy2->MainCamera = MainCamera;
	enemy2->Init();
	enemy2->NumberTextureList_Red = NumberTextureList_Red;
	enemy2->NumberTextureList_Black = NumberTextureList_Black;
	enemy2->Name->m_pCutInfoList[0]->tc = TextTextureList.find(L"enemy2")->second;
	EnemyList.push_back(enemy2);
	AllEnemyList.push_back(enemy2);

	Init_Sound();
	Init_UI();
	Init_Map();
	/*bool testFlag = true;
	if (!testFlag)
	{
		Init_Map();
		MainSaveLoadSystem->Save("../resource/Map/", "TestWorld");
	}
	else
	{
		MainSaveLoadSystem->Load("../resource/Map/", "TestWorld");
	}*/
	Init_Chara();
	// Init_Effect(); -> 배틀씬 피커 이펙트를 멀티씬이 뺏어가버리는 이슈가 존재해서, 이건 그 프레임의 거시기 인잇에서 해야..? 


	// 카메라 시스템 및 랜더링 시스템 추가.
	TheWorld.AddSystem(MainCameraSystem);
	TheWorld.AddSystem(new ColliderSystem);
	TheWorld.AddSystem(new MovementSystem);
	TheWorld.AddSystem(new UpdateAnimSystem);
	// SelectAnimSystem 추가
	TheWorld.AddSystem(new SelectAnimSystem);
	// ADD NOTIFY SYSTEM
	TheWorld.AddSystem(new ECS::NotifySystem);
	// ADD TIMLELINE SYSTEM
	TheWorld.AddSystem(new TimelineSystem);

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

	WRS = new WidgetRenderSystem;
	TheWorld.AddSystem(WRS);
	MAIN_PICKER.SetPickingMode(PMOD_CHARACTER);

	//////  초기 카메라 세팅  //////

	// 플레이어 캐릭터 포워드 벡터 셋팅, 1P 기준
	auto PlayerForward = PlayerCharacter->MovementComp->Destination - PlayerCharacter->Transform->Translation;
	PlayerForward.Normalize();
	player1->Forward = PlayerForward;

	// Right 벡터 구할라고
	DirectX::XMFLOAT3 upVec(0.0f, 1.0f, 0.0f);
	Vector3 Right;	// Calculate the right vector	
	DirectX::XMFLOAT3 crossProduct;
	XMStoreFloat3(&crossProduct, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&player1->Forward), XMLoadFloat3(&upVec)));
	DirectX::XMStoreFloat3(&Right, DirectX::XMVector3Normalize(XMLoadFloat3(&crossProduct)));

	// 카메라 위치 세팅, 나중에 또 업데이트 필요할듯 (주로 이동 후에)
	PlayerCameraPos = PlayerCharacter->MovementComp->Destination;	// 플레이어 도착예상지점 기준..
	PlayerCameraPos -= player1->Forward * 1.0f;	// 플레이어 캐릭터가 향하는 방향에서 살짝 뒤로 뺌
	PlayerCameraPos.y += 30.0f;					// 위로 좀 올려줌
	PlayerCameraPos -= Right * 35.0f;			// 오른쪽으로 좀 빼주고

	// 카메라 타겟 세팅
	auto CameraStartTarget = (enemy1->chara->MovementComp->Destination + enemy2->chara->MovementComp->Destination) / 2;

	// 카메라 뷰 매트릭스 재설정하고 타겟 카메라로 임명
	MoveCamera->CreateViewMatrix(PlayerCameraPos, CameraStartTarget, Vector3(0.0f, 1.0, 0.0f));
	MainCameraSystem->TargetCamera = MoveCamera;

	TargetEnemy = EnemyList[0];

	return true;
}

bool MultiBattleScene::Frame()
{
	BaseScene::Frame();

	//서버 추가
	if (initTriger)
	{
		Init_Effect();	// 피커 이펙트 뺏기는 이슈땜에 여기

		//서버에서 추가
		if (gpHost != nullptr && gpHost->IsConnected())
		{
			playerNum = 1;
		}
		if (gpClient != nullptr && gpClient->IsConnected())
		{
			playerNum = 2;
		}
		CurrentPlayer = player1;
		initTriger = false;
	}

	// 적 상태창 업데이트
	if (TurnState) // 적 턴에는 그냥 적 상태창 안보이게끔
	{
		for (auto enemy : EnemyList)
		{
			if (enemy == TargetEnemy)
			{
				for (auto obj : enemy->ObjList) { obj->m_bIsDead = false; }

				// 화살표랑 밑바닥원 이펙트 갱신
				auto ArrowTrans = MAIN_PICKER.pSelectedArrowUIEntity->GetComponent<TransformComponent>();
				ArrowTrans->Translation = enemy->chara->Transform->Translation;
				ArrowTrans->Translation.y += enemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center.y * 2.0f;
				ArrowTrans->Translation.y += 7.0f;

				auto CircleTrans = MAIN_PICKER.pSelectedCircleUIEntity->GetComponent<TransformComponent>();
				CircleTrans->Translation = enemy->chara->Transform->Translation;
				CircleTrans->Translation.y = 0.005f;
			}
			else { for (auto obj : enemy->ObjList) { obj->m_bIsDead = true; } }
			enemy->HpEmpty->m_bIsDead = false;
			enemy->HpFull->m_bIsDead = false;
		}
		MAIN_PICKER.pSelectedCircleUIEntity->GetComponent<ParticleEffectComponent>()->m_pPPsystem->m_PSProp.bShow = true;
		MAIN_PICKER.pSelectedArrowUIEntity->GetComponent<ParticleEffectComponent>()->m_pPPsystem->m_PSProp.bShow = true;
	}
	else
	{
		for (auto enemy : EnemyList)
		{
			for (auto obj : enemy->ObjList) { obj->m_bIsDead = true; }
			enemy->HpEmpty->m_bIsDead = true;
			enemy->HpFull->m_bIsDead = true;
		}

		MAIN_PICKER.pSelectedArrowUIEntity->GetComponent<ParticleEffectComponent>()->m_pPPsystem->m_PSProp.bShow = false;
		MAIN_PICKER.pSelectedCircleUIEntity->GetComponent<ParticleEffectComponent>()->m_pPPsystem->m_PSProp.bShow = false;
	}

	// A키 누르면 타겟변경, 내 턴일 때만
	KeyState btnA = Input::GetInstance()->getKey('A');
	if (btnA == KeyState::Down && WhoseTurn == playerNum)
	{
		if (TargetEnemy != nullptr)
		{
			int enemyNum = 0;
			for (int i = 0; i < EnemyList.size(); i++)
			{
				if (EnemyList[i] == TargetEnemy)
				{
					enemyNum = i;
				}

			}
			// 현재 타겟중인 적이 마지막 적이라면, 첫 적으로 돌림
			if (enemyNum + 1 == EnemyList.size()) { TargetEnemy = EnemyList[0]; }
			else { TargetEnemy = EnemyList[enemyNum + 1]; }

			/*auto PlayerForward = PlayerCharacter->MovementComp->Destination - PlayerCharacter->Transform->Translation;
			PlayerForward.Normalize();
			player->Forward = PlayerForward;*/

			// 캐릭터 방향 회전, 애니메이션 줄라면 줄수있는데 여유생기면,,머,,
			Vector3 NewPlayerForward = TargetEnemy->chara->Transform->Translation - CurrentPlayer->chara->Transform->Translation;
			NewPlayerForward.Normalize();
			DirectX::SimpleMath::Quaternion quaternion = Quaternion::FromToRotation(CurrentPlayer->Forward, NewPlayerForward);
			Vector3 eulerAngles = quaternion.ToEuler();

			Vector3 cross = DirectX::XMVector3Cross(CurrentPlayer->Forward, NewPlayerForward);
			Vector3 dot = DirectX::XMVector3Dot(cross, Vector3(0, 1, 0));

			if (dot.y < 0.f)
			{
				// 좌우 판별
			}

			eulerAngles.y = DirectX::XMConvertToDegrees(eulerAngles.y);
			//player->chara->Transform->Rotation += eulerAngles;
			auto timeline = PlayerCharacter->GetComponent<TimelineComponent>();
			if (timeline != nullptr)
			{
				timeline->CreateTimeline(CurrentPlayer->chara->Transform->Rotation.y, CurrentPlayer->chara->Transform->Rotation.y + eulerAngles.y, 0.2f);
				timeline->SetTarget(&CurrentPlayer->chara->Transform->Rotation.y);
				timeline->PlayFromStart();
			}

			CurrentPlayer->Forward = NewPlayerForward;

			// 카메라 타겟 세팅
			auto CameraTarget = TargetEnemy->chara->Transform->Translation;
			CameraTarget.y += 10.0f;
			TargetPlayer = CurrentPlayer;
			UpdateCameraPos();

			MoveCamera->CreateViewMatrix(PlayerCameraPos, CameraTarget, Vector3(0.0f, 1.0, 0.0f));
			MainCameraSystem->TargetCamera = MoveCamera;

			// 락온 사운드 출력
			SoundMap.find(L"LockOn")->second->Play();
		}
	}

	for (auto enemy : EnemyList)
	{
		enemy->HpEmpty->m_OrginPos3D = enemy->chara->MovementComp->Location;
		enemy->HpEmpty->m_OrginPos3D.y += enemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center.y * 2.0f;
		//enemy->HpEmpty->m_OrginPos3D.y += 1.0f;
		enemy->HpFull->m_OrginPos3D = enemy->chara->MovementComp->Location;
		enemy->HpFull->m_OrginPos3D.y += enemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center.y * 2.0f;
		//enemy->HpFull->m_OrginPos3D.y += 1.0f;
	}

	// 얘는 일단 주기적으로 업데이트 해줘야함.
	MainRenderSystem->MainCamera = MainCameraSystem->MainCamera;

	return true;
}

bool MultiBattleScene::Render()
{
	BaseScene::Render();

	if (WhoseTurn == playerNum)	// 내 턴일때만 확인 가능토록, 왜냐면 왠지 꼬일 것 같거든
	{
		// 남은 카드 확인 or 버린 카드 확인 버튼 클릭시 씬 전환 (카드 보는 씬으로)
		if (RemainCardButton->m_bClicked) { SS = REMAINVIEW; RemainCardButton->m_bClicked = false; }
		if (DiscardButton->m_bClicked) { SS = DISCARDVIEW; DiscardButton->m_bClicked = false; }
	}

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
	MoveButton = bc->FindObj(L"MoveButton");

	// 카드 오브젝트 생성 후 리스트에 넣어줌
	CardList[0] = bc->FindObj(L"Card1");
	CardList[1] = bc->FindObj(L"Card2");
	CardList[2] = bc->FindObj(L"Card3");
	CardList[0]->m_bDraggable = true;
	CardList[1]->m_bDraggable = true;
	CardList[2]->m_bDraggable = true;
	CardList[0]->m_OriginalOriginPos = CardList[0]->m_OriginPos;
	CardList[0]->m_OriginalOriginPos = CardList[0]->m_OriginPos;
	CardList[0]->m_OriginalOriginPos = CardList[0]->m_OriginPos;
	CardList[0]->m_bIsDead = true;		// 드로우 전까진 안보이게
	CardList[1]->m_bIsDead = true;
	CardList[2]->m_bIsDead = true;

	// 플레이어 거시기 표시용 
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
	Damage1->m_bIsDead = true;
	Damage2 = bc->FindObj(L"Damage_2");
	Damage2->m_bIsDead = true;

	// 툴팁 보드&텍스트, 보드 크기를 바꾸는건 굳이?같긴한데 암튼 텍스트는 자주 갈아끼워야 할듯
	ToolTipBoard = bc->FindObj(L"ToolTipBoard");
	ToolTipText = bc->FindObj(L"ToolTipText");
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

void MultiBattleScene::Init_Map()
{
	//// 지형 액터 추가.
	Landscape* landscape = new Landscape;
	auto landscapeComponents = landscape->GetComponent<LandscapeComponents>();
	/*if (!landscape->LoadXML(L"../resource/Map/TestMap.landscape"))
	{
		landscapeComponents->Build(16, 16, 7, 10);
		landscape->SaveXML(L"../resource/Map/TestMap.landscape");
	}*/
	landscapeComponents->Build(16, 16, 7, 10);
	landscapeComponents->SetCamera(MainCameraSystem->MainCamera);
	TheWorld.AddEntity(landscape);

	// Sky Dome 추가.
	Actor* skyDomeActor = new Actor;
	auto skyDomeComp = skyDomeActor->AddComponent<SkyDomeComponent>();
	skyDomeComp->Scale = Vector3(5000.0f, 5000.0f, 5000.0f);

	Material* skyDomeMaterial = MaterialManager::GetInstance()->CreateMaterial(L"SkyDome");
	skyDomeMaterial->DiffuseTextureName = L"../resource/Texture/SkyDome.png";
	skyDomeMaterial->Create();
	skyDomeComp->SetMaterial(skyDomeMaterial);

	TheWorld.AddEntity(skyDomeActor);

	// Cloud Dome 추가
	Actor* cloudDomeActor = new Actor;
	auto cloudDomeComp = cloudDomeActor->AddComponent<CloudDomeComponent>();
	cloudDomeComp->Scale = Vector3(4500, 4500, 4500);
	TheWorld.AddEntity(cloudDomeActor);

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
}

void MultiBattleScene::Init_Chara()
{
	PlayerCharacter = new Character;
	player1->chara = PlayerCharacter;
	auto playerCharMeshComp = PlayerCharacter->AddComponent<SkeletalMeshComponent>();
	FBXLoader::GetInstance()->LoadSkeletalMesh("../resource/FBX/Adam/Adam.skm", playerCharMeshComp);

	// ClipList에 저장되며 SetClipByName(name) 함수로 변경가능 <- name = 확장자명 제외한 파일명
	auto playerCharAnimComp = PlayerCharacter->AddComponent<AnimationComponent>();

	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Run.clp", playerCharAnimComp);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Idle.clp", playerCharAnimComp);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Hit.clp", playerCharAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Dying.clp", playerCharAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Shooting.clp", playerCharAnimComp, false);	// 일반사격
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Gunplay.clp", playerCharAnimComp, false);	// 좀쎈사격
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Pistol_Whip.clp", playerCharAnimComp, false); // 죽탱이
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Inward_Block.clp", playerCharAnimComp, false); // 가드
	playerCharAnimComp->SetClipByName(L"Idle");

	// 노티파이 설정
	int soundFrame = 0;
	int delayFrame = 0;
	auto notiMgr = PlayerCharacter->AddComponent<NotifierMgrComponent>();
	delayFrame = 5;
	// 일반사격
	soundFrame = 6;
	Notifier* Fire_ready = notiMgr->CreateNotifier(L"Fire_ready", soundFrame - delayFrame);
	Notifier* Fire = notiMgr->CreateNotifier(L"Fire", soundFrame);
	notiMgr->MakeSound(L"Fire", L"Fire1.mp3", 1.0f, false);
	notiMgr->AddNotifier(*playerCharAnimComp, L"Shooting", Fire_ready);
	notiMgr->AddNotifier(*playerCharAnimComp, L"Shooting", Fire);
	//좀쎈사격
	soundFrame = 20;
	Notifier* Fire2_ready = notiMgr->CreateNotifier(L"Fire2_ready", soundFrame - delayFrame);
	Notifier* Fire2 = notiMgr->CreateNotifier(L"Fire2", soundFrame);
	notiMgr->MakeSound(L"Fire2", L"Fire2.mp3", 1.0f, false);
	notiMgr->AddNotifier(*playerCharAnimComp, L"Gunplay", Fire2_ready);
	notiMgr->AddNotifier(*playerCharAnimComp, L"Gunplay", Fire2);
	// 손잡이 후리기
	soundFrame = 17;
	Notifier* Strike_ready = notiMgr->CreateNotifier(L"Strike_ready", soundFrame - delayFrame);
	Notifier* Strike = notiMgr->CreateNotifier(L"Strike", soundFrame);
	notiMgr->MakeSound(L"Strike", L"Attack_Strike.ogg", 1.0f, false);
	notiMgr->AddNotifier(*playerCharAnimComp, L"Pistol_Whip", Strike_ready);
	notiMgr->AddNotifier(*playerCharAnimComp, L"Pistol_Whip", Strike);


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
	//playerCharTransformComp->Translation = Vector3(10.0f, 0.0f, 0.0f);

	auto playerCharMovementComp = PlayerCharacter->GetComponent<MovementComponent>();
	playerCharMovementComp->Speed = 40.0f;
	PlayerCharacter->MoveTo(Vector3(-20.0f, 0.0f, 0.0f));

	//Picking Info Test
	playerCharMeshComp->Name = "player";
	////////////// Bounding Box Add /////////////////
	auto playerOBBComp = PlayerCharacter->AddComponent<BoundingBoxComponent>(Vector3(0.75f, 1.1f, 0.75f), Vector3(0.0f, 1.1f, 0.0f));
	// 플레이어용 카메라 및 카메라 암 설정.
	auto playerCamera = PlayerCharacter->AddComponent<Camera>();
	auto playerCameraArm = PlayerCharacter->AddComponent<CameraArmComponent>();
	playerCameraArm->Distance = 30.0f;
	playerCameraArm->Pitch = 15.0f;
	playerCameraArm->Yaw = 180.0f - 40.0f;
	playerCamera->CreateViewMatrix(Vector3(0.0f, 25.0f, -100.0f), Vector3(0.0f, 0.0f, 00.0f), Vector3(0.0f, 1.0, 0.0f));

	auto timeline = PlayerCharacter->AddComponent <TimelineComponent>();

	TheWorld.AddEntity(PlayerCharacter);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// 2P 캐릭터 //////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Character* Chara_2P = new Character;
	player2->chara = Chara_2P;

	auto Char2PMeshComp = Chara_2P->AddComponent<SkeletalMeshComponent>();
	FBXLoader::GetInstance()->LoadSkeletalMesh("../resource/FBX/James/James.skm", Char2PMeshComp);

	auto Char2PAnimComp = Chara_2P->AddComponent<AnimationComponent>();
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/James/James_anim/Run.clp", Char2PAnimComp);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/James/James_anim/Idle.clp", Char2PAnimComp);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/James/James_anim/Hit.clp", Char2PAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/James/James_anim/Dying.clp", Char2PAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/James/James_anim/Inward_Block.clp", Char2PAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/James/James_anim/Chapa_2.clp", Char2PAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/James/James_anim/Kick.clp", Char2PAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/James/James_anim/Chapa_Giratoria_2.clp", Char2PAnimComp, false);
	Char2PAnimComp->SetClipByName(L"Idle");

	// 노티파이 설정
	notiMgr = Chara_2P->AddComponent<NotifierMgrComponent>();
	delayFrame = 5;
	// 발차기 1 (간단)
	soundFrame = 15;
	Notifier* P2A1_R = notiMgr->CreateNotifier(L"P2A1_R", soundFrame - delayFrame);	// Player2 Attack1 Ready
	Notifier* P2A1 = notiMgr->CreateNotifier(L"P2A1", soundFrame);
	notiMgr->MakeSound(L"P2A1", L"Attack_Strike.ogg", 1.0f, false);
	notiMgr->AddNotifier(*Char2PAnimComp, L"Chapa_2", P2A1_R);
	notiMgr->AddNotifier(*Char2PAnimComp, L"Chapa_2", P2A1);
	// 발차기 2 (살짝 화려)
	soundFrame = 30;
	Notifier* P2A2_R = notiMgr->CreateNotifier(L"P2A2_R", soundFrame - delayFrame);
	Notifier* P2A2 = notiMgr->CreateNotifier(L"P2A2", soundFrame);
	notiMgr->MakeSound(L"P2A2", L"Attack_Bludgeon.ogg", 1.0f, false);
	notiMgr->AddNotifier(*Char2PAnimComp, L"Chapa_2", P2A2_R);
	notiMgr->AddNotifier(*Char2PAnimComp, L"Chapa_2", P2A2);
	// 발차기 3 (겐세이 겁나넣다가 때림)
	soundFrame = 56;
	Notifier* P2A3_R = notiMgr->CreateNotifier(L"P2A3_R", soundFrame - delayFrame);
	Notifier* P2A3 = notiMgr->CreateNotifier(L"P2A1", soundFrame);
	notiMgr->MakeSound(L"P2A3", L"Hit.ogg", 1.0f, false);
	notiMgr->AddNotifier(*Char2PAnimComp, L"Chapa_2", P2A3_R);
	notiMgr->AddNotifier(*Char2PAnimComp, L"Chapa_2", P2A3);

	auto Char2PTransformComp = Chara_2P->GetComponent<TransformComponent>();
	Char2PTransformComp->Scale = Vector3(13.0f, 13.0f, 13.0f);
	Char2PTransformComp->Rotation = Vector3(0.0f, 90.0f, 0.0f);
	Char2PTransformComp->Translation = Vector3(0.0f, 0.0f, 40.0f);

	auto Char2PMovementComp = Chara_2P->GetComponent<MovementComponent>();
	Char2PMovementComp->Speed = 45.0f;
	Chara_2P->MoveTo(Vector3(0.0f, 0.0f, 40.0f));

	/////////////// Bounding Box Add ////////////
	auto BBComp = Chara_2P->AddComponent<BoundingBoxComponent>(Vector3(0.5f, 0.9f, 0.5f), Vector3(0.0f, 0.9f, 0.0f));

	auto timeline2 = Chara_2P->AddComponent<TimelineComponent>();

	TheWorld.AddEntity(Chara_2P);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////// 적 캐릭터 1  //////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	EnemyCharacter = new Character;
	enemy1->chara = EnemyCharacter;

	auto enemyCharMeshComp = EnemyCharacter->AddComponent<SkeletalMeshComponent>();
	FBXLoader::GetInstance()->LoadSkeletalMesh("../resource/FBX/Monster/Monster.skm", enemyCharMeshComp);

	auto enemyCharAnimComp = EnemyCharacter->AddComponent<AnimationComponent>();
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Monster/Monster_anim/Run.clp", enemyCharAnimComp);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Monster/Monster_anim/Idle.clp", enemyCharAnimComp);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Monster/Monster_anim/Attack1.clp", enemyCharAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Monster/Monster_anim/Attack2.clp", enemyCharAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Monster/Monster_anim/Attack3.clp", enemyCharAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Monster/Monster_anim/Hit.clp", enemyCharAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Monster/Monster_anim/Dying.clp", enemyCharAnimComp, false);
	enemyCharAnimComp->SetClipByName(L"Idle");

	// 노티파이 설정
	notiMgr = EnemyCharacter->AddComponent<NotifierMgrComponent>();
	delayFrame = 5;
	// 플레이어 캐릭터 1은 미리 맞는 애니 없어도 ㄱㅊ은듯? 멀티가면 어케될지 모르지
	// 펀치
	soundFrame = 40;
	Notifier* E1A1 = notiMgr->CreateNotifier(L"E1A1", soundFrame);		// Enemy1 Attack1
	notiMgr->MakeSound(L"E1A1", L"E1A1.ogg", 1.0f, false);
	notiMgr->AddNotifier(*enemyCharAnimComp, L"Attack1", E1A1);
	// 킦꾸
	soundFrame = 25;
	Notifier* E1A2 = notiMgr->CreateNotifier(L"E1A2", soundFrame);
	notiMgr->MakeSound(L"E1A2", L"Attack_Bludgeon.ogg", 1.0f, false);
	notiMgr->AddNotifier(*enemyCharAnimComp, L"Attack2", E1A2);
	// 쑤꾸림(비명)
	soundFrame = 60;
	Notifier* E1A3 = notiMgr->CreateNotifier(L"E1A3", soundFrame);
	notiMgr->MakeSound(L"E1A3", L"E1A3.mp3", 1.0f, false);
	notiMgr->AddNotifier(*enemyCharAnimComp, L"Attack3", E1A3);
	// 사망, 소리붙이려고
	soundFrame = 1;
	Notifier* E1Defeat = notiMgr->CreateNotifier(L"E1Defeat", soundFrame);
	notiMgr->MakeSound(L"E1Defeat", L"E1Defeat.ogg", 1.0f, false);
	notiMgr->AddNotifier(*enemyCharAnimComp, L"Dying", E1Defeat);

	auto enemyCharTransformComp = EnemyCharacter->GetComponent<TransformComponent>();
	enemyCharTransformComp->Scale = Vector3(15.0f, 15.0f, 15.0f);
	enemyCharTransformComp->Rotation = Vector3(0.0f, 90.0f, 0.0f);
	enemyCharTransformComp->Translation = Vector3(100.0f, 0.0f, 0.0f);
	//enemyCharTransformComp->Translation =  Vector3(40.0f, 0.0f, 0.0f);

	auto E1MC = EnemyCharacter->GetComponent<MovementComponent>();
	E1MC->Speed = 30.0f;
	EnemyCharacter->MoveTo(Vector3(20.0f, 0.0f, 0.0f));

	//Picking Info Test
	enemyCharMeshComp->Name = "Enemy";

	/////////////// Bounding Box Add ////////////
	auto enemyOBBComp = EnemyCharacter->AddComponent<BoundingBoxComponent>(Vector3(0.5f, 1.0f, 0.5f), Vector3(0.0f, 1.0f, 0.0f));

	// 적 캐릭터 용 카메라 및 카메라 암
	auto enemyCamera = EnemyCharacter->AddComponent<Camera>();
	auto enemyCameraArm = EnemyCharacter->AddComponent<CameraArmComponent>();
	enemyCameraArm->Distance = 50.0f;
	enemyCameraArm->Pitch = 35.0f;
	enemyCameraArm->Yaw = 180.0f + 30.0f;
	enemyCamera->CreateViewMatrix(Vector3(0.0f, 25.0f, -100.0f), Vector3(0.0f, 0.0f, 00.0f), Vector3(0.0f, 1.0, 0.0f));
	enemyCamera->CreateProjectionMatrix(1.0f, 10000.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));

	TheWorld.AddEntity(EnemyCharacter);

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////// 적 캐릭터 2 //////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	Character* EnemyCharacter2 = new Character;
	enemy2->chara = EnemyCharacter2;

	auto Enemy2_MeshComp = EnemyCharacter2->AddComponent<SkeletalMeshComponent>();
	FBXLoader::GetInstance()->LoadSkeletalMesh("../resource/FBX/Skeletonzombie_T_Avelange/Skeletonzombie_T_Avelange.skm", Enemy2_MeshComp);

	auto Enemy2_CharAnimComp = EnemyCharacter2->AddComponent<AnimationComponent>();

	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Skeletonzombie_T_Avelange/Skeletonzombie_T_Avelange_anim/Run.clp", Enemy2_CharAnimComp);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Skeletonzombie_T_Avelange/Skeletonzombie_T_Avelange_anim/Idle.clp", Enemy2_CharAnimComp);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Skeletonzombie_T_Avelange/Skeletonzombie_T_Avelange_anim/Attack1.clp", Enemy2_CharAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Skeletonzombie_T_Avelange/Skeletonzombie_T_Avelange_anim/Attack2.clp", Enemy2_CharAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Skeletonzombie_T_Avelange/Skeletonzombie_T_Avelange_anim/Attack3.clp", Enemy2_CharAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Skeletonzombie_T_Avelange/Skeletonzombie_T_Avelange_anim/Hit.clp", Enemy2_CharAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Skeletonzombie_T_Avelange/Skeletonzombie_T_Avelange_anim/Dying.clp", Enemy2_CharAnimComp, false);

	// 노티파이 설정
	notiMgr = EnemyCharacter2->AddComponent<NotifierMgrComponent>();
	// 쎄게긁기
	soundFrame = 39;
	Notifier* E2A1 = notiMgr->CreateNotifier(L"E2A1", soundFrame);		// Enemy2 Attack1
	notiMgr->MakeSound(L"E2A1", L"E2A1.wav", 1.0f, false);
	notiMgr->AddNotifier(*Enemy2_CharAnimComp, L"Attack1", E2A1);
	// 살짝긁기
	soundFrame = 31;
	Notifier* E2A2 = notiMgr->CreateNotifier(L"E2A2", soundFrame);
	notiMgr->MakeSound(L"E2A2", L"E2A2.mp3", 1.0f, false);
	notiMgr->AddNotifier(*Enemy2_CharAnimComp, L"Attack2", E2A2);
	// 중간긁기
	soundFrame = 31;
	Notifier* E2A3 = notiMgr->CreateNotifier(L"E2A3", soundFrame);
	notiMgr->MakeSound(L"E2A3", L"E2A3.mp3", 1.0f, false);
	notiMgr->AddNotifier(*Enemy2_CharAnimComp, L"Attack3", E2A3);
	// 사망, 소리붙이려고
	soundFrame = 1;
	Notifier* E2Defeat = notiMgr->CreateNotifier(L"E2Defeat", soundFrame);
	notiMgr->MakeSound(L"E2Defeat", L"E2Defeat.ogg", 1.0f, false);
	notiMgr->AddNotifier(*Enemy2_CharAnimComp, L"Dying", E2Defeat);

	auto Enemy2_CharTransformComp = EnemyCharacter2->GetComponent<TransformComponent>();
	Enemy2_CharTransformComp->Scale = Vector3(13.0f, 13.0f, 13.0f);
	Enemy2_CharTransformComp->Rotation = Vector3(0.0f, 90.0f, 0.0f);
	Enemy2_CharTransformComp->Translation = Vector3(70.0f, 0.0f, 100.0f);
	//Enemy2_CharTransformComp->Translation = Vector3(20.0f, 0.0f, 70.0f);

	auto Enemy2_CharMovementComp = EnemyCharacter2->GetComponent<MovementComponent>();
	Enemy2_CharMovementComp->Speed = 30.0f;
	EnemyCharacter2->MoveTo(Vector3(20.0f, 0.0f, 70.0f));

	/////////////// Bounding Box Add ////////////
	auto player_BOBBComp = EnemyCharacter2->AddComponent<BoundingBoxComponent>(Vector3(0.5f, 0.9f, 0.5f), Vector3(0.0f, 0.9f, 0.0f));

	// 적 캐릭터 용 카메라 및 카메라 암
	auto enemyCamera2 = EnemyCharacter2->AddComponent<Camera>();
	auto enemyCameraArm2 = EnemyCharacter2->AddComponent<CameraArmComponent>();
	enemyCameraArm2->Distance = 50.0f;
	enemyCameraArm2->Pitch = 35.0f;
	enemyCameraArm2->Yaw = 180.0f + 30.0f;
	enemyCamera->CreateViewMatrix(Vector3(0.0f, 25.0f, -100.0f), Vector3(0.0f, 0.0f, 00.0f), Vector3(0.0f, 1.0, 0.0f));
	enemyCamera->CreateProjectionMatrix(1.0f, 10000.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));

	TheWorld.AddEntity(EnemyCharacter2);

}

void MultiBattleScene::Init_Effect()
{
	//Effect Test
	ECS::EffectSystem* ESystem = new ECS::EffectSystem;
	ESystem->init(&TheWorld);
	TheWorld.AddSystem(ESystem);

	ParticleEffect* pMoveUICursor = new ParticleEffect(L"Arrow",
		TransformComponent(),
		{ true, true, 10.0f, 0.0f, 1.0f });
	TheWorld.AddEntity(pMoveUICursor);

	ParticleEffect* pSelectedArrowUICursor = new ParticleEffect(L"BlackLineArrow",
		TransformComponent(),
		{ false, true, 10.0f, 0.0f, 1.0f });
	TheWorld.AddEntity(pSelectedArrowUICursor);

	ParticleEffect* pSelectedCircleUICursor = new ParticleEffect(L"FloorCircle",
		TransformComponent(),
		{ false, true, 10.0f, 0.0f, 1.0f });
	TheWorld.AddEntity(pSelectedCircleUICursor);

	MAIN_PICKER.pMoveUIEntity = pMoveUICursor;
	MAIN_PICKER.pSelectedArrowUIEntity = pSelectedArrowUICursor;
	MAIN_PICKER.pSelectedCircleUIEntity = pSelectedCircleUICursor;
}

void MultiBattleScene::Init_Sound()
{
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/Effect/TurnEnd.ogg", SoundType::BGM))
	{
		FMODSound* Sound = FMODSoundManager::GetInstance()->GetSound(L"TurnEnd.ogg");
		Sound->Play();
		Sound->SetVolume(1.0);
		Sound->SetLoop(false);
		Sound->Stop();
		SoundMap.insert(std::make_pair(L"TurnEnd", Sound));
	}
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/Effect/Shield.ogg", SoundType::BGM))
	{
		FMODSound* Sound = FMODSoundManager::GetInstance()->GetSound(L"Shield.ogg");
		Sound->Play();
		Sound->SetVolume(1.0);
		Sound->SetLoop(false);
		Sound->Stop();
		SoundMap.insert(std::make_pair(L"Shield", Sound));
	}
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
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/Effect/Fire1.mp3", SoundType::BGM))
	{
		FMODSound* Sound = FMODSoundManager::GetInstance()->GetSound(L"Fire1.mp3");
		Sound->Play();
		Sound->SetVolume(0.5);
		Sound->SetLoop(false);
		Sound->Stop();
		SoundMap.insert(std::make_pair(L"Fire1", Sound));
	}
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/Effect/Fire2.mp3", SoundType::BGM))
	{
		FMODSound* Sound = FMODSoundManager::GetInstance()->GetSound(L"Fire2.mp3");
		Sound->Play();
		Sound->SetVolume(0.5);
		Sound->SetLoop(false);
		Sound->Stop();
		SoundMap.insert(std::make_pair(L"Fire2", Sound));
	}
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/Effect/Fire3.mp3", SoundType::BGM))
	{
		FMODSound* Sound = FMODSoundManager::GetInstance()->GetSound(L"Fire3.mp3");
		Sound->Play();
		Sound->SetVolume(0.5);
		Sound->SetLoop(false);
		Sound->Stop();
		SoundMap.insert(std::make_pair(L"Fire3", Sound));
	}
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/Effect/Hit.ogg", SoundType::BGM))
	{
		FMODSound* Sound = FMODSoundManager::GetInstance()->GetSound(L"Hit.ogg");
		Sound->Play();
		Sound->SetVolume(1.0);
		Sound->SetLoop(false);
		Sound->Stop();
		SoundMap.insert(std::make_pair(L"Hit", Sound));
	}
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/Effect/LockOn.wav", SoundType::BGM))
	{
		FMODSound* Sound = FMODSoundManager::GetInstance()->GetSound(L"LockOn.wav");
		Sound->Play();
		Sound->SetVolume(1.0);
		Sound->SetLoop(false);
		Sound->Stop();
		SoundMap.insert(std::make_pair(L"LockOn", Sound));
	}
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/Effect/Shield2.ogg", SoundType::BGM))
	{
		FMODSound* Sound = FMODSoundManager::GetInstance()->GetSound(L"Shield2.ogg");
		Sound->Play();
		Sound->SetVolume(1.0);
		Sound->SetLoop(false);
		Sound->Stop();
		SoundMap.insert(std::make_pair(L"Shield2", Sound));
	}
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/Effect/E1A1.ogg", SoundType::BGM))
	{
		FMODSound* Sound = FMODSoundManager::GetInstance()->GetSound(L"E1A1.ogg");
		Sound->Play();
		Sound->SetVolume(1.0);
		Sound->SetLoop(false);
		Sound->Stop();
		SoundMap.insert(std::make_pair(L"E1A1", Sound));
	}
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/Effect/E1A3.mp3", SoundType::BGM))
	{
		FMODSound* Sound = FMODSoundManager::GetInstance()->GetSound(L"E1A3.mp3");
		Sound->Play();
		Sound->SetVolume(1.0);
		Sound->SetLoop(false);
		Sound->Stop();
		SoundMap.insert(std::make_pair(L"E1A3", Sound));
	}
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/Effect/E2A1.wav", SoundType::BGM))
	{
		FMODSound* Sound = FMODSoundManager::GetInstance()->GetSound(L"E2A1.wav");
		Sound->Play();
		Sound->SetVolume(1.0);
		Sound->SetLoop(false);
		Sound->Stop();
		SoundMap.insert(std::make_pair(L"E2A1", Sound));
	}
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/Effect/E1Defeat.ogg", SoundType::BGM))
	{
		FMODSound* Sound = FMODSoundManager::GetInstance()->GetSound(L"E1Defeat.ogg");
		Sound->Play();
		Sound->SetVolume(1.0);
		Sound->SetLoop(false);
		Sound->Stop();
		SoundMap.insert(std::make_pair(L"E1Defeat", Sound));
	}
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/Effect/E2A2.mp3", SoundType::BGM))
	{
		FMODSound* Sound = FMODSoundManager::GetInstance()->GetSound(L"E2A2.mp3");
		Sound->Play();
		Sound->SetVolume(1.0);
		Sound->SetLoop(false);
		Sound->Stop();
		SoundMap.insert(std::make_pair(L"E2A2", Sound));
	}
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/Effect/E2A3.mp3", SoundType::BGM))
	{
		FMODSound* Sound = FMODSoundManager::GetInstance()->GetSound(L"E2A3.mp3");
		Sound->Play();
		Sound->SetVolume(1.0);
		Sound->SetLoop(false);
		Sound->Stop();
		SoundMap.insert(std::make_pair(L"E2A3", Sound));
	}
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/Effect/E2Defeat.ogg", SoundType::BGM))
	{
		FMODSound* Sound = FMODSoundManager::GetInstance()->GetSound(L"E2Defeat.ogg");
		Sound->Play();
		Sound->SetVolume(1.0);
		Sound->SetLoop(false);
		Sound->Stop();
		SoundMap.insert(std::make_pair(L"E2Defeat", Sound));
	}
}

void MultiBattleScene::UpdateCameraPos()
{
	// Right 벡터 구할라고
	DirectX::XMFLOAT3 upVec(0.0f, 1.0f, 0.0f);
	Vector3 Right;	// Calculate the right vector	
	DirectX::XMFLOAT3 crossProduct;
	XMStoreFloat3(&crossProduct, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&TargetPlayer->Forward), XMLoadFloat3(&upVec)));
	DirectX::XMStoreFloat3(&Right, DirectX::XMVector3Normalize(XMLoadFloat3(&crossProduct)));

	// 카메라 위치 세팅, 나중에 또 업데이트 필요할듯 (주로 이동 후에)
	PlayerCameraPos = PlayerCharacter->MovementComp->Destination;	// 플레이어 도착예상지점 기준..
	PlayerCameraPos -= TargetPlayer->Forward * 30.0f;	// 플레이어 캐릭터가 향하는 방향에서 살짝 뒤로 뺌
	PlayerCameraPos.y += 30.0f;					// 위로 좀 올려줌
	PlayerCameraPos -= Right * 15.0f;			// 오른쪽으로 좀 빼주고
}

// 노티파이 체크해서 플레이어랑 적 캐릭터랑 애니메이션 합을 맞추고 이펙트 터트림(위치필요해서), 사운드는 노티파이 안에서 터트림
void MultiBattleScene::NotifierCheck()
{
	// 애니메이션 세팅에 필요한거
	auto PAnime = CurrentPlayer->chara->GetComponent<AnimationComponent>();
	auto EAnime = TargetEnemy->chara->GetComponent<AnimationComponent>();

	for (auto player : PlayerList)
	{
		for (auto noti : player->chara->GetComponent<AnimationComponent>()->CurrentClip->NotifierList)
		{
			if (noti->IsOn)
			{
				// 이쪽은 P1 노티파이 처리
				if (noti->Lable == L"Fire_ready")
				{
					if (TargetEnemy->hp > 0) { EAnime->SetClipByName(L"Hit"); }
				}
				else if (noti->Lable == L"Fire")
				{
					PlayEffect(&TheWorld, L"Hit5", { TargetEnemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {3.0f, 3.0f, 3.0f} }, { false, 0.5f, 0.2f, 1.0f });
					if (PlayerDamage > 0) { DamageAnimation(PlayerDamage, true); PlayerDamage = 0; }
				}
				else if (noti->Lable == L"Fire2_ready")
				{
					if (TargetEnemy->hp > 0) { EAnime->SetClipByName(L"Hit"); }
				}
				else if (noti->Lable == L"Fire2")
				{
					PlayEffect(&TheWorld, L"Hit5", { TargetEnemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {4.0f, 4.0f, 4.0f} }, { false, 0.5f, 0.2f, 1.0f });
					if (PlayerDamage > 0) { DamageAnimation(PlayerDamage, true); PlayerDamage = 0; }
				}
				else if (noti->Lable == L"Strike_ready")
				{
					if (TargetEnemy->hp > 0) { EAnime->SetClipByName(L"Hit"); }
				}
				else if (noti->Lable == L"Strike")
				{
					Vector3 EffectPos = TargetEnemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center;
					EffectPos.y += 5.0f;
					PlayEffect(&TheWorld, L"Hit5", { EffectPos, Vector3(), {3.0f, 3.0f, 3.0f} }, { false, 0.5f, 0.2f, 1.0f });
					if (PlayerDamage > 0) { DamageAnimation(PlayerDamage, true); PlayerDamage = 0; }
				}


				// 여기서부턴 P2 노티파이 처리 
				else if (noti->Lable == L"P2A1_R")
				{
					if (TargetEnemy->hp > 0) { EAnime->SetClipByName(L"Hit"); }
				}
				else if (noti->Lable == L"P2A1")
				{
					Vector3 EffectPos = TargetEnemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center;
					PlayEffect(&TheWorld, L"Hit5", { EffectPos, Vector3(), {2.0f, 2.0f, 2.0f} }, { false, 0.5f, 0.2f, 1.0f });
					if (PlayerDamage > 0) { DamageAnimation(PlayerDamage, true); PlayerDamage = 0; }
				}
				else if (noti->Lable == L"P2A2_R")
				{
					if (TargetEnemy->hp > 0) { EAnime->SetClipByName(L"Hit"); }
				}
				else if (noti->Lable == L"P2A2")
				{
					Vector3 EffectPos = TargetEnemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center;
					PlayEffect(&TheWorld, L"Hit5", { EffectPos, Vector3(), {3.0f, 3.0f, 3.0f} }, { false, 0.5f, 0.2f, 1.0f });
					if (PlayerDamage > 0) { DamageAnimation(PlayerDamage, true); PlayerDamage = 0; }
				}
				else if (noti->Lable == L"P2A3_R")
				{
					if (TargetEnemy->hp > 0) { EAnime->SetClipByName(L"Hit"); }
				}
				else if (noti->Lable == L"P2A3")
				{
					Vector3 EffectPos = TargetEnemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center;
					PlayEffect(&TheWorld, L"Hit5", { EffectPos, Vector3(), {4.0f, 4.0f, 4.0f} }, { false, 0.5f, 0.2f, 1.0f });
					if (PlayerDamage > 0) { DamageAnimation(PlayerDamage, true); PlayerDamage = 0; }
				}

				UpdateEnemyState();
			}
		}
	}


	// 적 행동에서 발생한 노티파이를 기준으로 실행, 얘도 아마 적턴에만 ㅇㅇ
	for (auto enemy : AllEnemyList)
	{
		for (auto noti : enemy->chara->GetComponent<AnimationComponent>()->CurrentClip->NotifierList)
		{
			if (noti->IsOn)
			{
				if (noti->Lable == L"E1A1") // 적1 후리기
				{
					if (enemy->TargetPlayer->hp > 0) { PAnime->SetClipByName(L"Hit"); }
					PlayEffect(&TheWorld, L"Hit3", { enemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {5.0f, 5.0f, 5.0f} }, { false, 0.5f, 0.2f, 1.0f });
					if (EnemyDamage > 0) { DamageAnimation(EnemyDamage, false); EnemyDamage = 0; }
				}
				else if (noti->Lable == L"E1A2") // 적1 발차기
				{
					if (enemy->TargetPlayer->hp > 0) { PAnime->SetClipByName(L"Hit"); }
					PlayEffect(&TheWorld, L"Hit5", { enemy->TargetPlayer->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {3.0f, 3.0f, 3.0f} }, { false, 0.5f, 0.2f, 1.0f });
					if (EnemyDamage > 0) { DamageAnimation(EnemyDamage, false); EnemyDamage = 0; }
				}
				else if (noti->Lable == L"E1A3")	// 적1 비명
				{
					if (enemy->TargetPlayer->hp > 0) { PAnime->SetClipByName(L"Hit"); }
					PlayEffect(&TheWorld, L"Hit5", { enemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {10.0f, 10.0f, 10.0f} }, { false, 2.5f, 0.2f, 1.0f });
					if (EnemyDamage > 0) { DamageAnimation(EnemyDamage, false); EnemyDamage = 0; }
				}
				if (noti->Lable == L"E2A1")	// 적2 강긁기
				{
					if (enemy->TargetPlayer->hp > 0) { PAnime->SetClipByName(L"Hit"); }
					PlayEffect(&TheWorld, L"Hit1", { enemy->TargetPlayer->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {5.0f, 5.0f, 5.0f} }, { false, 0.5f, 0.2f, 1.0f });
					if (EnemyDamage > 0) { DamageAnimation(EnemyDamage, false); EnemyDamage = 0; }
				}
				if (noti->Lable == L"E2A2") // 적2 약긁기
				{
					if (enemy->TargetPlayer->hp > 0) { PAnime->SetClipByName(L"Hit"); }
					PlayEffect(&TheWorld, L"Hit2", { enemy->TargetPlayer->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {3.0f, 3.0f, 3.0f} }, { false, 0.5f, 0.2f, 1.0f });
					if (EnemyDamage > 0) { DamageAnimation(EnemyDamage, false); EnemyDamage = 0; }
				}
				if (noti->Lable == L"E2A3") // 적2 중긁기
				{
					if (enemy->TargetPlayer->hp > 0) { PAnime->SetClipByName(L"Hit"); }
					PlayEffect(&TheWorld, L"Hit3", { enemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {3.0f, 3.0f, 3.0f} }, { false, 0.5f, 0.2f, 1.0f });
					if (EnemyDamage > 0) { DamageAnimation(EnemyDamage, false); EnemyDamage = 0; }
				}

				UpdatePlayerState();
			}
		}
	}
}


void MultiBattleScene::BattleProcess()
{
	if (TurnStart) { TurnStartProcess(); }
	if ((WhoseTurn == playerNum) && MyTurnStart) { MyTurnProcess(); }
	if ((WhoseTurn == playerNum) && TurnEndButton->m_bClicked) { TurnEndProcess(); }
	if ((WhoseTurn == playerNum) && MoveButton->m_bClicked) { IsMoving = true; MoveButton->m_bClicked = false; MAIN_PICKER.optPickingMode = PMOD_LAND; }
	if (IsMoving) { MoveProcess(); }

	EnemyAnimProcess();
	if (WhoseTurn == playerNum && TurnState) { CardCheck(); }
	else if (WhoseTurn != playerNum && TurnState) { Reaction(); }
	ToolTipCheck();
	NotifierCheck();
	DeadCheck();
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

	TurnEndButton->m_bDisable = false;	// 일단 둘다 턴종 꺼놓고 밑에서 켜줌
}

void MultiBattleScene::MyTurnProcess()
{
	UpdatePlayerState();

	int drawNum = 3;
	for (int i = 0; i < drawNum; i++) { CardList[i]->m_bIsDead = false; }
	MyDeck->Draw(drawNum);
	UpdateHand(drawNum);

	TurnEndButton->m_bDisable = false;
	MyTurnStart = false;
	
	//if (gpHost != nullptr && gpHost->IsConnected())
	//{
	//	protocol::S_DRAWCARD hostDraw;
	//	*hostDraw.mutable_remainingcardlist()	= { multyserver->MyDeck->RemainingCardList.begin(), multyserver->MyDeck->RemainingCardList.end() };
	//	*hostDraw.mutable_handlist()			= { multyserver->MyDeck->HandList.begin(), multyserver->MyDeck->HandList.end() };
	//	*hostDraw.mutable_discardlist()			= { multyserver->MyDeck->DiscardList.begin(), multyserver->MyDeck->DiscardList.end() };
	//
	//	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(hostDraw);
	//
	//	auto session = gpHost->GetService()->_listener->GetAcceptVector().front()->_session;
	//	session->Send(sendBuffer);
	//}
	//if (gpClient != nullptr && gpClient->IsConnected())
	//{
	//	protocol::C_DRAWCARD clientDraw;
	//	*clientDraw.mutable_remainingcardlist()		= { MyDeck->RemainingCardList.begin(), MyDeck->RemainingCardList.end() };
	//	*clientDraw.mutable_handlist()				= { MyDeck->HandList.begin(), MyDeck->HandList.end() };
	//	*clientDraw.mutable_discardlist()			= { MyDeck->DiscardList.begin(), MyDeck->DiscardList.end() };
	//	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(clientDraw);
	//	auto session = gpClient->GetClientservice()->sessionsForConnect.front();
	//	session->Send(sendBuffer);
	//}
}

//void MultiBattleScene::temp()
//{
//	protocol::S_DECK sDeck;
//	protocol::Deck toSendDeck;
//	*toSendDeck.mutable_decklist() = { MyDeck->DeckList.begin(), MyDeck->DeckList.end() };
//	*toSendDeck.mutable_discardlist() = { MyDeck->DiscardList.begin(), MyDeck->DiscardList.end() };
//	*toSendDeck.mutable_handlist() = { MyDeck->HandList.begin(), MyDeck->HandList.end() };
//	*toSendDeck.mutable_remainingcardlist() = { MyDeck->RemainingCardList.begin(), MyDeck->RemainingCardList.end() };
//	sDeck.set_allocated_deck(&toSendDeck);
//	std::string serial = sDeck.SerializeAsString();
//	protocol::S_DECK rDeck;
//	rDeck.ParseFromString(serial);
//}

void MultiBattleScene::TurnEndProcess()
{
	// 내 턴일때, 즉 내가 턴을 종료한 주체일 경우
	if (WhoseTurn == playerNum)
	{
		// 다른 플레이어에게 패킷을 보냄
		if (gpHost != nullptr && gpHost->IsConnected())
		{
			protocol::S_TURNEND hostEnd;
			hostEnd.set_bturnend(true);

			auto sendBuf = ServerPacketHandler::MakeSendBuffer(hostEnd);
			auto session = gpHost->GetService()->GetSessions().begin();
			(*session)->Send(sendBuf);
		}
		if (gpClient != nullptr && gpClient->IsConnected())
		{
			protocol::C_TURNEND clientEnd;
			clientEnd.set_bturnend(true);

			auto sendBuf = ClientPacketHandler::MakeSendBuffer(clientEnd);
			auto session = gpClient->GetClientservice()->GetSessions().begin();
			(*session)->Send(sendBuf);
		}

		// 내가 턴종했을때만 작동해야 하는 것들
		for (int card = 0; card < MyDeck->HandList.size(); card++)	// 카드들 화면 밖으로 날리는 애니메이션	
		{
			CardList[card]->m_bIsDead = false;
			Vector2 AnimPos[2] = { CardList[card]->NtoP_Pos(CardList[card]->m_OriginalOriginPos), {1800.0f,700.0f} };
			CardList[card]->SetAnimation(AnimPos, 0.5f);
		}
		MyDeck->TurnEnd();
		TurnEndButton->m_bClicked = false;
		TurnEndButton->m_bDisable = true;
	}

	// 내가 호스트, 즉 1p일 경우
	if (gpHost != nullptr && gpHost->IsConnected())
	{
		if (WhoseTurn == 1)
		{
			WhoseTurn = 2;
			CurrentPlayer = player2;
		}
		else
		{
			WhoseTurn = 1;
			CurrentPlayer = player1;			// 근데 이거 여기저기서 만지지 않나... 오류날듯?
			TurnNum++;
			EnemyTurnProcess();
		}
	}
	// 내가 고객, 즉 2p일 경우
	else if (gpClient != nullptr && gpClient->IsConnected())
	{
		if (WhoseTurn == 1)
		{
			WhoseTurn = 2;
			CurrentPlayer = player2;
		}
		else
		{
			WhoseTurn = 1;
			CurrentPlayer = player1;			// 근데 이거 여기저기서 만지지 않나... 오류날듯?
			TurnNum++;
			EnemyTurnProcess();
		}
	}

	SoundMap.find(L"TurnEnd")->second->Play();	// 턴종 소리



	{	// moveClicked, movementPoint
		if (gpHost != nullptr && gpHost->IsConnected())
		{
			//moveclicked
			{
				protocol::S_MOVECLICKED p1MoveClicked;
				p1MoveClicked.set_bmoveclick(true);

				auto sendBuf = ServerPacketHandler::MakeSendBuffer(p1MoveClicked);
				auto session = gpHost->GetService()->GetSessions().begin();
				(*session)->Send(sendBuf);
			}

			//movementPoint
			{
				protocol::S_MOVETOPOINT p1Point;
				p1Point.set_vecx(0.0f);
				p1Point.set_vecy(0.0f);
				p1Point.set_vecz(0.0f);

				auto sendBuf = ServerPacketHandler::MakeSendBuffer(p1Point);
				auto session = gpHost->GetService()->GetSessions().begin();
				(*session)->Send(sendBuf);
			}
		}
		if (gpClient != nullptr && gpClient->IsConnected())
		{
			//moveclicked
			{
				protocol::C_MOVECLICKED p2MoveClicked;
				p2MoveClicked.set_bmoveclick(true);

				auto sendBuf = ClientPacketHandler::MakeSendBuffer(p2MoveClicked);
				auto session = gpClient->GetClientservice()->GetSessions().begin();
				(*session)->Send(sendBuf);
			}

			//movementPoint
			{
				protocol::C_MOVETOPOINT p2Point;
				p2Point.set_vecx(0.0f);
				p2Point.set_vecy(0.0f);
				p2Point.set_vecz(0.0f);

				auto sendBuf = ClientPacketHandler::MakeSendBuffer(p2Point);
				auto session = gpClient->GetClientservice()->GetSessions().begin();
				(*session)->Send(sendBuf);
			}
		}
	}
}



void MultiBattleScene::EnemyTurnProcess()
{
	TurnState = false;

	InActionEnemyNum = 0;
	InActionEnemy = EnemyList[InActionEnemyNum];
}

void MultiBattleScene::EnemyAnimProcess()
{
	// 적 턴이고, 현재 행동중인 적이 아직 이동이나 공격을 하지 않았을 경우
	if (!TurnState && (!InActionEnemy->doMove || !InActionEnemy->doAction))
	{
		// 현재 행동중인 적에게서 가장 가까운 아군을 타겟팅
		float distance = 100000.0f;
		for (auto player : PlayerList)
		{
			Vector3 ForDis = InActionEnemy->chara->Transform->Translation - player->chara->Transform->Translation;
			if (ForDis.Length() < distance)
			{
				InActionEnemy->TargetPlayer = player;
				distance = ForDis.Length();
			}
		}

		// 적이 아직 이동하지 않았을 경우
		if (!InActionEnemy->doMove)
		{
			InActionEnemy->move(InActionEnemy->TargetPlayer, TurnNum);

			// 행동중인 적 쪽으로 타겟이 된 플레이어 캐릭터 회전
			Vector3 NewPlayerForward = InActionEnemy->chara->Transform->Translation - InActionEnemy->TargetPlayer->chara->Transform->Translation;
			NewPlayerForward.Normalize();
			DirectX::SimpleMath::Quaternion quaternion = Quaternion::FromToRotation(InActionEnemy->TargetPlayer->Forward, NewPlayerForward);
			Vector3 eulerAngles = quaternion.ToEuler();
			eulerAngles.y = DirectX::XMConvertToDegrees(eulerAngles.y);
			InActionEnemy->TargetPlayer->chara->Transform->Rotation += eulerAngles;
			InActionEnemy->TargetPlayer->Forward = NewPlayerForward;

			// 카메라 워크
			auto CameraTarget = InActionEnemy->chara->Transform->Translation;
			CameraTarget.y += 10.0f;
			TargetPlayer = InActionEnemy->TargetPlayer;
			UpdateCameraPos();

			MoveCamera->CreateViewMatrix(PlayerCameraPos, CameraTarget, Vector3(0.0f, 1.0, 0.0f));
			MainCameraSystem->TargetCamera = MoveCamera;
		}

		// 이동이 끝나고, 적이 아직 행동하지 않았을 경우
		else if (InActionEnemy->doMove && !InActionEnemy->chara->MovementComp->IsMoving && !InActionEnemy->doAction)
		{
			EnemyDamage = InActionEnemy->patern(InActionEnemy->TargetPlayer, TurnNum);
			UpdateEnemyState();

			// 카메라 워크
			Vector3 t1 = InActionEnemy->TargetPlayer->chara->GetComponent<BoundingBoxComponent>()->OBB.Center;
			Vector3 t2 = InActionEnemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center;
			float dist = t1.Distance(t1, t2);
			TestActionCamera->Lerp(t1, t2, 50.0f, 0.1f);

			MoveCamera->CreateViewMatrix(TestActionCamera->MovementComp->Destination, TestActionCamera->ActionCameraComp->NextLook, Vector3(0.0f, 1.0, 0.0f));
			MainCameraSystem->TargetCamera = MoveCamera;
		}
	}

	// 적 턴이고, 현재 행동중이었던 적의 이동&행동이 끝나면 잡힘
	else if (!TurnState && InActionEnemy->doMove && InActionEnemy->doAction && !InActionEnemy->chara->GetComponent<AnimationComponent>()->IsInAction())
	{
		// 초기화
		InActionEnemy->doMove = false;
		InActionEnemy->doAction = false;

		if (InActionEnemy == EnemyList[EnemyList.size() - 1]) // 지금 행동중인 적이 마지막 적이라면  
		{
			TurnState = true;
			TurnStart = true;
			InActionEnemyNum = 0;
			InActionEnemy = EnemyList[InActionEnemyNum];
			return;
		}
		InActionEnemyNum++;
		InActionEnemy = EnemyList[InActionEnemyNum];
	}
}

static float MoveTimer = 0.0f;
void MultiBattleScene::MoveProcess()
{
	// 이동버튼 눌리고 최초 실행시에만, 카메라 바꿔줌
	if (MoveTimer == 0.0f)
	{
		Vector3 SkyCamPos = PlayerCameraPos;
		SkyCamPos -= CurrentPlayer->Forward * 200.0f;
		SkyCamPos.y += 150.0f;
		Vector3 CameraTarget;
		MoveCamera->CreateViewMatrix(SkyCamPos, CameraTarget, Vector3(0.0f, 1.0, 0.0f));
		MainCameraSystem->TargetCamera = MoveCamera;
	}

	// 딜레이주려고
	MoveTimer += Timer::GetInstance()->GetDeltaTime();

	if (MoveTimer >= 0.3f)
	{
		// 이동 끝날때까지 UI 숨겨줌, 취소버튼? 그런건없다
		WRS->hide = true;

		// 땅을 찍은 적이 없다면 (이동 명령을 아직 내리지 않았다면)
		if (MAIN_PICKER.optPickingMode != PMOD_CHARACTER)
		{
			// 거리가 일정 수치 이상이면 이동불가
			Vector3 Forward = CurrentPlayer->chara->Transform->Translation - MAIN_PICKER.vIntersection;
			float CanMoveRange = 100.0f; // 수치 설정
			float Distance = Forward.Length();
			if (Distance < CanMoveRange)
			{
				MAIN_PICKER.SetMoveUIColor(&TheWorld, { 0.0f, 0.0f, 0.0f, 0.5f }, { 0.5f, 1.0f, 0.0f, 0.5f });

				// 땅을 찍었다면, 이동명령 내리고 카메라 바꿔줌
				KeyState btnLC = Input::GetInstance()->getKey(VK_LBUTTON);
				if (btnLC == KeyState::Down)
				{
					CurrentPlayer->chara->MoveTo(MAIN_PICKER.vIntersection);
					MAIN_PICKER.optPickingMode = PMOD_CHARACTER;
				}
			}
			else
			{
				MAIN_PICKER.SetMoveUIColor(&TheWorld, { 0.5f, 0.0f, 0.0f, 0.5f }, { 0.75f, 0.0f, 0.0f, 0.5f });
			}
		}

		// 땅을 찍었고, 플레이어의 이동이 끝났다면
		else if (!PlayerCharacter->MovementComp->IsMoving)
		{
			IsMoving = false;
			MoveTimer = 0.0f;
			WRS->hide = false;

			// 타겟팅한 적이 없다면 자동으로 첫번째 적 타겟팅
			if (TargetEnemy == nullptr) { TargetEnemy = EnemyList[0]; }
			// 플레이어 캐릭터 방향 회전 (타겟팅한 적 쪽으로)
			CurrentPlayer->Forward = CurrentPlayer->chara->MovementComp->Forward;
			Vector3 NewPlayerForward = TargetEnemy->chara->Transform->Translation - CurrentPlayer->chara->Transform->Translation;
			NewPlayerForward.Normalize();
			DirectX::SimpleMath::Quaternion quaternion = Quaternion::FromToRotation(CurrentPlayer->Forward, NewPlayerForward);
			Vector3 eulerAngles = quaternion.ToEuler();
			eulerAngles.y = DirectX::XMConvertToDegrees(eulerAngles.y);
			CurrentPlayer->chara->Transform->Rotation += eulerAngles;
			CurrentPlayer->Forward = NewPlayerForward;

			// 적 캐릭터 방향 회전 (플레이어 캐릭터 쪽으로)
			for (auto enemy : EnemyList)
			{
				Vector3 CurrentForward = enemy->chara->MovementComp->Forward;
				Vector3 NewForward = CurrentPlayer->chara->Transform->Translation - enemy->chara->Transform->Translation;
				NewForward.Normalize();
				DirectX::SimpleMath::Quaternion quaternion = Quaternion::FromToRotation(CurrentForward, NewForward);
				Vector3 eulerAngles = quaternion.ToEuler();
				eulerAngles.y = DirectX::XMConvertToDegrees(eulerAngles.y);
				enemy->chara->Transform->Rotation += eulerAngles;
				enemy->chara->MovementComp->Forward = NewForward;
			}

			// 카메라 셋팅
			auto CameraTarget = TargetEnemy->chara->Transform->Translation;
			CameraTarget.y += 10.0f;
			TargetPlayer = CurrentPlayer;
			UpdateCameraPos();
			MainCameraSystem->MainCamera = MainCamera;
			MoveCamera->CreateViewMatrix(PlayerCameraPos, CameraTarget, Vector3(0.0f, 1.0, 0.0f));
			MainCameraSystem->TargetCamera = MoveCamera;

			// 턴당 이동은 한번으로 제한, 코스트 먹을지 말지는 뭐 모르겠다
			MoveButton->m_bIsDead = true;
		}
	}
}

void MultiBattleScene::CardCheck()
{
	if (TargetEnemy == nullptr || TargetEnemy->hp <= 0) { return; } // 타겟이 없거나 죽었다면 실행ㄴ

	for (int cardNum = 0; cardNum < MyDeck->HandList.size(); cardNum++)
	{
		if (CardList[cardNum]->m_bClicked && CardList[cardNum]->m_OriginPos.y >= 0.5)	// 카드를 드래그해서 일정 높이 이상으로 올라갔다면
		{
			bool CanUse = false;
			int	 DrawedCard = 0;

			CardList[cardNum]->m_bClicked = false;
			CardList[cardNum]->m_OriginPos = CardList[cardNum]->m_OriginalOriginPos;

			auto PAnime = CurrentPlayer->chara->GetComponent<AnimationComponent>();
			auto EAnime = TargetEnemy->chara->GetComponent<AnimationComponent>();

			switch (MyDeck->HandList[cardNum])
			{

			case Strike:
			{
				if (ManaCheck(1))
				{
					TargetEnemy->hp -= 6;
					PlayerDamage = 6;
					CanUse = true;

					// 일단 덱 같은거 쓰고있어봐..
					if (playerNum == 1) { PAnime->SetClipByName(L"Shooting"); }
					else { PAnime->SetClipByName(L"Chapa_2"); }
				}
			}break;

			case Defend:
			{
				if (ManaCheck(1))
				{
					CurrentPlayer->armor += 5;
					CanUse = true;
				}

				PAnime->SetClipByName(L"Inward_Block");
				PlayEffect(&TheWorld, L"Shield1", { CurrentPlayer->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {3.0f, 3.0f, 3.0f} }, { false, 1.0f, 0.2f, 1.0f });
				SoundMap.find(L"Shield")->second->Play();
			}break;

			case PommelStrike:
			{
				if (ManaCheck(1))
				{
					TargetEnemy->hp -= 9;
					PlayerDamage = 9;
					MyDeck->Draw(1);
					DrawedCard = 1;
					CanUse = true;

					// 일단 덱 같은거 쓰고있어봐..
					if (playerNum == 1) { PAnime->SetClipByName(L"Gunplay"); }
					else { PAnime->SetClipByName(L"Chapa_Giratoria_2"); }
				}
			}break;

			case ShrugItOff:
			{
				if (ManaCheck(1))
				{
					CurrentPlayer->armor += 8;
					MyDeck->Draw(1);
					DrawedCard = 1;
					CanUse = true;

					PAnime->SetClipByName(L"Inward_Block");
					PlayEffect(&TheWorld, L"Shield1", { CurrentPlayer->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {3.0f, 3.0f, 3.0f} }, { false, 1.0f, 0.2f, 1.0f });
					SoundMap.find(L"Shield2")->second->Play();
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
					PlayerDamage = 5;
					CurrentPlayer->armor += 5;
					CanUse = true;

					// 일단 덱 같은거 쓰고있어봐..
					if (playerNum == 1) { PAnime->SetClipByName(L"Pistol_Whip"); }
					else { PAnime->SetClipByName(L"Kick"); }
				}
			}break;

			}

			if (CanUse)
			{
				// 내가 1P일때
				if ((gpHost != nullptr && gpHost->IsConnected()))
				{
					protocol::S_USECARD hostUseCard;
					for (int i = 0; i < EnemyList.size(); i++)
					{
						if (EnemyList[i] == TargetEnemy) { hostUseCard.set_targetenemynum(i); }
					}
					hostUseCard.set_usedcardnum(MyDeck->HandList[cardNum]);

					auto sendBuf = ServerPacketHandler::MakeSendBuffer(hostUseCard);
					auto session = gpHost->GetService()->GetSessions().begin();
					(*session)->Send(sendBuf);
				}
				// 내가 2P일때
				if (gpClient != nullptr && gpClient->IsConnected())
				{
					protocol::C_USECARD clientUseCard;
					for (int i = 0; i < EnemyList.size(); i++)
					{
						if (EnemyList[i] == TargetEnemy) { clientUseCard.set_targetenemynum(i); }
					}
					clientUseCard.set_usedcardnum(MyDeck->HandList[cardNum]);

					auto sendBuf = ClientPacketHandler::MakeSendBuffer(clientUseCard);
					auto session = gpClient->GetClientservice()->GetSessions().begin();
					(*session)->Send(sendBuf);
				}

				MyDeck->Use(cardNum);
				UpdateHand(MyDeck->HandList.size(), cardNum, DrawedCard);
				UpdatePlayerState();

				if (TargetEnemy->hp <= 0)
				{
					// 적 캐릭터가 죽었다면 적 리스트에서 빼줌
					auto iter = std::find(EnemyList.begin(), EnemyList.end(), TargetEnemy);
					if (iter != EnemyList.end()) { EnemyList.erase(iter); }

					// 상태창 꺼줌
					for (auto obj : TargetEnemy->ObjList) { obj->m_bIsDead = true; }
					TargetEnemy->HpEmpty->m_bIsDead = true;
					TargetEnemy->HpFull->m_bIsDead = true;
				}

				// 카메라 워크
				Vector3 t1 = CurrentPlayer->chara->GetComponent<BoundingBoxComponent>()->OBB.Center;
				Vector3 t2 = TargetEnemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center;
				float dist = t1.Distance(t1, t2);
				if (dist < 30.0f) // 어느정도 이상 가까울 때만 카메라 변경
				{
					dist = 40.0f;
					TestActionCamera->Lerp(t1, t2, dist, 0.1f);

					MoveCamera->CreateViewMatrix(TestActionCamera->MovementComp->Destination, TestActionCamera->ActionCameraComp->NextLook, Vector3(0.0f, 1.0, 0.0f));
					MainCameraSystem->TargetCamera = MoveCamera;
				}
			}
			else {} // 여기서 마나부족 경고문구 출력

		}
	}
}

void MultiBattleScene::DamageAnimation(int damage, bool hitEnemy)
{
	int fontWidth = 140.0f;	// 한 글자 너비

	// 아군 캐릭터가 적 공격, 뭐 큰차인 없고 걍 데미지 날라가는 방향
	if (hitEnemy)
	{
		// 데미지가 10 이상일 경우, 두자리수 ㅇㅇ
		if (damage / 10 > 0)
		{
			Damage1->m_bIsDead = false;
			Damage1->m_pCutInfoList[0]->tc = NumberTextureList_Damage[damage / 10];
			Vector2 AnimPos[2] = { {1000.0f, 450.0f},{1400.0f, 250.0f} };
			Damage1->SetAnimation(AnimPos, 1.2f, true);
		}
		Damage2->m_bIsDead = false;
		Damage2->m_pCutInfoList[0]->tc = NumberTextureList_Damage[damage % 10];
		Vector2 AnimPos[2] = { {1000.0f + fontWidth, 450.0f},{1400.0f + fontWidth, 250.0f} };
		Damage2->SetAnimation(AnimPos, 1.2f, true);
	}

	// 적이 아군 공격
	else
	{	// 데미지가 10 이상일 경우, 두자리수 ㅇㅇ
		if (damage / 10 > 0)
		{
			Damage1->m_bIsDead = false;
			Damage1->m_pCutInfoList[0]->tc = NumberTextureList_Damage[damage / 10];
			Vector2 AnimPos[2] = { {400.0f, 450.0f},{0.0f, 250.0f} };
			Damage1->SetAnimation(AnimPos, 1.2f, true);
		}
		Damage2->m_bIsDead = false;
		Damage2->m_pCutInfoList[0]->tc = NumberTextureList_Damage[damage % 10];
		Vector2 AnimPos[2] = { {400.0f + fontWidth, 450.0f},{0.0f + fontWidth, 250.0f} };
		Damage2->SetAnimation(AnimPos, 1.2f, true);
	}
}

void MultiBattleScene::Reaction()
{
	// 다른 플레이어가 턴종버튼을 눌렀을 경우, 턴종 프로세스를 실행함
	if (hostTurnEnd || clientTurnEnd)
	{
		TurnEndProcess();
		hostTurnEnd = false;
		clientTurnEnd = false;
	}

	if (OtherPlayerUsedCardNum != 999) // 상대방이 카드를 사용했을 때
	{
		TargetEnemy = EnemyList[OtherPlayerTargetEnemyNum];

		CardList[0]->m_bIsDead = false;
		CardList[0]->m_pCutInfoList[0]->tc = CardTextureList[OtherPlayerUsedCardNum];
		Vector2 AnimPos[2] = { {800.0f, 450.0f},{800.0f, 450.0f} };
		CardList[0]->SetAnimation(AnimPos, 1.0f, true);

		auto PAnime = CurrentPlayer->chara->GetComponent<AnimationComponent>();
		auto EAnime = TargetEnemy->chara->GetComponent<AnimationComponent>();

		switch (OtherPlayerUsedCardNum)
		{

		case Strike:
		{
			TargetEnemy->hp -= 6;
			PlayerDamage = 6;

			if (playerNum == 2) { PAnime->SetClipByName(L"Shooting"); }
			else { PAnime->SetClipByName(L"Chapa_2"); }
		}break;

		case Defend:
		{
			CurrentPlayer->armor += 5;
			PAnime->SetClipByName(L"Inward_Block");
			PlayEffect(&TheWorld, L"Shield1", { CurrentPlayer->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {3.0f, 3.0f, 3.0f} }, { false, 1.0f, 0.2f, 1.0f });
			SoundMap.find(L"Shield")->second->Play();
		}break;

		case PommelStrike:
		{
			TargetEnemy->hp -= 9;
			PlayerDamage = 9;

			if (playerNum == 2) { PAnime->SetClipByName(L"Gunplay"); }
			else { PAnime->SetClipByName(L"Chapa_Giratoria_2"); }
		}break;

		case ShrugItOff:
		{
			CurrentPlayer->armor += 8;

			PAnime->SetClipByName(L"Inward_Block");
			PlayEffect(&TheWorld, L"Shield1", { CurrentPlayer->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {3.0f, 3.0f, 3.0f} }, { false, 1.0f, 0.2f, 1.0f });
			SoundMap.find(L"Shield")->second->Play();
		}break;

		case Hemokinesis:
		{

		}break;

		case Bludgeon:
		{

		}break;

		case IronWave:
		{
			TargetEnemy->hp -= 5;
			PlayerDamage = 5;
			CurrentPlayer->armor += 5;

			if (playerNum == 2) { PAnime->SetClipByName(L"Pistol_Whip"); }
			else { PAnime->SetClipByName(L"Kick"); }
		}break;

		}
		UpdatePlayerState();
		OtherPlayerUsedCardNum = 999;
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
			CardList[card]->m_pCutInfoList[ci]->tc = CardTextureList[MyDeck->HandList[card]];
		}
		CardList[card]->m_bIsDead = false;
		CardList[card]->m_fAlpha = 1.0f;
		// Vector2 AnimPos[2] = { CardList[card]->NtoP_Pos(CardList[card]->m_OriginalOriginPos), CardPosList[card][handSize - 1] };
		Vector2 AnimPos[2] = { { -200.0f, 700.0f }, CardPosList[card][handSize - 1] };
		CardList[card]->SetAnimation(AnimPos, 0.5f);
	}

	for (int card = handSize; card < 3; card++) // for (int card = handSize; card < CardList.size(); card++)
	{
		CardList[card]->m_bIsDead = true;
	}
}

void MultiBattleScene::UpdateHand(int handSize, int UsedCard, int DrawedCard)
{
	for (int card = 0; card < handSize; card++)
	{
		for (int ci = 0; ci < 4; ci++)
		{
			CardList[card]->m_pCutInfoList[ci]->tc = CardTextureList[MyDeck->HandList[card]];
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

void MultiBattleScene::UpdatePlayerState()
{
	CurrentMana->m_pCutInfoList[0]->tc = NumberTextureList_Black[CurrentPlayer->cost];
	MaxMana->m_pCutInfoList[0]->tc = NumberTextureList_Black[CurrentPlayer->maxCost];

	PlayerCurrenHP1->m_pCutInfoList[0]->tc = NumberTextureList_Black[CurrentPlayer->hp / 10];
	PlayerCurrenHP2->m_pCutInfoList[0]->tc = NumberTextureList_Black[CurrentPlayer->hp % 10];
	PlayerMaxHP1->m_pCutInfoList[0]->tc = NumberTextureList_Black[CurrentPlayer->maxHp / 10];
	PlayerMaxHP2->m_pCutInfoList[0]->tc = NumberTextureList_Black[CurrentPlayer->maxHp % 10];

	// 프로그레스 바 (HP바) 
	float hpPercentage = (float)CurrentPlayer->hp / (float)CurrentPlayer->maxHp;
	PlayerHPBar->m_pCutInfoList[0]->pxWH.x = PlayerHPBar->m_OriginWH.x * hpPercentage;		// X축만(가로로만)줄어들어야하니까
	PlayerHPBar->m_pCutInfoList[0]->uv = PlayerHPBar->m_OriginUV * hpPercentage;			// x축만 ㅇㅇ


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
		if ((player1->armor / 10) >= 1)
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
	// 아 이거 모든 플레이어가 죽으면 씬전환으로 바뀌어야 하는데 몰라 지금안할꺼야
	if (CurrentPlayer->hp <= 0 && !PlayerCharacter->GetComponent<AnimationComponent>()->IsInAction())
	{
		for (auto enemy : AllEnemyList)
		{
			if (enemy->chara->GetComponent<AnimationComponent>()->IsInAction()) return; // 적이 하나라도 애니메이션중이라면 탈출!
		}
		SS = GAMEOVER;
	}
	else
	{
		for (auto enemy : AllEnemyList)
		{
			if (enemy->hp > 0 || enemy->chara->GetComponent<AnimationComponent>()->IsInAction()) return; // 적이 하나라도 살아있거나 죽는 애니메이션중이라면 탈출!
			else if (CurrentPlayer->chara->GetComponent<AnimationComponent>()->IsInAction()) { return; }	// 플레이어가 행동중이어도 안끝나게, 그,, 노티파이맞추면서암튼이래야함
		}
		SS = CLEAR;
	}
}

void MultiBattleScene::ToolTipCheck()
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
	else if (MoveButton->m_bHover && !MoveButton->m_bIsDead) // 이동 버튼
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
