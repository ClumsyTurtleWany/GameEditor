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
//�߰�
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

//�߰�
#include "ColliderSystem.h"

///////////////////
//Effect Include
///////////////////
#include "EffectInclude/EffectSystem.h"


bool BattleScene::Init()
{
	MainSaveLoadSystem = new SaveLoadSystem;
	TheWorld.AddSystem(MainSaveLoadSystem);

	ID = BATTLE;

	// ���� ī�޶�
	MainCameraSystem = new CameraSystem;
	MainCameraActor = new Actor;
	MainCamera = MainCameraActor->AddComponent<Camera>();
	MainCamera->CreateViewMatrix(Vector3(0.0f, 300.0f, -600.0f), Vector3(150.0f, 20.0f, 50.0f), Vector3(0.0f, 1.0, 0.0f));
	MainCamera->CreateProjectionMatrix(1.0f, 10000.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));
	MainCameraSystem->MainCamera = MainCamera;
	MainCameraActor->AddComponent<OscillationComponent>();
	TheWorld.AddEntity(MainCameraActor);

	// Ÿ�� ī�޶�� ���� ���� ��
	MoveCamera = new Camera;
	MoveCamera->CreateProjectionMatrix(1.0f, 10000.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));

	// �׼� ī�޶�, ������ �׳� lerp�Լ��� �Ÿ����ϱ��
	TestActionCamera = new ActionCamera;
	TestActionCamera->Create(Vector3(0.0f, 50.0f, -70.0f), Vector3(0.0f, 20.0f, 50.0f), 1.0f, 10000.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));
	TheWorld.AddEntity(TestActionCamera);

	// ��� �÷��̾�� Ÿ��Ʋ���� �� �ʱ�ȭ�ϰ� ���°͵�..
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

	// ���״��� �׽�Ʈ��
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
	


	// ī�޶� �ý��� �� ������ �ý��� �߰�.
	TheWorld.AddSystem(MainCameraSystem);
	TheWorld.AddSystem(new ColliderSystem);
	TheWorld.AddSystem(new MovementSystem);
	TheWorld.AddSystem(new UpdateAnimSystem);
	// SelectAnimSystem �߰�
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



	//////  �ʱ� ī�޶� ����  //////
	
	// �÷��̾� ĳ���� ������ ���� ����
	auto PlayerForward = PlayerCharacter->MovementComp->Destination - PlayerCharacter->Transform->Translation;
	PlayerForward.Normalize();
	player->Forward = PlayerForward;

	// Right ���� ���Ҷ��
	DirectX::XMFLOAT3 upVec(0.0f, 1.0f, 0.0f);
	Vector3 Right;	// Calculate the right vector	
	DirectX::XMFLOAT3 crossProduct;
	XMStoreFloat3(&crossProduct, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&player->Forward), XMLoadFloat3(&upVec)));
	DirectX::XMStoreFloat3(&Right, DirectX::XMVector3Normalize(XMLoadFloat3(&crossProduct)));

	// ī�޶� ��ġ ����, ���߿� �� ������Ʈ �ʿ��ҵ� (�ַ� �̵� �Ŀ�)
	PlayerCameraPos = PlayerCharacter->MovementComp->Destination;	// �÷��̾� ������������ ����..
	PlayerCameraPos -= player->Forward * 1.0f;	// �÷��̾� ĳ���Ͱ� ���ϴ� ���⿡�� ��¦ �ڷ� ��
	PlayerCameraPos.y += 30.0f;					// ���� �� �÷���
	PlayerCameraPos -= Right * 35.0f;			// ���������� �� ���ְ�

	// ī�޶� Ÿ�� ����
	auto CameraStartTarget = (enemy1->chara->MovementComp->Destination + enemy2->chara->MovementComp->Destination) / 2;

	// ī�޶� �� ��Ʈ���� �缳���ϰ� Ÿ�� ī�޶�� �Ӹ�
	MoveCamera->CreateViewMatrix(PlayerCameraPos, CameraStartTarget, Vector3(0.0f, 1.0, 0.0f));
	MainCameraSystem->TargetCamera = MoveCamera;

	TargetEnemy = EnemyList[0];

	return true;
}

bool BattleScene::Frame()
{
	BaseScene::Frame();

	// �� ����â ������Ʈ
	if(TurnState) // �� �Ͽ��� �׳� �� ����â �Ⱥ��̰Բ�
	{
		for (auto enemy : EnemyList)
		{
			if (enemy == TargetEnemy) 
			{ 
				for (auto obj : enemy->ObjList) { obj->m_bIsDead = false; } 

				// ȭ��ǥ�� �عٴڿ� ����Ʈ ����
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

	// AŰ ������ Ÿ�ٺ���
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
			// ���� Ÿ������ ���� ������ ���̶��, ù ������ ����
			if (enemyNum + 1 == EnemyList.size()) { TargetEnemy = EnemyList[0]; }
			else { TargetEnemy = EnemyList[enemyNum + 1]; }

			/*auto PlayerForward = PlayerCharacter->MovementComp->Destination - PlayerCharacter->Transform->Translation; 
			PlayerForward.Normalize();
			player->Forward = PlayerForward;*/

			// ĳ���� ���� ȸ��, �ִϸ��̼� �ٶ�� �ټ��ִµ� ���������,,��,,
			Vector3 NewPlayerForward = TargetEnemy->chara->Transform->Translation - player->chara->Transform->Translation;
			NewPlayerForward.Normalize();
			DirectX::SimpleMath::Quaternion quaternion = Quaternion::FromToRotation(player->Forward, NewPlayerForward);
			Vector3 eulerAngles = quaternion.ToEuler();

			Vector3 cross = DirectX::XMVector3Cross(player->Forward, NewPlayerForward);
			Vector3 dot = DirectX::XMVector3Dot(cross, Vector3(0,1,0));

			if (dot.y < 0.f)
			{
				// �¿� �Ǻ�
			}
			
			eulerAngles.y = DirectX::XMConvertToDegrees(eulerAngles.y);
			//player->chara->Transform->Rotation += eulerAngles;
			auto timeline = PlayerCharacter->GetComponent<TimelineComponent>();
			if (timeline != nullptr)
			{
				timeline->CreateTimeline(player->chara->Transform->Rotation.y, player->chara->Transform->Rotation.y+eulerAngles.y,0.2f);
				//timeline->SetTarget(&player->chara->Transform->Rotation.y); // ������Ʈ �߰��� �� �̸� �صα�?
				timeline->PlayFromStart();
			}
			
			player->Forward = NewPlayerForward;

			// ī�޶� Ÿ�� ����
			auto CameraTarget = TargetEnemy->chara->Transform->Translation;
			CameraTarget.y += 10.0f;
			UpdateCameraPos();

			MoveCamera->CreateViewMatrix(PlayerCameraPos, CameraTarget, Vector3(0.0f, 1.0, 0.0f));
			MainCameraSystem->TargetCamera = MoveCamera;
			
			// ���� ���� ���
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
		float MinGamDoe = 0.3f; // ���߿� ���콺 �ΰ��� �ʿ��ϸ�?
		//MainCameraSystem->MainCamera->Pitch += MinGamDoe * Input::GetInstance()->m_ptOffset.x;
	}

	// ī�޶� ��ȯ ���ÿ�. �ʿ信 ���� ���� �ٶ�.
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

	// ��� �ϴ� �ֱ������� ������Ʈ �������.
	MainRenderSystem->MainCamera = MainCameraSystem->MainCamera;

	//Effect test
	if (Input::GetInstance()->getKey('U') == KeyState::Up)
	{
		PlayEffect(&TheWorld, L"Hit5", { { 20.0f, 20.0f, 0.0f }, Vector3(), {10.0f, 10.0f, 10.0f} }, { false, 1.0f, 1.0f, 1.0f });
	}

	//PickedCharacter = (Character*)MAIN_PICKER.curSelect.pTarget;
	//for (auto enemy : EnemyList)
	//{
	//	if (PickedCharacter == enemy->chara)
	//	{
	//		for (auto obj : enemy->ObjList) // ������ ���߿� ������ ���� ���� �����������.. ��
	//		{
	//			obj->m_bIsDead = false;
	//		}	

	//		if (TargetEnemy != enemy) // Ÿ������ �ٲ���� �� �ѹ���
	//		{
	//			TargetEnemy = enemy;

	//			// ĳ���� ���� ȸ��, �ִϸ��̼� �ٶ�� �ټ��ִµ� ���������,,��,,
	//			Vector3 NewPlayerForward = TargetEnemy->chara->Transform->Translation - player->chara->Transform->Translation;
	//			NewPlayerForward.Normalize();
	//			DirectX::SimpleMath::Quaternion quaternion = Quaternion::FromToRotation(player->Forward, NewPlayerForward);
	//			Vector3 eulerAngles = quaternion.ToEuler();
	//			eulerAngles.y = DirectX::XMConvertToDegrees(eulerAngles.y);
	//			player->chara->Transform->Rotation += eulerAngles;
	//			player->Forward = NewPlayerForward;

	//			// ī�޶� Ÿ�� ����
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

	// ���� ī�� Ȯ�� or ���� ī�� Ȯ�� ��ư Ŭ���� �� ��ȯ (ī�� ���� ������)
	if (RemainCardButton->m_bClicked) { SS = REMAINVIEW; RemainCardButton->m_bClicked = false; }
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

	//TurnEndButton->SetOrginStandard(false); // �׽�Ʈ��
	//RemainCardButton->SetOrginStandard(false);
	//DiscardButton->SetOrginStandard(false);

	// ī�� ������Ʈ ���� �� ����Ʈ�� �־���
	CardList[0] = bc->FindObj(L"Card1");
	CardList[1] = bc->FindObj(L"Card2");
	CardList[2] = bc->FindObj(L"Card3");
	CardList[0]->m_bDraggable = true;
	CardList[1]->m_bDraggable = true;
	CardList[2]->m_bDraggable = true;
	CardList[0]->m_OriginalOriginPos = CardList[0]->m_OriginPos;
	CardList[0]->m_OriginalOriginPos = CardList[0]->m_OriginPos;
	CardList[0]->m_OriginalOriginPos = CardList[0]->m_OriginPos;
	CardList[0]->m_bIsDead = true;		// ��ο� ������ �Ⱥ��̰�
	CardList[1]->m_bIsDead = true;
	CardList[2]->m_bIsDead = true;

	// �÷��̾� �Žñ� ���� ǥ�ÿ� 
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

	// ���α׷��� �ٸ� ���ؼ� �»�� �������� �ٲ���
	PlayerHPBar->SetOrginStandard(false);
	PlayerHPBar->m_OriginWH = PlayerHPBar->m_pCutInfoList[0]->pxWH;
	PlayerHPBar->m_OriginUV = PlayerHPBar->m_pCutInfoList[0]->uv;
	UpdateEnemyState();

	// ������, �ϴ� ������ ���ڸ���..
	Damage1 = bc->FindObj(L"Damage_1");
	Damage2 = bc->FindObj(L"Damage_2");
	Damage1->m_bIsDead = true;
	Damage2->m_bIsDead = true;

	// ���� ����&�ؽ�Ʈ, ���� ũ�⸦ �ٲٴ°� ����?�����ѵ� ��ư �ؽ�Ʈ�� ���� ���Ƴ����� �ҵ�
	ToolTipBoard = bc->FindObj(L"ToolTipBoard");
	ToolTipText	 = bc->FindObj(L"ToolTipText");
	ToolTipBoard->m_fAlpha = 0.7f;
	ToolTipText->m_fAlpha = 0.5f;
	ToolTipBoard->SetOrginStandard(false);
	ToolTipText->SetOrginStandard(false);
	ToolTipBoard->m_bIsDead = true;
	ToolTipText->m_bIsDead = true;


	// ���� ���忡 ���� �߰�.
	TheWorld.AddEntity(UI);

	// ī�� ��ġ���� ����, �ϴ� 1�� 2�� 3���϶���? ���߿� �ٲ�������� �𸣴� �� ���ϰ� �����صα�
	float CSS = 400.0f; // Card Space Start
	float CSW = 900.0f; // Card Space Width
	float CH = 700.0f;	// Card Height
	Vector2 CFS = { 0.0f, 700.0f };	// Card Flying Start, ī�� ������� ������ 

	CardPosList[0].push_back({ CSS + CSW * 1 / 2, CH });	// 1���϶� ��ġ
	CardPosList[0].push_back({ CSS + CSW * 1 / 3, CH });	// 2���϶�
	CardPosList[0].push_back({ CSS + CSW * 1 / 4, CH });	// 3���϶�
	CardPosList[1].push_back(CFS);
	CardPosList[1].push_back({ CSS + CSW * 2 / 3, CH });
	CardPosList[1].push_back({ CSS + CSW * 2 / 4, CH });
	CardPosList[2].push_back(CFS);
	CardPosList[2].push_back(CFS);
	CardPosList[2].push_back({ CSS + CSW * 3 / 4, CH });
}

void BattleScene::Init_Map()
{
	//// ���� ���� �߰�.
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

	// Sky Dome �߰�.
	Actor* skyDomeActor = new Actor;
	auto skyDomeComp = skyDomeActor->AddComponent<SkyDomeComponent>();
	skyDomeComp->Scale = Vector3(5000.0f, 5000.0f, 5000.0f);

	Material* skyDomeMaterial = MaterialManager::GetInstance()->CreateMaterial(L"SkyDome");
	skyDomeMaterial->DiffuseTextureName = L"../resource/Texture/SkyDome.png";
	skyDomeMaterial->Create();
	skyDomeComp->SetMaterial(skyDomeMaterial);

	TheWorld.AddEntity(skyDomeActor);

	// Cloud Dome �߰�
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

	// ClipList�� ����Ǹ� SetClipByName(name) �Լ��� ���氡�� <- name = Ȯ���ڸ� ������ ���ϸ�
	auto playerCharAnimComp = PlayerCharacter->AddComponent<AnimationComponent>();

	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Run.clp", playerCharAnimComp);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Idle.clp", playerCharAnimComp);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Hit.clp", playerCharAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Dying.clp", playerCharAnimComp, false);
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Shooting.clp", playerCharAnimComp, false);	// �Ϲݻ��
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Gunplay.clp", playerCharAnimComp, false);	// ������
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Pistol_Whip.clp", playerCharAnimComp, false); // ������
	FBXLoader::GetInstance()->LoadAnimClip("../resource/FBX/Adam/Adam_anim/Inward_Block.clp", playerCharAnimComp, false); // ����
	playerCharAnimComp->SetClipByName(L"Idle");

	///////////////// NOTIFIER ��� ����/////////////////
	//// 1. ��Ƽ���̾� �Ŵ��� ������Ʈ�� �߰��ϰ�
	//auto notiMgr = PlayerCharacter->AddComponent<NotifierMgrComponent>();

	//// 2. ��Ƽ���̾� ���� (������ ���� �����ϸ� �� ��Ƽ���̾�� ����, ����Ʈ ���ÿ� �߻���ų �� ����)
	////		 ������ ���ڴ� ��Ƽ���̾� �̸� , �߻���Ű���� �ϴ� ������(����) , ��Ƽ���̾� ����(������, ����) <- �ƹ����� �ȳ����� 1�� �⺻
	//Notifier* newNoti = notiMgr->CreateNotifier(L"step1", 7);
	//// 3. ����Ʈ �߻��� ���� ��Ƽ���̾ ����Ʈ�� �޾��ֱ�
	////		 ����, ����Ʈ ���� �ΰ��� ������� �� �� ����
	//// 
	////		 MakeFmodSound(��Ƽ���̾� �̸� , FmodSound*) <- ���� ������� Ȱ���� �� ������ second�� �ٷ� �����ϱ⶧���� �ε� �ȵ� �� �߸� ������ ������ ����
	//notiMgr->MakeFmodSound(L"step1", SoundMap.find(L"Hit2")->second);

	////		 MakeEffect(��Ƽ���̾� �̸�, ����Ʈ Ű �̸� , ����Ʈ ��ġ(Ʈ������ ������Ʈ) , ��ƼŬ �Ӽ�)
	////		 �̶� ����Ʈ ��ġ�� Vector3 ���� �־ �� (�����ε�)
	//notiMgr->MakeEffect(L"step1", L"Hit5", { {0.0f, 10.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {5.0f, 5.0f, 5.0f} }, { false, 0.5f, 0.2f, 1.0f });

	//Notifier* newNoti2 = notiMgr->CreateNotifier(L"step2", 20);

	////		 MakeSound(��Ƽ���̾� �̸� , ������ �ε��ߴ� ���� ����(FmodSoundManager�� ���� �ʿ��� �޾ƿ�) , ����, ���� ����)
	//notiMgr->MakeSound(L"step2", L"Attack_Bludgeon.ogg", 0.5f, false);

	//// 4. ������� ��Ƽ���̾ �ִϸ��̼� ������Ʈ�� �ٿ��ֱ�
	////		 AddNotifier(�ִϸ��̼� ������Ʈ , �ش� ������Ʈ�� ��� �ִ� �ִϸ��̼� Ŭ���� �̸� , ������ ��Ƽ���̾�)
	//notiMgr->AddNotifier(*playerCharAnimComp, L"Run", newNoti);
	//notiMgr->AddNotifier(*playerCharAnimComp, L"Run", newNoti2);
	
	// ��Ƽ���� ����
	int soundFrame = 0;
	int delayFrame = 0;
	auto notiMgr = PlayerCharacter->AddComponent<NotifierMgrComponent>();
	delayFrame = 5;
	// �Ϲݻ��
	soundFrame = 6;
	Notifier* Fire_ready = notiMgr->CreateNotifier(L"Fire_ready", soundFrame - delayFrame);
	Notifier* Fire = notiMgr->CreateNotifier(L"Fire", soundFrame);
	notiMgr->MakeSound(L"Fire", L"Fire1.mp3", 1.0f, false);
	notiMgr->AddNotifier(*playerCharAnimComp, L"Shooting", Fire_ready);
	notiMgr->AddNotifier(*playerCharAnimComp, L"Shooting", Fire);
	//������
	soundFrame = 20;
	Notifier* Fire2_ready = notiMgr->CreateNotifier(L"Fire2_ready", soundFrame - delayFrame);
	Notifier* Fire2 = notiMgr->CreateNotifier(L"Fire2", soundFrame);
	notiMgr->MakeSound(L"Fire2", L"Fire2.mp3", 1.0f, false);
	notiMgr->AddNotifier(*playerCharAnimComp, L"Gunplay", Fire2_ready);
	notiMgr->AddNotifier(*playerCharAnimComp, L"Gunplay", Fire2);
	// ������ �ĸ���
	soundFrame = 17;
	Notifier* Strike_ready = notiMgr->CreateNotifier(L"Strike_ready", soundFrame - delayFrame);
	Notifier* Strike = notiMgr->CreateNotifier(L"Strike", soundFrame);
	notiMgr->MakeSound(L"Strike", L"Attack_Strike.ogg", 1.0f, false);
	notiMgr->AddNotifier(*playerCharAnimComp, L"Pistol_Whip", Strike_ready);
	notiMgr->AddNotifier(*playerCharAnimComp, L"Pistol_Whip", Strike);


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
	// �÷��̾�� ī�޶� �� ī�޶� �� ����.
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
	/////////////////////////////////////////// �� ĳ���� 1  //////////////////////////////////////////////////////
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

	// ��Ƽ���� ����
	notiMgr = EnemyCharacter->AddComponent<NotifierMgrComponent>();
	delayFrame = 5;
	// �÷��̾� ĳ���� 1�� �̸� �´� �ִ� ��� ��������? ��Ƽ���� ���ɵ��� ����
	// ��ġ
	soundFrame = 40;
	Notifier* E1A1 = notiMgr->CreateNotifier(L"E1A1", soundFrame);		// Enemy1 Attack1
	notiMgr->MakeSound(L"E1A1", L"E1A1.ogg", 1.0f, false);	
	notiMgr->AddNotifier(*enemyCharAnimComp, L"Attack1", E1A1);
	// �P��
	soundFrame = 25;
	Notifier* E1A2 = notiMgr->CreateNotifier(L"E1A2", soundFrame);
	notiMgr->MakeSound(L"E1A2", L"Attack_Bludgeon.ogg", 1.0f, false);	
	notiMgr->AddNotifier(*enemyCharAnimComp, L"Attack2", E1A2);
	// ���ٸ�(���)
	soundFrame = 60;
	Notifier* E1A3 = notiMgr->CreateNotifier(L"E1A3", soundFrame);
	notiMgr->MakeSound(L"E1A3", L"E1A3.mp3", 1.0f, false);	
	notiMgr->AddNotifier(*enemyCharAnimComp, L"Attack3", E1A3);
	// ���, �Ҹ����̷���
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
	//E1MC->Speed = 40.0f;
	E1MC->Speed = 30.0f;
	EnemyCharacter->MoveTo(Vector3(20.0f, 0.0f, 0.0f));

	//Picking Info Test
	enemyCharMeshComp->Name = "Enemy";

	/////////////// Bounding Box Add ////////////
	auto enemyOBBComp = EnemyCharacter->AddComponent<BoundingBoxComponent>(Vector3(0.5f, 1.0f, 0.5f), Vector3(0.0f, 1.0f, 0.0f));

	// �� ĳ���� �� ī�޶� �� ī�޶� ��
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
	/////////////////////////// �� ĳ���� 2 //////////////////////////////////////////////////////////////////
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

	// ��Ƽ���� ����
	notiMgr = EnemyCharacter2->AddComponent<NotifierMgrComponent>();
	// ��Աܱ�
	soundFrame = 39;
	Notifier* E2A1 = notiMgr->CreateNotifier(L"E2A1", soundFrame);		// Enemy2 Attack1
	notiMgr->MakeSound(L"E2A1", L"E2A1.wav", 1.0f, false);
	notiMgr->AddNotifier(*Enemy2_CharAnimComp, L"Attack1", E2A1);
	// ��¦�ܱ�
	soundFrame = 31;
	Notifier* E2A2 = notiMgr->CreateNotifier(L"E2A2", soundFrame);
	notiMgr->MakeSound(L"E2A2", L"E2A2.mp3", 1.0f, false);	
	notiMgr->AddNotifier(*Enemy2_CharAnimComp, L"Attack2", E2A2);
	// �߰��ܱ�
	soundFrame = 31;
	Notifier* E2A3 = notiMgr->CreateNotifier(L"E2A3", soundFrame);
	notiMgr->MakeSound(L"E2A3", L"E2A3.mp3", 1.0f, false);	 
	notiMgr->AddNotifier(*Enemy2_CharAnimComp, L"Attack3", E2A3);
	// ���, �Ҹ����̷���
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
	//Enemy2_CharMovementComp->Speed = 50.0f;
	Enemy2_CharMovementComp->Speed = 30.0f;
	EnemyCharacter2->MoveTo(Vector3(20.0f, 0.0f, 70.0f));

	/////////////// Bounding Box Add ////////////
	auto player_BOBBComp = EnemyCharacter2->AddComponent<BoundingBoxComponent>(Vector3(0.5f, 0.9f, 0.5f), Vector3(0.0f, 0.9f, 0.0f));

	// �� ĳ���� �� ī�޶� �� ī�޶� ��
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

// ī�޶� ��ġ ����, �� �Լ��� �÷��̾� ĳ���� �������� ī�޶� ��ġ�� ������
void BattleScene::UpdateCameraPos()
{
	// Right ���� ���Ҷ��
	DirectX::XMFLOAT3 upVec(0.0f, 1.0f, 0.0f);
	Vector3 Right;	// Calculate the right vector	
	DirectX::XMFLOAT3 crossProduct;
	XMStoreFloat3(&crossProduct, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&player->Forward), XMLoadFloat3(&upVec)));
	DirectX::XMStoreFloat3(&Right, DirectX::XMVector3Normalize(XMLoadFloat3(&crossProduct)));

	// ī�޶� ��ġ ����, ���߿� �� ������Ʈ �ʿ��ҵ� (�ַ� �̵� �Ŀ�)
	PlayerCameraPos = PlayerCharacter->MovementComp->Destination;	// �÷��̾� ������������ ����..
	PlayerCameraPos -= player->Forward * 30.0f;	// �÷��̾� ĳ���Ͱ� ���ϴ� ���⿡�� ��¦ �ڷ� ��
	PlayerCameraPos.y += 30.0f;					// ���� �� �÷���
	PlayerCameraPos -= Right * 15.0f;			// ���������� �� ���ְ�
}

// ��Ƽ���� üũ�ؼ� �÷��̾�� �� ĳ���Ͷ� �ִϸ��̼� ���� ���߰� ����Ʈ ��Ʈ��(��ġ�ʿ��ؼ�), ����� ��Ƽ���� �ȿ��� ��Ʈ��
void BattleScene::NotifierCheck()
{
	// �ִϸ��̼� ���ÿ� �ʿ��Ѱ�
	auto PAnime = PlayerCharacter->GetComponent<AnimationComponent>();		//��Ƽ�������ٲ��
	auto EAnime = TargetEnemy->chara->GetComponent<AnimationComponent>();

	// �÷��̾��� �ൿ���� �߻��� ��Ƽ���̸� �������� ����, �÷��̾� �Ͽ����� �۵��ҵ�
	// for(auto player : PlayerList) // �̰� ��Ƽ��,,,, ��Ƽ���� ���Ͼȿ� �ذ��� �� ������?
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

			UpdateEnemyState();
		}
	}

	// �� �ൿ���� �߻��� ��Ƽ���̸� �������� ����, �굵 �Ƹ� ���Ͽ��� ����
	for (auto enemy : AllEnemyList)
	{
		for (auto noti : enemy->chara->GetComponent<AnimationComponent>()->CurrentClip->NotifierList)
		{
			if (noti->IsOn)
			{
				if (noti->Lable == L"E1A1") // ��1 �ĸ���
				{
					if (player->hp > 0) { PAnime->SetClipByName(L"Hit"); }
					PlayEffect(&TheWorld, L"Hit3", { enemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {5.0f, 5.0f, 5.0f} }, { false, 0.5f, 0.2f, 1.0f });
					if (EnemyDamage > 0) { DamageAnimation(EnemyDamage, false); EnemyDamage = 0; }
				}
				else if (noti->Lable == L"E1A2") // ��1 ������
				{
					if (player->hp > 0) { PAnime->SetClipByName(L"Hit"); }
					PlayEffect(&TheWorld, L"Hit5", { player->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {3.0f, 3.0f, 3.0f} }, { false, 0.5f, 0.2f, 1.0f });
					if (EnemyDamage > 0) { DamageAnimation(EnemyDamage, false); EnemyDamage = 0; }
				}
				else if (noti->Lable == L"E1A3")	// ��1 ���
				{
					if (player->hp > 0) { PAnime->SetClipByName(L"Hit"); }
					PlayEffect(&TheWorld, L"Hit5", { enemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {10.0f, 10.0f, 10.0f} }, { false, 2.5f, 0.2f, 1.0f });
					if (EnemyDamage > 0) { DamageAnimation(EnemyDamage, false); EnemyDamage = 0; }
				}
				if (noti->Lable == L"E2A1")	// ��2 ���ܱ�
				{
					if (player->hp > 0) { PAnime->SetClipByName(L"Hit"); }
					PlayEffect(&TheWorld, L"Hit1", { player->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {5.0f, 5.0f, 5.0f} }, { false, 0.5f, 0.2f, 1.0f });
					if (EnemyDamage > 0) { DamageAnimation(EnemyDamage, false); EnemyDamage = 0; }
				}
				if (noti->Lable == L"E2A2") // ��2 ��ܱ�
				{
					if (player->hp > 0) { PAnime->SetClipByName(L"Hit"); }
					PlayEffect(&TheWorld, L"Hit2", { player->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {3.0f, 3.0f, 3.0f} }, { false, 0.5f, 0.2f, 1.0f });
					if (EnemyDamage > 0) { DamageAnimation(EnemyDamage, false); EnemyDamage = 0; }
				}
				if (noti->Lable == L"E2A3") // ��2 �ܱ߱�
				{
					if (player->hp > 0) { PAnime->SetClipByName(L"Hit"); }
					PlayEffect(&TheWorld, L"Hit3", { enemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {3.0f, 3.0f, 3.0f} }, { false, 0.5f, 0.2f, 1.0f });
					if (EnemyDamage > 0) { DamageAnimation(EnemyDamage, false); EnemyDamage = 0; }
				}

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
	// �� ���̰�, ���� �ൿ���� ���� ���� �̵��̳� ������ ���� �ʾ��� ���
	if (!TurnState && (!InActionEnemy->doMove || !InActionEnemy->doAction)) 
	{
		// ���� ���� �̵����� �ʾ��� ���
		if (!InActionEnemy->doMove) 
		{
			InActionEnemy->move(player, TurnNum); 

			// �� �������� �� ������ �÷��̾� ĳ���� ȸ��
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

			// ī�޶� ��ũ
			auto CameraTarget = InActionEnemy->chara->Transform->Translation;
			CameraTarget.y += 10.0f;
			UpdateCameraPos();

			MoveCamera->CreateViewMatrix(PlayerCameraPos, CameraTarget, Vector3(0.0f, 1.0, 0.0f));
			MainCameraSystem->TargetCamera = MoveCamera;

			// UI ������, ��� ����� ��� �����ϱ� �� �� ���� ���� -> ���̰���;
			//WRS->hide = true;
		}

		// �̵��� ������, ���� ���� �ൿ���� �ʾ��� ���
		else if (InActionEnemy->doMove && !InActionEnemy->chara->MovementComp->IsMoving && !InActionEnemy->doAction)
		{
			EnemyDamage = InActionEnemy->patern(player, TurnNum);
			UpdateEnemyState();

			// ī�޶� ��ũ
			Vector3 t1 = player->chara->GetComponent<BoundingBoxComponent>()->OBB.Center;
			Vector3 t2 = InActionEnemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center;
			float dist = t1.Distance(t1, t2);
			TestActionCamera->Lerp(t1, t2, 50.0f, 0.1f);

			MoveCamera->CreateViewMatrix(TestActionCamera->MovementComp->Destination, TestActionCamera->ActionCameraComp->NextLook, Vector3(0.0f, 1.0, 0.0f));
			MainCameraSystem->TargetCamera = MoveCamera;

			// UI �ٽ� �����-> ���̰���;
			//WRS->hide = false;
		}
	}

	// �� ���̰�, ���� �ൿ���̾��� ���� �̵�&�ൿ�� ������ ����
	else if (!TurnState && InActionEnemy->doMove && InActionEnemy->doAction && !InActionEnemy->chara->GetComponent<AnimationComponent>()->IsInAction())
	{
		// �ʱ�ȭ
		InActionEnemy->doMove = false;
		InActionEnemy->doAction = false;

		if (InActionEnemy == EnemyList[EnemyList.size() - 1]) // ���� �ൿ���� ���� ������ ���̶��  
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
	// �̵���ư ������ ���� ����ÿ���, ī�޶� �ٲ���
	if (MoveTimer == 0.0f) 
	{
		Vector3 SkyCamPos = PlayerCameraPos;
		SkyCamPos -= player->Forward * 200.0f;
		SkyCamPos.y += 150.0f;
		Vector3 CameraTarget;
		MoveCamera->CreateViewMatrix(SkyCamPos, CameraTarget, Vector3(0.0f, 1.0, 0.0f));
		MainCameraSystem->TargetCamera = MoveCamera;
	}

	// �������ַ���
	MoveTimer += Timer::GetInstance()->GetDeltaTime();

	if (MoveTimer >= 0.3f)
	{
		// �̵� ���������� UI ������, ��ҹ�ư? �׷��Ǿ���
		WRS->hide = true;

		// ���� ���� ���� ���ٸ� (�̵� ����� ���� ������ �ʾҴٸ�)
		if (MAIN_PICKER.optPickingMode != PMOD_CHARACTER) 
		{
			// �Ÿ��� ���� ��ġ �̻��̸� �̵��Ұ�
			Vector3 Forward = player->chara->Transform->Translation - MAIN_PICKER.vIntersection;
			float CanMoveRange = 100.0f; // ��ġ ����
			float Distance = Forward.Length();
			if (Distance < CanMoveRange) 
			{
				MAIN_PICKER.SetMoveUIColor(&TheWorld, { 0.0f, 0.0f, 0.0f, 0.5f }, { 0.5f, 1.0f, 0.0f, 0.5f });

				// ���� ����ٸ�, �̵���� ������ ī�޶� �ٲ���
				KeyState btnLC = Input::GetInstance()->getKey(VK_LBUTTON);
				if (btnLC == KeyState::Down)
				{
					// ������ ����ī�޶�� Ȯ �ٲٴ°�?
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

		// ���� �����, �÷��̾��� �̵��� �����ٸ�
		else if (!PlayerCharacter->MovementComp->IsMoving) 
		{
			IsMoving = false;
			MoveTimer = 0.0f;
			WRS->hide = false;

			// Ÿ������ ���� ���ٸ� �ڵ����� ù��° �� Ÿ����
			if (TargetEnemy == nullptr) { TargetEnemy = EnemyList[0]; }
			// �÷��̾� ĳ���� ���� ȸ�� (Ÿ������ �� ������)
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
				//timeline->SetTarget(&player->chara->Transform->Rotation.y); // ������Ʈ �߰��� �� �̸� �صα�?
				timeline->PlayFromStart();
			}

			player->Forward = NewPlayerForward;

			// �� ĳ���� ���� ȸ�� (�÷��̾� ĳ���� ������)
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
					//timeline->SetTarget(&player->chara->Transform->Rotation.y); // ������Ʈ �߰��� �� �̸� �صα�?
					timeline->PlayFromStart();
				}
				enemy->chara->MovementComp->Forward = NewForward;
			}

			// ī�޶� ����
			auto CameraTarget = TargetEnemy->chara->Transform->Translation;
			CameraTarget.y += 10.0f;
			UpdateCameraPos();
			MainCameraSystem->MainCamera = MainCamera;
			MoveCamera->CreateViewMatrix(PlayerCameraPos, CameraTarget, Vector3(0.0f, 1.0, 0.0f));
			MainCameraSystem->TargetCamera = MoveCamera;

			// �ϴ� �̵��� �ѹ����� ����, �ڽ�Ʈ ������ ������ �� �𸣰ڴ�
			MoveButton->m_bIsDead = true;
		}
	}
}

void BattleScene::CardCheck()
{
	if (TargetEnemy == nullptr || TargetEnemy->hp <= 0) { return; } // Ÿ���� ���ų� �׾��ٸ� ���त

	for (int cardNum = 0; cardNum < Dick->HandList.size(); cardNum++)
	{
		if (CardList[cardNum]->m_bClicked && CardList[cardNum]->m_OriginPos.y >= 0.5)	// ī�带 �巡���ؼ� ���� ���� �̻����� �ö󰬴ٸ�
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
				}

				PAnime->SetClipByName(L"Inward_Block");
				PlayEffect(&TheWorld, L"Shield1", { player->chara->GetComponent<BoundingBoxComponent>()->OBB.Center, Vector3(), {3.0f, 3.0f, 3.0f} }, { false, 1.0f, 0.2f, 1.0f });
				SoundMap.find(L"Shield")->second->Play();
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
				// UpdateEnemyState();		// ���⼭ ���� ��Ƽ���̿��� ó���ص� �����Ѱ�?
				if (TargetEnemy->hp <= 0) 
				{
					// �� ĳ���Ͱ� �׾��ٸ� �� ����Ʈ���� ����
					auto iter = std::find(EnemyList.begin(), EnemyList.end(), TargetEnemy);
					if (iter != EnemyList.end()) { EnemyList.erase(iter); }
					
					// ����â ����
					for (auto obj : TargetEnemy->ObjList) { obj->m_bIsDead = true; }
					TargetEnemy->HpEmpty->m_bIsDead = true;
					TargetEnemy->HpFull->m_bIsDead = true;
				}

				// ī�޶� ��ũ
				Vector3 t1 = player->chara->GetComponent<BoundingBoxComponent>()->OBB.Center;
				Vector3 t2 = TargetEnemy->chara->GetComponent<BoundingBoxComponent>()->OBB.Center;
				float dist = t1.Distance(t1, t2);
				if (dist < 30.0f) // ������� �̻� ����� ���� ī�޶� ����
				{ 
					dist = 40.0f; 
					TestActionCamera->Lerp(t1, t2, dist, 0.1f);

					MoveCamera->CreateViewMatrix(TestActionCamera->MovementComp->Destination, TestActionCamera->ActionCameraComp->NextLook, Vector3(0.0f, 1.0, 0.0f));
					MainCameraSystem->TargetCamera = MoveCamera;
				}
			}
			else {} // ���⼭ �������� ����� ���

		}
	}
}

// ������ �ִϸ��̼�, 
void BattleScene::DamageAnimation(int damage, bool hitEnemy)
{
	int fontWidth = 140.0f;	// �� ���� �ʺ�

	// �Ʊ� ĳ���Ͱ� �� ����, �� ū���� ���� �� ������ ���󰡴� ����
	if (hitEnemy) 
	{
		// �������� 10 �̻��� ���, ���ڸ��� ����
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

	// ���� �Ʊ� ����
	else 
	{	// �������� 10 �̻��� ���, ���ڸ��� ����
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
		if (card >= UsedCard) // ���� ī�� ��ġ���� ��ĭ�� �о �ִϸ��̼� �������� ��� �����غ���
		{
			// ��ο�ī�带 ���� ��, ���� ������+1�� �����ϸ� �ȵǴϱ�
			if (DrawedCard > 0 && card + DrawedCard >= handSize)
			{
				AnimPos[0] = { -100.0f, 700.0f };
				AnimTime = 0.5f;
			}	// ��ο�� ī��� �ָ��� ������ϱ� �ִϸ��̼� �ð� �� ����

			// ��ο�X
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
	// ���� (�ڽ�Ʈ)
	CurrentMana->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->cost];
	MaxMana->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->maxCost];

	// 
	PlayerCurrenHP1->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->hp / 10];
	PlayerCurrenHP2->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->hp % 10];
	PlayerMaxHP1->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->maxHp / 10];
	PlayerMaxHP2->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->maxHp % 10];

	// ���α׷��� �� (HP��) 
	float hpPercentage = (float)player->hp / (float)player->maxHp;
	PlayerHPBar->m_pCutInfoList[0]->pxWH.x = PlayerHPBar->m_OriginWH.x * hpPercentage;	// X�ุ(���ηθ�)�پ�����ϴϱ�
	PlayerHPBar->m_pCutInfoList[0]->uv = PlayerHPBar->m_OriginUV * hpPercentage;			// x�ุ ����

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
			if (enemy->chara->GetComponent<AnimationComponent>()->IsInAction()) return; // ���� �ϳ��� �ִϸ��̼����̶�� Ż��!
		}
		SS = GAMEOVER;
	}
	else
	{
		for (auto enemy : AllEnemyList)
		{
			if (enemy->hp > 0 || enemy->chara->GetComponent<AnimationComponent>()->IsInAction()) return; // ���� �ϳ��� ����ְų� �״� �ִϸ��̼����̶�� Ż��!
			else if (player->chara->GetComponent<AnimationComponent>()->IsInAction()) { return; }	// �÷��̾ �ൿ���̾ �ȳ�����, ��,, ��Ƽ���̸��߸鼭��ư�̷�����
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

	if (RemainCardButton->m_bHover) // ���� ī�� ��ư
	{
		ToolTipText->m_pCutInfoList[0]->tc = TextTextureList.find(L"remain")->second;
		ToolTipBoard->m_bIsDead = false;
		ToolTipText->m_bIsDead = false;
	}
	else if (DiscardButton->m_bHover) // ���� ī�� ��ư
	{
		ToolTipText->m_pCutInfoList[0]->tc = TextTextureList.find(L"discard")->second;
		// ������ ���̶� �������� ������
		ToolTipBoard->m_OriginPos.x = mouseNdcX - ToolTipBoard->PtoN(ToolTipBoard->m_pCutInfoList[0]->pxWH).x;	
		ToolTipText->m_OriginPos.x = mouseNdcX - ToolTipText->PtoN(ToolTipText->m_pCutInfoList[0]->pxWH).x;
		ToolTipBoard->m_bIsDead = false;
		ToolTipText->m_bIsDead = false;
	}
	else if (MoveButton->m_bHover && !MoveButton->m_bIsDead) // �̵� ��ư
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
