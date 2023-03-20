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

///////////////////
//Effect Include
///////////////////
#include "EffectInclude\ParticleEffect.h"

bool BattleScene::Init()
{
	ID = battle;

	// ��� �÷��̾�� Ÿ��Ʋ���� �� �ʱ�ȭ�ϰ� ���°͵�..
	player = new Player;
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
	TheWorld.AddSystem(new RenderSystem);
	TheWorld.AddSystem(new WidgetRenderSystem);

	return true;
}

bool BattleScene::Frame()
{
	BaseScene::Frame();
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

	// �÷��̾� �Žñ� HPǥ�ÿ� 
	PlayerCurrenHP1 = bc->FindObj(L"PlayerCurrentHP_1");
	PlayerCurrenHP2 = bc->FindObj(L"PlayerCurrentHP_2");
	PlayerMaxHP1 = bc->FindObj(L"PlayerMaxHP_1");
	PlayerMaxHP2 = bc->FindObj(L"PlayerMaxHP_2");

	PlayerCurrenHP1->m_pCutInfoList[0]->tc = NumberTextureList[player->hp / 10];
	PlayerCurrenHP2->m_pCutInfoList[0]->tc = NumberTextureList[player->hp % 10];
	PlayerMaxHP1->m_pCutInfoList[0]->tc = NumberTextureList[player->maxHp / 10];
	PlayerMaxHP2->m_pCutInfoList[0]->tc = NumberTextureList[player->maxHp % 10];


	// ���� ���忡 ���� �߰�.
	TheWorld.AddEntity(UI);
}

void BattleScene::Init_Map()
{	// 1. Actor ����
	Actor* actor = new Actor;

	// 2. Static Mesh Component �߰�.
	auto comp = actor->AddComponent<StaticMeshComponent>();

	// 3. Material ����
	Material* material = new Material;

	// 4. �ؽ��� �ε� �� ���׸��� �߰�.
	DXTexture* texture = nullptr;
	if (DXTextureManager::GetInstance()->Load(L"../resource/Default.bmp"))
	{
		texture = DXTextureManager::GetInstance()->GetTexture(L"../resource/Default.bmp");
	}
	if (texture != nullptr)
	{
		material->SetDiffuseTexture(texture);
	}

	DXTexture* normalMaptexture = nullptr;
	if (DXTextureManager::GetInstance()->Load(L"../resource/NormalMap/normal1.bmp"))
	{
		normalMaptexture = DXTextureManager::GetInstance()->GetTexture(L"../resource/NormalMap/normal1.bmp");
	}
	if (normalMaptexture != nullptr)
	{
		material->SetNormalTexture(normalMaptexture);
	}

	material->SetPixelShader(DXShaderManager::GetInstance()->GetPixelShader(L"NormalMap"));

	// 5. ��� �޽� ���� �� ���׸��� ����. 
	PlaneComponent* plane = new PlaneComponent;
	plane->SetMaterial(material);
	plane->CalcTangent();

	// 6. ����ƽ �޽��� ��� �޽� �߰�.
	comp->Meshes.push_back(*plane);

	// 7. ���Ϳ� Ʈ���� �� �߰�.
	auto transformComp = actor->AddComponent<TransformComponent>();
	transformComp->Translation = Vector3(0.0f, 0.0, 100.0f);
	//transformComp->Rotation = Vector3(0.0f, 3.14f / 4.0f, 0.0f);
	transformComp->Rotation = Vector3(0.0f, 0.0f, 0.0f);
	transformComp->Scale = Vector3(20.0f, 20.0f, 1.0f);

	// 8. ���Ϳ� ī�޶� �߰�.
	Actor* cameraActor = new Actor;
	MainCamera = cameraActor->AddComponent<Camera>();
	MainCamera->CreateViewMatrix(Vector3(0.0f, 5.0f, -100.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0, 0.0f));
	MainCamera->CreateProjectionMatrix(1.0f, 10000.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));

	// Sky Box
	Actor* skyActor = new Actor;
	auto skyBox = skyActor->AddComponent<SkyBoxComponent>();
	if (DXTextureManager::GetInstance()->Load(L"../resource/Sky/ft.bmp"))
	{
		skyBox->SetFrontTexture(DXTextureManager::GetInstance()->GetTexture(L"../resource/Sky/ft.bmp"));
	}

	if (DXTextureManager::GetInstance()->Load(L"../resource/Sky/bk.bmp"))
	{
		skyBox->SetBackTexture(DXTextureManager::GetInstance()->GetTexture(L"../resource/Sky/bk.bmp"));
	}

	if (DXTextureManager::GetInstance()->Load(L"../resource/Sky/lt.bmp"))
	{
		skyBox->SetLeftTexture(DXTextureManager::GetInstance()->GetTexture(L"../resource/Sky/lt.bmp"));
	}

	if (DXTextureManager::GetInstance()->Load(L"../resource/Sky/rt.bmp"))
	{
		skyBox->SetRightTexture(DXTextureManager::GetInstance()->GetTexture(L"../resource/Sky/rt.bmp"));
	}

	if (DXTextureManager::GetInstance()->Load(L"../resource/Sky/up.bmp"))
	{
		skyBox->SetUpperTexture(DXTextureManager::GetInstance()->GetTexture(L"../resource/Sky/up.bmp"));
	}

	if (DXTextureManager::GetInstance()->Load(L"../resource/Sky/dn.bmp"))
	{
		skyBox->SetLowerTexture(DXTextureManager::GetInstance()->GetTexture(L"../resource/Sky/dn.bmp"));
	}
	float scale = 500.0f;
	auto skyTransformComp = skyActor->AddComponent<TransformComponent>();
	skyTransformComp->Translation = Vector3(0.0f, 0.0, 0.0f);
	skyTransformComp->Rotation = Vector3(0.0f, 0.0f, 0.0f);
	skyTransformComp->Scale = Vector3(scale, scale, scale);

	Actor* landscapeActor = new Actor;
	auto landscape = landscapeActor->AddComponent<Landscape>();
	landscape->Build(8, 8, 7);
	landscape->SetCamera(MainCamera);

	Actor* skyDomeActor = new Actor;
	auto skyDomeComp = skyDomeActor->AddComponent<SkyDomeComponent>();
	skyDomeComp->Scale = Vector3(500.0f, 500.0f, 500.0f);
	TheWorld.AddSystem(new SkyRenderSystem);

	// 9. ���� ���忡 ���� �߰�.
	TheWorld.AddEntity(actor);
	TheWorld.AddEntity(cameraActor);
	TheWorld.AddEntity(landscapeActor);
	TheWorld.AddEntity(skyDomeActor);
}

void BattleScene::Init_Chara()
{
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

	PlayerCurrenHP1->m_pCutInfoList[0]->tc = NumberTextureList[player->hp / 10];
	PlayerCurrenHP2->m_pCutInfoList[0]->tc = NumberTextureList[player->hp % 10];

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
