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
#include "AnimationComponent.h"
#include "UpdateAnimSystem.h"
#include "MovementSystem.h"
#include "Character.h"

///////////////////
//Effect Include
///////////////////
#include "EffectInclude\ParticleEffect.h"

bool BattleScene::Init()
{
	ID = battle;

	// 사실 플레이어는 타이틀에서 함 초기화하고 가는것도..
	player = new Player;
	player->maxHp = 50;
	player->hp = 50; 
	enemy = new Enemy_1;
	enemy->Init();

	Init_UI();
	Init_Map();
	Init_Chara();
	Init_Effect();

	// 카메라 시스템 및 랜더링 시스템 추가.
	TheWorld.AddSystem(new CameraSystem);
	TheWorld.AddSystem(new RenderSystem);
	TheWorld.AddSystem(new WidgetRenderSystem);
	TheWorld.AddSystem(new MovementSystem);
	TheWorld.AddSystem(new UpdateAnimSystem);
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

	return true;
}

bool BattleScene::Render()
{
	BaseScene::Render();

	// 남은 카드 확인 or 버린 카드 확인 버튼 클릭시 씬 전환 (카드 보는 씬으로)
	if (RemainCardButton->m_bClicked){ SS = remainView; RemainCardButton->m_bClicked = false; }
	if (DiscardButton->m_bClicked) { SS = discardView; DiscardButton->m_bClicked = false; }

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
	//UI->Name = L"Battle";
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
	UpdatePlayerState();

	// 적 상태
	EnemyCurrentHP1 = bc->FindObj(L"EnemyCurrentHp_1");
	EnemyCurrentHP2 = bc->FindObj(L"EnemyCurrentHp_2");
	EnemyMaxHP1 = bc->FindObj(L"EnemyMaxHp_1");
	EnemyMaxHP2 = bc->FindObj(L"EnemyMaxHp_2");
	UpdateEnemyState();

	// 메인 월드에 액터 추가.
	TheWorld.AddEntity(UI);
}

void BattleScene::Init_Map()
{	
	// 카메라 액터 추가.
	Actor* cameraActor = new Actor;
	MainCamera = cameraActor->AddComponent<Camera>();
	MainCamera->CreateViewMatrix(Vector3(0.0f, 5.0f, -100.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0, 0.0f));
	MainCamera->CreateProjectionMatrix(1.0f, 10000.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));

	// 지형 액터 추가.
	Actor* landscapeActor = new Actor;
	auto landscape = landscapeActor->AddComponent<Landscape>();
	landscape->Build(8, 8, 7);
	landscape->SetCamera(MainCamera);
	auto landTransform = landscapeActor->GetComponent<TransformComponent>();

	// Sky Dome 추가.
	Actor* skyDomeActor = new Actor;
	auto skyDomeComp = skyDomeActor->AddComponent<SkyDomeComponent>();
	skyDomeComp->Scale = Vector3(500.0f, 500.0f, 500.0f);
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

	// 9. 메인 월드에 액터 추가.
	TheWorld.AddEntity(cameraActor);
	TheWorld.AddEntity(landscapeActor);
	TheWorld.AddEntity(skyDomeActor);
}

void BattleScene::Init_Chara()
{
	PlayerCharacter = new Character;
	auto playerCharMeshComp = PlayerCharacter->AddComponent<SkeletalMeshComponent>();
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Hulk_fbx/hulk_removeTwist.FBX"))
	{
		FBXLoader::GetInstance()->GenerateSkeletalMeshFromFileData(L"../resource/FBX/Hulk_fbx/hulk_removeTwist.FBX", playerCharMeshComp);
	}
	auto playerCharAnimComp = PlayerCharacter->AddComponent<AnimationComponent>();
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Hulk_fbx/Mutant_Punch_Retargeted.FBX"))
	{
		FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Hulk_fbx/Mutant_Punch_Retargeted.FBX", playerCharAnimComp);
	}
	auto playerCharTransformComp = PlayerCharacter->GetComponent<TransformComponent>();
	playerCharTransformComp->Scale = Vector3(10.f, 10.f, 10.f);
	playerCharTransformComp->Rotation = Vector3(0.0f, 90.0f, 0.0f);
	playerCharTransformComp->Translation = Vector3(0.0f, 0.0f, 100.0f);

	auto playerCharMovementComp = PlayerCharacter->GetComponent<MovementComponent>();
	playerCharMovementComp->Speed = 10.0f;
	playerCharMovementComp->Destination = Vector3(0.0f, 0.0f, -100.0f);
	TheWorld.AddEntity(PlayerCharacter);
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

			switch (Dick->HandList[cardNum])
			{

			case Strike:
			{
				enemy->hp -= 6;
			}break;

			case Defend:
			{
				player->armor += 5;
			}break;

			case PommelStrike:
			{
				enemy->hp -= 9;
				Dick->Draw(1);
			}break;

			case ShrugItOff:
			{
				player->armor += 8;
				Dick->Draw(1);
			}break;

			case Hemokinesis:
			{
				
			}break;

			case Bludgeon:
			{
				
			}break;

			case IronWave:
			{
				enemy->hp -= 5;
				player->armor += 5;
			}break;

			}

			Dick->Use(cardNum);
			UpdateHand(Dick->HandList.size());
			UpdatePlayerState();
			UpdateEnemyState();
		}
	}
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
	PlayerCurrenHP1->m_pCutInfoList[0]->tc = NumberTextureList_Red[player->hp / 10];
	PlayerCurrenHP2->m_pCutInfoList[0]->tc = NumberTextureList_Red[player->hp % 10];
	PlayerMaxHP1->m_pCutInfoList[0]->tc = NumberTextureList_Red[player->maxHp / 10];
	PlayerMaxHP2->m_pCutInfoList[0]->tc = NumberTextureList_Red[player->maxHp % 10];

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
			PlayerArmor1->m_OriginPos = { -0.714, -0.693 };
			PlayerArmor2->m_OriginPos = { -0.693, -0.693 };

			PlayerArmor1->m_bIsDead = false;
			PlayerArmor1->m_pCutInfoList[0]->tc = NumberTextureList_Black[player->armor / 10];
		}
		else
		{
			PlayerArmor2->m_OriginPos = { -0.703, -0.693 };
		}
	}
}

void BattleScene::UpdateEnemyState()
{
	if (enemy->hp <= 0) 
	{
		// 적 격파 이벤트 발생
		enemy->hp = 0;
	}

	EnemyCurrentHP1->m_pCutInfoList[0]->tc = NumberTextureList_Red[enemy->hp / 10];
	EnemyCurrentHP2->m_pCutInfoList[0]->tc = NumberTextureList_Red[enemy->hp % 10];
	EnemyMaxHP1->m_pCutInfoList[0]->tc = NumberTextureList_Red[enemy->maxHp / 10];
	EnemyMaxHP2->m_pCutInfoList[0]->tc = NumberTextureList_Red[enemy->maxHp % 10];

	// 방어도 부분, 좀 나중에..
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
