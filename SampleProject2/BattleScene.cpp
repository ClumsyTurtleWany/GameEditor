#include "BattleScene.h"
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
#include "MotionTrail.h"

///////////////////
//Effect Include
///////////////////
#include "EffectInclude/EffectSystem.h"


bool BattleScene::Init()
{
	MainSaveLoadSystem = new SaveLoadSystem;
	TheWorld.AddSystem(MainSaveLoadSystem);

	ID = BATTLE;

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

	// 사실 플레이어는 타이틀에서 함 초기화하고 가는것도..
	player = new Player;
	player->cost = player->maxCost;
	player->maxHp = 50;
	player->hp = 50;
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

	// 잘죽는지 테스트용
	// player->hp = 1;
	// enemy1->hp = 5;
	// enemy2->hp = 10;


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
	Init_Effect();

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

	MainRenderSystem->pTest = new MotionTrail;

	MainRenderSystem->pTest->create(
		DXDevice::g_pd3dDevice,
		DXDevice::g_pImmediateContext,
		&TheWorld,
		player->chara,
		MainCameraSystem->MainCamera,
		6,
		1.25f,
		0.2f
	);

	MainRenderSystem->pTest->play();

	SkyRenderSystem* skyRenderSystem = new SkyRenderSystem;
	skyRenderSystem->MainCamera = MainCameraSystem->MainCamera;
	TheWorld.AddSystem(skyRenderSystem);

	WRS = new WidgetRenderSystem;
	TheWorld.AddSystem(WRS);
	MAIN_PICKER.SetPickingMode(PMOD_CHARACTER);



	//////  초기 카메라 세팅  //////
	
	// 플레이어 캐릭터 포워드 벡터 셋팅
	auto PlayerForward = PlayerCharacter->MovementComp->Destination - PlayerCharacter->Transform->Translation;
	PlayerForward.Normalize();
	player->Forward = PlayerForward;

	// Right 벡터 구할라고
	DirectX::XMFLOAT3 upVec(0.0f, 1.0f, 0.0f);
	Vector3 Right;	// Calculate the right vector	
	DirectX::XMFLOAT3 crossProduct;
	XMStoreFloat3(&crossProduct, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&player->Forward), XMLoadFloat3(&upVec)));
	DirectX::XMStoreFloat3(&Right, DirectX::XMVector3Normalize(XMLoadFloat3(&crossProduct)));

	// 카메라 위치 세팅, 나중에 또 업데이트 필요할듯 (주로 이동 후에)
	PlayerCameraPos = PlayerCharacter->MovementComp->Destination;	// 플레이어 도착예상지점 기준..
	PlayerCameraPos -= player->Forward * 1.0f;	// 플레이어 캐릭터가 향하는 방향에서 살짝 뒤로 뺌
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

bool BattleScene::Frame()
{
	BaseScene::Frame();

	// 적 상태창 업데이트
	if(TurnState) // 적 턴에는 그냥 적 상태창 안보이게끔
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

	// A키 누르면 타겟변경
	KeyState btnA = Input::GetInstance()->getKey('A');
	if (btnA == KeyState::Down)
	{
		if (TargetEnemy != nullptr) 
		{
			int enemyNum = 0;
			for (int i = 0; i <EnemyList.size(); i++)
			{
				if (EnemyList[i] == TargetEnemy)
				{ enemyNum = i; }
			
			}
			// 현재 타겟중인 적이 마지막 적이라면, 첫 적으로 돌림
			if (enemyNum + 1 == EnemyList.size()) { TargetEnemy = EnemyList[0]; }
			else { TargetEnemy = EnemyList[enemyNum + 1]; }

			/*auto PlayerForward = PlayerCharacter->MovementComp->Destination - PlayerCharacter->Transform->Translation; 
			PlayerForward.Normalize();
			player->Forward = PlayerForward;*/

			// 캐릭터 방향 회전, 애니메이션 줄라면 줄수있는데 여유생기면,,머,,
			Vector3 NewPlayerForward = TargetEnemy->chara->Transform->Translation - player->chara->Transform->Translation;
			NewPlayerForward.Normalize();
			DirectX::SimpleMath::Quaternion quaternion = Quaternion::FromToRotation(player->Forward, NewPlayerForward);
			Vector3 eulerAngles = quaternion.ToEuler();

			Vector3 cross = DirectX::XMVector3Cross(player->Forward, NewPlayerForward);
			Vector3 dot = DirectX::XMVector3Dot(cross, Vector3(0,1,0));

			if (dot.y < 0.f)
			{
				// 좌우 판별
			}
			
			eulerAngles.y = DirectX::XMConvertToDegrees(eulerAngles.y);
			//player->chara->Transform->Rotation += eulerAngles;
			auto timeline = PlayerCharacter->GetComponent<TimelineComponent>();
			if (timeline != nullptr)
			{
				timeline->CreateTimeline(player->chara->Transform->Rotation.y, player->chara->Transform->Rotation.y+eulerAngles.y,0.2f);
				//timeline->SetTarget(&player->chara->Transform->Rotation.y); // 컴포넌트 추가할 때 미리 해두기?
				timeline->PlayFromStart();
			}
			
			player->Forward = NewPlayerForward;

			// 카메라 타겟 세팅
			auto CameraTarget = TargetEnemy->chara->Transform->Translation;
			CameraTarget.y += 10.0f;
			UpdateCameraPos();

			MoveCamera->CreateViewMatrix(PlayerCameraPos, CameraTarget, Vector3(0.0f, 1.0, 0.0f));
			MainCameraSystem->TargetCamera = MoveCamera;
			
			// 락온 사운드 출력
			SoundMap.find(L"LockOn")->second->Play();
		}
	}

	KeyState btnD = Input::GetInstance()->getKey('D');
	if (btnD == KeyState::Hold || btnD == KeyState::Down)
	{
		MainCameraSystem->TargetCamera = enemy1->chara->GetComponent<Camera>();
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
		if (TestActionCamera != nullptr)
		{
			TestActionCamera->Damped(20.0f, 2.0f, 10.0f, 10.0f);
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
		auto boundingBox = PlayerCharacter->GetComponent<BoundingBoxComponent>();
		if (TestActionCamera != nullptr)
		{
			TestActionCamera->LookAt(boundingBox->OBB.Center, 0.5f);
		}
	}
	KeyState btnX = Input::GetInstance()->getKey('X');
	if (btnX == KeyState::Hold || btnX == KeyState::Down)
	{
		auto boundingBox = EnemyCharacter->GetComponent<BoundingBoxComponent>();
		if (TestActionCamera != nullptr)
		{
			TestActionCamera->LookAt(boundingBox->OBB.Center, 0.5f);
		}
	}
	KeyState btnC = Input::GetInstance()->getKey('C');
	if (btnC == KeyState::Hold || btnC == KeyState::Down)
	{
		MainCameraSystem->MainCamera = MainCamera;
	}
	KeyState btnV = Input::GetInstance()->getKey('V');
	if (btnV == KeyState::Hold || btnV == KeyState::Down)
	{
		auto boundingBox1 = PlayerCharacter->GetComponent<BoundingBoxComponent>();
		auto boundingBox2 = EnemyCharacter->GetComponent<BoundingBoxComponent>();
		Vector3 t1 = boundingBox1->OBB.Center;
		Vector3 t2 = boundingBox2->OBB.Center;
		float dist = t1.Distance(t1, t2);
		if (TestActionCamera != nullptr)
		{
			TestActionCamera->Lerp(t1, t2, dist, 1.0f);
		}
	}
	KeyState btnB = Input::GetInstance()->getKey('B');
	if (btnB == KeyState::Hold || btnB == KeyState::Down)
	{
		auto boundingBox1 = PlayerCharacter->GetComponent<BoundingBoxComponent>();
		auto boundingBox2 = EnemyCharacter->GetComponent<BoundingBoxComponent>();
		Vector3 t1 = boundingBox1->OBB.Center;
		Vector3 t2 = boundingBox2->OBB.Center;
		
		if (TestActionCamera != nullptr)
		{
			TestActionCamera->MoveTo(t1, 1.0f);
			TestActionCamera->LookAt(t2, 1.0f);
		}
	}
	KeyState btnN = Input::GetInstance()->getKey('N');
	if (btnN == KeyState::Hold || btnN == KeyState::Down)
	{
		auto boundingBox1 = PlayerCharacter->GetComponent<BoundingBoxComponent>();
		auto boundingBox2 = EnemyCharacter->GetComponent<BoundingBoxComponent>();
		Vector3 t1 = boundingBox1->OBB.Center;
		Vector3 t2 = boundingBox2->OBB.Center;

		if (TestActionCamera != nullptr)
		{
			TestActionCamera->MoveTo(t2, 1.0f);
			TestActionCamera->LookAt(t1, 1.0f);
		}
	}

	// 얘는 일단 주기적으로 업데이트 해줘야함.
	MainRenderSystem->MainCamera = MainCameraSystem->MainCamera;

	//Effect test
	if (Input::GetInstance()->getKey(VK_F7) == KeyState::Up)
	{
		MainRenderSystem->pTest->targetChange(enemy1->chara);
	}

	if (Input::GetInstance()->getKey(VK_F8) == KeyState::Up)
	{
		MainRenderSystem->pTest->targetChange(player->chara);
	}

	if (Input::GetInstance()->getKey(VK_F9) == KeyState::Up)
	{
		MainRenderSystem->pTest->play();
	}

	if (Input::GetInstance()->getKey(VK_F10) == KeyState::Up)
	{
		MainRenderSystem->pTest->Pause();
	}

	//PickedCharacter = (Character*)MAIN_PICKER.curSelect.pTarget;
	//for (auto enemy : EnemyList)
	//{
	//	if (PickedCharacter == enemy->chara)
	//	{
	//		for (auto obj : enemy->ObjList) // 이쪽은 나중에 빌보드 띄우고 나면 쓸모없을지도.. 흠
	//		{
	//			obj->m_bIsDead = false;
	//		}	

	//		if (TargetEnemy != enemy) // 타겟팅이 바뀌었을 때 한번만
	//		{
	//			TargetEnemy = enemy;

	//			// 캐릭터 방향 회전, 애니메이션 줄라면 줄수있는데 여유생기면,,머,,
	//			Vector3 NewPlayerForward = TargetEnemy->chara->Transform->Translation - player->chara->Transform->Translation;
	//			NewPlayerForward.Normalize();
	//			DirectX::SimpleMath::Quaternion quaternion = Quaternion::FromToRotation(player->Forward, NewPlayerForward);
	//			Vector3 eulerAngles = quaternion.ToEuler();
	//			eulerAngles.y = DirectX::XMConvertToDegrees(eulerAngles.y);
	//			player->chara->Transform->Rotation += eulerAngles;
	//			player->Forward = NewPlayerForward;

	//			// 카메라 타겟 세팅
	//			auto CameraStartTarget = TargetEnemy->chara->Transform->Translation;
	//			CameraStartTarget.y += 10.0f;

	//			MoveCamera->CreateViewMatrix(PlayerCameraPos, CameraStartTarget, Vector3(0.0f, 1.0, 0.0f));
	//			MainCameraSystem->TargetCamera = MoveCamera;
	//		}
	//	}
	//	else
	//	{
	//		if (enemy != TargetEnemy)
	//		{
	//			for (auto obj : enemy->ObjList)
	//			{
	//				obj->m_bIsDead = true;
	//			}
	//		}
	//	}
	//}

	//for (auto enemy : EnemyList) 
	//{
	//	for (auto obj : enemy->ObjList) 
	//	{
	//		obj->m_OrginPos3D = enemy->chara->MovementComp->Location;
	//		obj->m_OrginPos3D.y += enemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center.y * 2.0f;
	//		obj->m_OrginPos3D.y += 3.0f;
	//	}
	//}

	for (auto enemy : EnemyList) 
	{
		enemy->HpEmpty->m_OrginPos3D = enemy->chara->MovementComp->Location;
		enemy->HpEmpty->m_OrginPos3D.y += enemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center.y * 2.0f;
		//enemy->HpEmpty->m_OrginPos3D.y += 1.0f;
		enemy->HpFull->m_OrginPos3D = enemy->chara->MovementComp->Location;
		enemy->HpFull->m_OrginPos3D.y += enemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center.y * 2.0f;
		//enemy->HpFull->m_OrginPos3D.y += 1.0f;
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
	CardList[0]->m_OriginalOriginPos = CardList[0]->m_OriginPos;
	CardList[0]->m_OriginalOriginPos = CardList[0]->m_OriginPos;
	CardList[0]->m_OriginalOriginPos = CardList[0]->m_OriginPos;
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

	//Actor* containerShip = new Actor;
	//auto staticMesh = containerShip->AddComponent<StaticMeshComponent>();
	////if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Map/Container/Shipping_Container_C.fbx"))
	////{
	////	FBXLoader::GetInstance()->GenerateStaticMeshFromFileData(L"../resource/FBX/Map/Container/Shipping_Container_C.fbx", staticMesh);
	////	FBXLoader::GetInstance()->SaveSTMFromSTComp(staticMesh);
	////}
	//FBXLoader::GetInstance()->LoadStaticMesh("../resource/FBX/Shipping_Container_C/Shipping_Container_C.stm", staticMesh);
	//auto transform = containerShip->GetComponent<TransformComponent>();
	//transform->Scale = Vector3(0.1f, 0.1f, 0.1f);
	//transform->Translation = Vector3(0.0f, 3.2f, 0.2f);
	////transform->Rotation = Vector3(0.0f, 0.0f, 0.0f);
	////transform->Translation = Vector3(500.0f, 0.0f, -500.0f + static_cast<float>(cnt) * 125);
	//TheWorld.AddEntity(containerShip);
	//

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
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Hit.clp", playerCharAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Dying.clp", playerCharAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Shooting.clp", playerCharAnimComp, false);	// 일반사격
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Gunplay.clp", playerCharAnimComp, false);	// 좀쎈사격
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Pistol_Whip.clp", playerCharAnimComp, false); // 죽탱이
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Inward_Block.clp", playerCharAnimComp, false); // 가드
	playerCharAnimComp->SetClipByName(L"Idle");

	///////////////// NOTIFIER 사용 예제/////////////////
	//// 1. 노티파이어 매니저 컴포넌트를 추가하고
	//auto notiMgr = PlayerCharacter->AddComponent<NotifierMgrComponent>();

	//// 2. 노티파이어 생성 (여러개 생성 가능하며 한 노티파이어에서 사운드, 이펙트 동시에 발생시킬 수 있음)
	////		 생성시 인자는 노티파이어 이름 , 발생시키고자 하는 프레임(정수) , 노티파이어 수명(프레임, 정수) <- 아무값도 안넣으면 1이 기본
	//Notifier* newNoti = notiMgr->CreateNotifier(L"step1", 7);
	//// 3. 이펙트 발생을 위해 노티파이어에 이펙트를 달아주기
	////		 사운드, 이펙트 각각 두가지 방법으로 달 수 있음
	//// 
	////		 MakeFmodSound(노티파이어 이름 , FmodSound*) <- 기존 사운드맵을 활용할 수 있지만 second로 바로 접근하기때문에 로드 안된 거 잘못 넣으면 뻑나니 주의
	//notiMgr->MakeFmodSound(L"step1", SoundMap.find(L"Hit2")->second);

	////		 MakeEffect(노티파이어 이름, 이펙트 키 이름 , 이펙트 위치(트랜스폼 컴포넌트) , 파티클 속성)
	////		 이때 이펙트 위치에 Vector3 값만 넣어도 됨 (오버로딩)
	//notiMgr->MakeEffect(L"step1", L"Hit5", { {0.0f, 10.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {5.0f, 5.0f, 5.0f} }, { false, 0.5f, 0.2f, 1.0f });

	//Notifier* newNoti2 = notiMgr->CreateNotifier(L"step2", 20);

	////		 MakeSound(노티파이어 이름 , 기존에 로드했던 사운드 파일(FmodSoundManager의 사운드 맵에서 받아옴) , 볼륨, 루프 상태)
	//notiMgr->MakeSound(L"step2", L"Attack_Bludgeon.ogg", 0.5f, false);

	//// 4. 만들어진 노티파이어를 애니메이션 컴포넌트에 붙여주기
	////		 AddNotifier(애니메이션 컴포넌트 , 해당 컴포넌트가 들고 있는 애니메이션 클립의 이름 , 생성한 노티파이어)
	//notiMgr->AddNotifier(*playerCharAnimComp, L"Run", newNoti);
	//notiMgr->AddNotifier(*playerCharAnimComp, L"Run", newNoti2);
	
	// 노티파이 설정
	int soundFrame = 0;
	int delayFrame = 0;
	auto notiMgr = PlayerCharacter->AddComponent<NotifierMgrComponent>();
	delayFrame = 5;
	// 일반사격
	soundFrame = 6;
	Notifier* Fire_ready = notiMgr->CreateNotifier(L"Fire_ready", soundFrame - delayFrame, 10);
	Notifier* Fire = notiMgr->CreateNotifier(L"Fire", soundFrame, 10);
	notiMgr->MakeSound(L"Fire", L"Fire1.mp3", 1.0f, false);
	notiMgr->AddNotifier(*playerCharAnimComp, L"Shooting", Fire_ready);
	notiMgr->AddNotifier(*playerCharAnimComp, L"Shooting", Fire);
	//좀쎈사격
	soundFrame = 20;
	Notifier* Fire2_ready = notiMgr->CreateNotifier(L"Fire2_ready", soundFrame - delayFrame, 10);
	Notifier* Fire2 = notiMgr->CreateNotifier(L"Fire2", soundFrame, 10);
	notiMgr->MakeSound(L"Fire2", L"Fire2.mp3", 1.0f, false);
	notiMgr->AddNotifier(*playerCharAnimComp, L"Gunplay", Fire2_ready);
	notiMgr->AddNotifier(*playerCharAnimComp, L"Gunplay", Fire2);
	// 손잡이 후리기
	soundFrame = 17;
	Notifier* Strike_ready = notiMgr->CreateNotifier(L"Strike_ready", soundFrame - delayFrame, 10);
	Notifier* Strike = notiMgr->CreateNotifier(L"Strike", soundFrame, 10);
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
	playerCharMovementComp->Speed = 50.0f;
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
	timeline->SetTarget(&player->chara->Transform->Rotation.y);

	TheWorld.AddEntity(PlayerCharacter);

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
	Notifier* E1A1 = notiMgr->CreateNotifier(L"E1A1", soundFrame, 10);		// Enemy1 Attack1
	notiMgr->MakeSound(L"E1A1", L"E1A1.ogg", 1.0f, false);	
	notiMgr->AddNotifier(*enemyCharAnimComp, L"Attack1", E1A1);
	// 킦꾸
	soundFrame = 25;
	Notifier* E1A2 = notiMgr->CreateNotifier(L"E1A2", soundFrame, 10);
	notiMgr->MakeSound(L"E1A2", L"Attack_Bludgeon.ogg", 1.0f, false);	
	notiMgr->AddNotifier(*enemyCharAnimComp, L"Attack2", E1A2);
	// 쑤꾸림(비명)
	soundFrame = 60;
	Notifier* E1A3 = notiMgr->CreateNotifier(L"E1A3", soundFrame, 10);
	notiMgr->MakeSound(L"E1A3", L"E1A3.mp3", 1.0f, false);	
	notiMgr->AddNotifier(*enemyCharAnimComp, L"Attack3", E1A3);
	// 사망, 소리붙이려고
	soundFrame = 1;
	Notifier* E1Defeat = notiMgr->CreateNotifier(L"E1Defeat", soundFrame, 10);
	notiMgr->MakeSound(L"E1Defeat", L"E1Defeat.ogg", 1.0f, false);
	notiMgr->AddNotifier(*enemyCharAnimComp, L"Dying", E1Defeat);

	auto enemyCharTransformComp = EnemyCharacter->GetComponent<TransformComponent>();
	enemyCharTransformComp->Scale = Vector3(15.0f, 15.0f, 15.0f);
	enemyCharTransformComp->Rotation = Vector3(0.0f, 90.0f, 0.0f);
	enemyCharTransformComp->Translation = Vector3(100.0f, 0.0f, 0.0f);
	//enemyCharTransformComp->Translation =  Vector3(40.0f, 0.0f, 0.0f);

	auto E1MC = EnemyCharacter->GetComponent<MovementComponent>();
	//E1MC->Speed = 40.0f;
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


	auto timeline_e1 = EnemyCharacter->AddComponent <TimelineComponent>();
	timeline_e1->SetTarget(&enemy1->chara->Transform->Rotation.y);

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
	Notifier* E2A1 = notiMgr->CreateNotifier(L"E2A1", soundFrame, 10);		// Enemy2 Attack1
	notiMgr->MakeSound(L"E2A1", L"E2A1.wav", 1.0f, false);
	notiMgr->AddNotifier(*Enemy2_CharAnimComp, L"Attack1", E2A1);
	// 살짝긁기
	soundFrame = 31;
	Notifier* E2A2 = notiMgr->CreateNotifier(L"E2A2", soundFrame, 10);
	notiMgr->MakeSound(L"E2A2", L"E2A2.mp3", 1.0f, false);	
	notiMgr->AddNotifier(*Enemy2_CharAnimComp, L"Attack2", E2A2);
	// 중간긁기
	soundFrame = 31;
	Notifier* E2A3 = notiMgr->CreateNotifier(L"E2A3", soundFrame, 10);
	notiMgr->MakeSound(L"E2A3", L"E2A3.mp3", 1.0f, false);	 
	notiMgr->AddNotifier(*Enemy2_CharAnimComp, L"Attack3", E2A3);
	// 사망, 소리붙이려고
	soundFrame = 1;
	Notifier* E2Defeat = notiMgr->CreateNotifier(L"E2Defeat", soundFrame, 10);
	notiMgr->MakeSound(L"E2Defeat", L"E2Defeat.ogg", 1.0f, false);
	notiMgr->AddNotifier(*Enemy2_CharAnimComp, L"Dying", E2Defeat);

	auto Enemy2_CharTransformComp = EnemyCharacter2->GetComponent<TransformComponent>();
	Enemy2_CharTransformComp->Scale = Vector3(13.0f, 13.0f, 13.0f);
	Enemy2_CharTransformComp->Rotation = Vector3(0.0f, 90.0f, 0.0f);
	Enemy2_CharTransformComp->Translation = Vector3(70.0f, 0.0f, 100.0f);
	//Enemy2_CharTransformComp->Translation = Vector3(20.0f, 0.0f, 70.0f);

	auto Enemy2_CharMovementComp = EnemyCharacter2->GetComponent<MovementComponent>();
	//Enemy2_CharMovementComp->Speed = 50.0f;
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

	auto timeline_e2 = EnemyCharacter2->AddComponent <TimelineComponent>();
	timeline_e2->SetTarget(&enemy2->chara->Transform->Rotation.y);

	TheWorld.AddEntity(EnemyCharacter2);
}

void BattleScene::Init_Effect()
{
	//Effect Test
	ECS::EffectSystem* ESystem = new ECS::EffectSystem;
	ESystem->init(&TheWorld);
	TheWorld.AddSystem(ESystem);

	ParticleEffect* pMoveUICursor = new ParticleEffect(L"Arrow", 
		TransformComponent(),
		{ true, true, 10.0f, 0.0f, 1.0f });
	TheWorld.AddEntity(pMoveUICursor);

	MAIN_PICKER.pMoveUIEntity = pMoveUICursor;

	ParticleEffect* pSelectedArrowUICursor = new ParticleEffect(L"BlackLineArrow",
		TransformComponent(),
		{ false, true, 10.0f, 0.0f, 1.0f });
	TheWorld.AddEntity(pSelectedArrowUICursor);

	MAIN_PICKER.pSelectedArrowUIEntity = pSelectedArrowUICursor;

	ParticleEffect* pSelectedCircleUICursor = new ParticleEffect(L"FloorCircle",
		TransformComponent(),
		{ false, true, 10.0f, 0.0f, 1.0f });
	TheWorld.AddEntity(pSelectedCircleUICursor);

	MAIN_PICKER.pSelectedCircleUIEntity = pSelectedCircleUICursor;

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
	//testEffectTransform4->Translation = { 40.0f, 20.0f, 0.0f };
	//testEffectTransform5->Translation = { 50.0f, 20.0f, 0.0f };
	//testEffectTransform6->Translation = { 60.0f, 20.0f, 0.0f };
	//testEffectTransform7->Translation = { 70.0f, 20.0f, 0.0f };
	//testEffectTransform8->Translation = { 80.0f, 20.0f, 0.0f };

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

// 카메라 위치 세팅, 이 함수는 플레이어 캐릭터 우상단으로 카메라 위치를 셋팅함
void BattleScene::UpdateCameraPos()
{
	// Right 벡터 구할라고
	DirectX::XMFLOAT3 upVec(0.0f, 1.0f, 0.0f);
	Vector3 Right;	// Calculate the right vector	
	DirectX::XMFLOAT3 crossProduct;
	XMStoreFloat3(&crossProduct, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&player->Forward), XMLoadFloat3(&upVec)));
	DirectX::XMStoreFloat3(&Right, DirectX::XMVector3Normalize(XMLoadFloat3(&crossProduct)));

	// 카메라 위치 세팅, 나중에 또 업데이트 필요할듯 (주로 이동 후에)
	PlayerCameraPos = PlayerCharacter->MovementComp->Destination;	// 플레이어 도착예상지점 기준..
	PlayerCameraPos -= player->Forward * 30.0f;	// 플레이어 캐릭터가 향하는 방향에서 살짝 뒤로 뺌
	PlayerCameraPos.y += 30.0f;					// 위로 좀 올려줌
	PlayerCameraPos -= Right * 15.0f;			// 오른쪽으로 좀 빼주고
}

// 노티파이 체크해서 플레이어랑 적 캐릭터랑 애니메이션 합을 맞추고 이펙트 터트림(위치필요해서), 사운드는 노티파이 안에서 터트림
void BattleScene::NotifierCheck()
{
	// 애니메이션 세팅에 필요한거
	auto PAnime = PlayerCharacter->GetComponent<AnimationComponent>();		//멀티가면좀바꿔야
	auto EAnime = TargetEnemy->chara->GetComponent<AnimationComponent>();

	// 플레이어의 행동에서 발생한 노티파이를 기준으로 실행, 플레이어 턴에서만 작동할듯
	// for(auto player : PlayerList) // 이건 멀티용,,,, 멀티까지 내일안에 해결할 수 있을까?
	for (auto noti : player->chara->GetComponent<AnimationComponent>()->CurrentClip->NotifierList) 
	{
		if (noti->IsOn) 
		{
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

			noti->Disable = true;
			UpdateEnemyState();
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
					if (player->hp > 0) { PAnime->SetClipByName(L"Hit"); }
					PlayEffect(&TheWorld, L"Hit3", { enemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {2.0f, 2.0f, 2.0f} }, { false, 0.5f, 0.2f, 1.0f });
					if (EnemyDamage > 0) { DamageAnimation(EnemyDamage, false); EnemyDamage = 0; }
				}
				else if (noti->Lable == L"E1A2") // 적1 발차기
				{
					if (player->hp > 0) { PAnime->SetClipByName(L"Hit"); }
					PlayEffect(&TheWorld, L"Hit5", { player->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {3.0f, 3.0f, 3.0f} }, { false, 0.5f, 0.2f, 1.0f });
					if (EnemyDamage > 0) { DamageAnimation(EnemyDamage, false); EnemyDamage = 0; }
				}
				else if (noti->Lable == L"E1A3")	// 적1 비명
				{
					if (player->hp > 0) { PAnime->SetClipByName(L"Hit"); }
					PlayEffect(&TheWorld, L"Hit5", { enemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {10.0f, 10.0f, 10.0f} }, { false, 2.5f, 0.2f, 1.0f });
					if (EnemyDamage > 0) { DamageAnimation(EnemyDamage, false); EnemyDamage = 0; }
				}
				if (noti->Lable == L"E2A1")	// 적2 강긁기
				{
					if (player->hp > 0) { PAnime->SetClipByName(L"Hit"); }
					PlayEffect(&TheWorld, L"Hit1", { player->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {5.0f, 5.0f, 5.0f} }, { false, 0.5f, 0.2f, 1.0f });
					if (EnemyDamage > 0) { DamageAnimation(EnemyDamage, false); EnemyDamage = 0; }
				}
				if (noti->Lable == L"E2A2") // 적2 약긁기
				{
					if (player->hp > 0) { PAnime->SetClipByName(L"Hit"); }
					PlayEffect(&TheWorld, L"Hit2", { player->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {3.0f, 3.0f, 3.0f} }, { false, 0.5f, 0.2f, 1.0f });
					if (EnemyDamage > 0) { DamageAnimation(EnemyDamage, false); EnemyDamage = 0; }
				}
				if (noti->Lable == L"E2A3") // 적2 중긁기
				{
					if (player->hp > 0) { PAnime->SetClipByName(L"Hit"); }
					PlayEffect(&TheWorld, L"Hit3", { enemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {2.0f, 2.0f, 2.0f} }, { false, 0.5f, 0.2f, 1.0f });
					if (EnemyDamage > 0) { DamageAnimation(EnemyDamage, false); EnemyDamage = 0; }
				}

				noti->Disable = true;
				UpdatePlayerState();
			}
		}
	}
}

void BattleScene::BattleProcess()
{
	if (TurnStart) { TurnStartProcess(); }
	if (TurnEndButton->m_bClicked) { TurnEndProcess(); }
	if (MoveButton->m_bClicked) { IsMoving = true; MoveButton->m_bClicked = false; MAIN_PICKER.optPickingMode = PMOD_LAND; }
	if (IsMoving) { MoveProcess(); }

	EnemyAnimProcess();
	CardCheck();
	ToolTipCheck();
	NotifierCheck();
	DeadCheck();
}

void BattleScene::TurnStartProcess()
{
	TurnNum++;
	player->armor = 0;
	player->cost = player->maxCost;
	UpdatePlayerState();

	for (auto enemy : EnemyList) { enemy->SetIntentObj(TurnNum, enemy->IntentIcon, enemy->Intent1, enemy->Intent2); }

	if (!player->isDead) 
	{
		int drawNum = 3;
		for (int i = 0; i < drawNum; i++) { CardList[i]->m_bIsDead = false; }
		Dick->Draw(drawNum);
		UpdateHand(drawNum);
	}

	TurnStart = false;
	TurnEndButton->m_bDisable = false;
	MoveButton->m_bIsDead = false;
}

void BattleScene::TurnEndProcess()
{
	SoundMap.find(L"TurnEnd")->second->Play();

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

	InActionEnemyNum = 0;
	InActionEnemy = EnemyList[InActionEnemyNum];
}

void BattleScene::EnemyAnimProcess()
{
	// 적 턴이고, 현재 행동중인 적이 아직 이동이나 공격을 하지 않았을 경우
	if (!TurnState && (!InActionEnemy->doMove || !InActionEnemy->doAction)) 
	{
		// 적이 아직 이동하지 않았을 경우
		if (!InActionEnemy->doMove) 
		{
			InActionEnemy->move(player, TurnNum); 

			// 턴 진행중인 적 쪽으로 플레이어 캐릭터 회전
			Vector3 NewPlayerForward = InActionEnemy->chara->Transform->Translation - player->chara->Transform->Translation;
			NewPlayerForward.Normalize();
			DirectX::SimpleMath::Quaternion quaternion = Quaternion::FromToRotation(player->Forward, NewPlayerForward);
			Vector3 eulerAngles = quaternion.ToEuler();
			eulerAngles.y = DirectX::XMConvertToDegrees(eulerAngles.y);
			//player->chara->Transform->Rotation += eulerAngles;
			auto timeline = PlayerCharacter->GetComponent<TimelineComponent>();
			if (timeline != nullptr)
			{
				timeline->CreateTimeline(player->chara->Transform->Rotation.y, player->chara->Transform->Rotation.y + eulerAngles.y, 0.2f);
				timeline->PlayFromStart();
			}

			player->Forward = NewPlayerForward;

			// 카메라 워크
			auto CameraTarget = InActionEnemy->chara->Transform->Translation;
			CameraTarget.y += 10.0f;
			UpdateCameraPos();

			MoveCamera->CreateViewMatrix(PlayerCameraPos, CameraTarget, Vector3(0.0f, 1.0, 0.0f));
			MainCameraSystem->TargetCamera = MoveCamera;

			// UI 숨겨줌, 골라서 숨기는 기능 없으니까 걍 다 숨겨 몰랑 -> 굳이같다;
			//WRS->hide = true;
		}

		// 이동이 끝나고, 적이 아직 행동하지 않았을 경우
		else if (InActionEnemy->doMove && !InActionEnemy->chara->MovementComp->IsMoving && !InActionEnemy->doAction)
		{
			EnemyDamage = InActionEnemy->patern(player, TurnNum);
			UpdateEnemyState();

			// 카메라 워크
			Vector3 t1 = player->chara->GetComponent<BoundingBoxComponent>()->OBB.Center;
			Vector3 t2 = InActionEnemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center;
			float dist = t1.Distance(t1, t2);
			TestActionCamera->Lerp(t1, t2, 50.0f, 0.1f);

			MoveCamera->CreateViewMatrix(TestActionCamera->MovementComp->Destination, TestActionCamera->ActionCameraComp->NextLook, Vector3(0.0f, 1.0, 0.0f));
			MainCameraSystem->TargetCamera = MoveCamera;

			// UI 다시 살려줌-> 굳이같다;
			//WRS->hide = false;
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
void BattleScene::MoveProcess()
{
	// 이동버튼 눌리고 최초 실행시에만, 카메라 바꿔줌
	if (MoveTimer == 0.0f) 
	{
		Vector3 SkyCamPos = PlayerCameraPos;
		SkyCamPos -= player->Forward * 200.0f;
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
			Vector3 Forward = player->chara->Transform->Translation - MAIN_PICKER.vIntersection;
			float CanMoveRange = 100.0f; // 수치 설정
			float Distance = Forward.Length();
			if (Distance < CanMoveRange) 
			{
				MAIN_PICKER.SetMoveUIColor(&TheWorld, { 0.0f, 0.0f, 0.0f, 0.5f }, { 0.5f, 1.0f, 0.0f, 0.5f });

				// 땅을 찍었다면, 이동명령 내리고 카메라 바꿔줌
				KeyState btnLC = Input::GetInstance()->getKey(VK_LBUTTON);
				if (btnLC == KeyState::Down)
				{
					// 이쪽은 메인카메라로 확 바꾸는게?
					//MainCamera->CreateViewMatrix(player->chara->GetComponent<Camera>()->Pos, player->chara->GetComponent<Camera>()->Target, Vector3(0.0f, 1.0, 0.0f));
					//MainCameraSystem->MainCamera = player->chara->GetComponent<Camera>();
					//MainCameraSystem->TargetCamera = player->chara->GetComponent<Camera>();
					PlayerCharacter->MoveTo(MAIN_PICKER.vIntersection);
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
			player->Forward = player->chara->MovementComp->Forward;
			Vector3 NewPlayerForward = TargetEnemy->chara->Transform->Translation - player->chara->Transform->Translation;
			NewPlayerForward.Normalize();
			DirectX::SimpleMath::Quaternion quaternion = Quaternion::FromToRotation(player->Forward, NewPlayerForward);
			Vector3 eulerAngles = quaternion.ToEuler();
			eulerAngles.y = DirectX::XMConvertToDegrees(eulerAngles.y);
			//player->chara->Transform->Rotation += eulerAngles;
			auto timeline = PlayerCharacter->GetComponent<TimelineComponent>();
			if (timeline != nullptr)
			{
				timeline->CreateTimeline(player->chara->Transform->Rotation.y, player->chara->Transform->Rotation.y + eulerAngles.y, 0.2f);
				//timeline->SetTarget(&player->chara->Transform->Rotation.y); // 컴포넌트 추가할 때 미리 해두기?
				timeline->PlayFromStart();
			}

			player->Forward = NewPlayerForward;

			// 적 캐릭터 방향 회전 (플레이어 캐릭터 쪽으로)
			for (auto enemy : EnemyList)
			{
				Vector3 CurrentForward = enemy->chara->MovementComp->Forward;
				Vector3 NewForward = player->chara->Transform->Translation - enemy->chara->Transform->Translation;
				NewForward.Normalize();
				DirectX::SimpleMath::Quaternion quaternion = Quaternion::FromToRotation(CurrentForward, NewForward);
				Vector3 eulerAngles = quaternion.ToEuler();
				eulerAngles.y = DirectX::XMConvertToDegrees(eulerAngles.y);
				//enemy->chara->Transform->Rotation += eulerAngles;
				auto timeline = enemy->chara->GetComponent<TimelineComponent>();
				if (timeline != nullptr)
				{
					timeline->CreateTimeline(enemy->chara->Transform->Rotation.y, enemy->chara->Transform->Rotation.y + eulerAngles.y, 0.2f);
					//timeline->SetTarget(&player->chara->Transform->Rotation.y); // 컴포넌트 추가할 때 미리 해두기?
					timeline->PlayFromStart();
				}
				enemy->chara->MovementComp->Forward = NewForward;
			}

			// 카메라 셋팅
			auto CameraTarget = TargetEnemy->chara->Transform->Translation;
			CameraTarget.y += 10.0f;
			UpdateCameraPos();
			MainCameraSystem->MainCamera = MainCamera;
			MoveCamera->CreateViewMatrix(PlayerCameraPos, CameraTarget, Vector3(0.0f, 1.0, 0.0f));
			MainCameraSystem->TargetCamera = MoveCamera;

			// 턴당 이동은 한번으로 제한, 코스트 먹을지 말지는 뭐 모르겠다
			MoveButton->m_bIsDead = true;
		}
	}
}

void BattleScene::CardCheck()
{
	if (TargetEnemy == nullptr || TargetEnemy->hp <= 0) { return; } // 타겟이 없거나 죽었다면 실행ㄴ

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
					TargetEnemy->hp -= 6;
					PlayerDamage = 6;
					CanUse = true;

					PAnime->SetClipByName(L"Shooting");
				}
			}break;

			case Defend:
			{
				if (ManaCheck(1))
				{
					player->armor += 5;
					CanUse = true;

					PAnime->SetClipByName(L"Inward_Block");
					PlayEffect(&TheWorld, L"Shield1", { player->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {3.0f, 3.0f, 3.0f} }, { false, 1.0f, 0.2f, 1.0f });
					SoundMap.find(L"Shield")->second->Play();
				}
			}break;

			case PommelStrike:
			{
				if (ManaCheck(1))
				{
					TargetEnemy->hp -= 9;
					PlayerDamage = 9;
					Dick->Draw(1);
					DrawedCard = 1;
					CanUse = true;

					PAnime->SetClipByName(L"Gunplay");
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

					PAnime->SetClipByName(L"Inward_Block");
					PlayEffect(&TheWorld, L"Shield1", { player->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {3.0f, 3.0f, 3.0f} }, { false, 1.0f, 0.2f, 1.0f });
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
					player->armor += 5;
					CanUse = true;

					PAnime->SetClipByName(L"Pistol_Whip");
				}
			}break;

			}

			if (CanUse)
			{
				Dick->Use(cardNum);
				UpdateHand(Dick->HandList.size(), cardNum, DrawedCard);
				UpdatePlayerState();
				// UpdateEnemyState();		// 여기서 빼고 노티파이에서 처리해도 무방한가?
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
				Vector3 t1 = player->chara->GetComponent<BoundingBoxComponent>()->OBB.Center;
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

// 데미지 애니메이션, 
void BattleScene::DamageAnimation(int damage, bool hitEnemy)
{
	int fontWidth = 140.0f;	// 한 글자 너비

	// 아군 캐릭터가 적 공격, 뭐 큰차인 없고 걍 데미지 날라가는 방향
	if (hitEnemy) 
	{
		// 데미지가 10 이상일 경우, 두자리수 ㅇㅇ
		if (damage/10 > 0) 
		{
			Damage1->m_bIsDead = false;
			Damage1->m_pCutInfoList[0]->tc = NumberTextureList_Damage[damage/10];
			Vector2 AnimPos[2] = { {1000.0f, 450.0f},{1400.0f, 250.0f} };
			Damage1->SetAnimation( AnimPos, 1.2f, true);
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
	for (auto enemy : AllEnemyList) { enemy->UpdateState(); }
}

void BattleScene::DeadCheck()
{
	if (player->hp <= 0 && !PlayerCharacter->GetComponent<AnimationComponent>()->IsInAction())
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
			else if (player->chara->GetComponent<AnimationComponent>()->IsInAction()) { return; }	// 플레이어가 행동중이어도 안끝나게, 그,, 노티파이맞추면서암튼이래야함
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
