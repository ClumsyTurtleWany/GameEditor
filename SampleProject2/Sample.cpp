#include "Sample.h"
#include "FBXLoader.hpp"
#include "EffectInclude/EffectSystem.h"

#include "ColliderSystem.h"
#include "BoundingBoxComponent.h"
#include "BoundingSphereComponent.h"

// �ؿ� ����� BaseScene �ƴϸ� BattleScene ������ �����ϴ� 
//#include "WindowsClient.h"
//#include "Actor.h"
//#include "PlaneComponent.h"
//#include "CameraSystem.h"
//#include "RenderSystem.h"
//#include "LightSystem.h"
//#include "DirectionalLight.h"
//#include "SkyBoxComponent.h"
//#include "Landscape.h"
//#include "SkeletalMeshComponent.h"
//#include "MaterialManager.h"
//#include "SkyRenderSystem.h"
//#include "SkyDomeComponent.h"
////�߰�
//#include "AnimationComponent.h"
//#include "UpdateAnimSystem.h"
//#include "MovementSystem.h"
//#include "Character.h"

struct CustomEvent
{
	int SomeNumber;
	bool SomeBoolean;
};

struct TickEvent
{
	float fCurTime;
};

ECS_DEFINE_TYPE(SomeEvent);
namespace ECS
{
	class TestSystem : public System,
		public EventSubscriber<CommonEvents::OnEntityCreated>,
		public EventSubscriber<CommonEvents::OnEntityDestroyed>,
		//public EventSubscriber<CommonEvents::OnComponentAdded<TransformComponent>>,
		public EventSubscriber<CustomEvent>,
		public EventSubscriber<TickEvent>
	{
	public:
		virtual ~TestSystem() {}

		void init(World* world) override
		{
			world->subscribe<CommonEvents::OnEntityCreated>(this);
			world->subscribe<CommonEvents::OnEntityDestroyed>(this);
			//world->subscribe<CommonEvents::OnComponentAdded<TransformComponent>>(this);
			world->subscribe<CustomEvent>(this);
			world->subscribe<TickEvent>(this);
		};

		void release(World* world) override
		{
			world->unsubscribe<CommonEvents::OnEntityCreated>(this);
			world->unsubscribe<CommonEvents::OnEntityDestroyed>(this);
			//world->subscribe<CommonEvents::OnComponentAdded<TransformComponent>>(this);
			world->unsubscribe<CustomEvent>(this);
			world->unsubscribe<TickEvent>(this);
		}

		virtual void Tick(World* world, float deltaTime) override
		{
			world->emit<TickEvent>({ Singleton<Timer>::GetInstance()->GetPlayTime() });
		}

		virtual void receive(class World* world, const CommonEvents::OnEntityCreated& event) override
		{
			OutputDebugString(L"Entity ���� \n");
		}

		virtual void receive(class World* world, const CommonEvents::OnEntityDestroyed& event) override
		{
			OutputDebugString(L"Entity �ı� \n");
		}

		/*virtual void receive(class World* world, const CommonEvents::OnComponentAdded<TransformComponent>& event) override
		{
			OutputDebugString(L"A Transform component was removed!");
		}*/

		virtual void receive(class World* world, const CustomEvent& event) override
		{
			OutputDebugString(L"Custom Event Value : ");
			OutputDebugString(L"\n");
			OutputDebugString((L"Int Param : " + std::to_wstring(event.SomeNumber)).c_str());
			OutputDebugString(L"\n");
			OutputDebugString((L"Boolean Param : " + std::to_wstring(event.SomeBoolean)).c_str());
			OutputDebugString(L"\n");
		}

		virtual void receive(class World* world, const TickEvent& event) override
		{
			OutputDebugString((std::to_wstring(event.fCurTime) + L" : DEBUG_TICK_EVENT\n").c_str());
		}
	};
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	WindowsClient sampleWindow;
	sampleWindow.Create(hInstance, L"Window Name", 1600, 900);

	SampleCore core;
	core.SetWindowHandle(sampleWindow.GetHandle());
	core.CoreInitialize();
	bool isRun = true;
	while (isRun)
	{
		if (!sampleWindow.Run())
		{
			isRun = false;
		}
		else
		{
			core.CoreFrame();
			core.CoreRender();
		}
	}

	core.CoreRelease();

	return 1;
}

SampleCore::SampleCore()
{
}

SampleCore::~SampleCore()
{
}

bool SampleCore::Initialize()
{
	EditorCore::Initialize();
	FBXLoader::GetInstance()->Initialize();
	
	Dick = new Deck;
	LoadTexture();
	LoadBGM();
	bgm_Current = bgm_Title;
	bgm_Current->Play();

	// �ʱ� �ε��߿� �ε�ȭ�� �ٷ� ����.. �ε� ���� ���ϴ� �׸��� �ȳ�����..
	//Loading = new LoadingScene;
	//Loading->Init();
	//Loading->Frame();
	//Loading->Render();

	Title = new TitleScene;
	Title->Init();
	Select = new SelectScene;
	Select->Init();
	Map = new MapScene;
	Map->Init();
	Battle = new BattleScene;
	Battle->Dick = Dick;
	Battle->CardTextureList = CardTextureList;
	Battle->NumberTextureList_Red = NumberTextureList_Red;
	Battle->NumberTextureList_Black = NumberTextureList_Black;
	Battle->Init();
	MultiBattle = new MultiBattleScene;
	MultiBattle->Dick = Dick;
	MultiBattle->CardTextureList = CardTextureList;
	MultiBattle->NumberTextureList_Red = NumberTextureList_Red;
	MultiBattle->NumberTextureList_Black = NumberTextureList_Black;
	MultiBattle->Init();
	CardView = new CardViewScene;
	CardView->Dick = Dick;
	CardView->CardTextureList = CardTextureList;
	CardView->Init();
	Clear = new ClearScene;
	Clear->Init();
	GameOver = new GameOverScene;
	GameOver->Init();

	CurrentScene = Title;
	//CurrentScene = Battle;

	////�׽�Ʈ �ý��� �߰�
	////ECS::TestSystem* test = new ECS::TestSystem;
	////MainWorld.AddSystem(test);
	////MainWorld.RemoveSystem(test);

	////<Ŀ���� �̺�Ʈ �׽�Ʈ>
	////---------------------------------------------
	//MainWorld.emit<CustomEvent>({856, FALSE});
	////---------------------------------------------

	////Collider System �׽�Ʈ
	//Actor* temp = new Character;
	//auto transform = temp->AddComponent<TransformComponent>();
	//transform->Translation = { 100.0f, 5.0f, 0.0f };
	//auto playerCharMeshComp = temp->AddComponent<SkeletalMeshComponent>();
	//auto boundBox = temp->AddComponent<BoundingBoxComponent>(Vector3(5.0f, 5.0f, 5.0f));
	//auto boundSphere = temp->AddComponent<BoundingSphereComponent>(7.0f);
	//if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Hulk_fbx/HULK.FBX")) //hulk_removeTwist
	//{
	//	FBXLoader::GetInstance()->GenerateSkeletalMeshFromFileData(L"../resource/FBX/Hulk_fbx/HULK.FBX", playerCharMeshComp);
	//}

	//Battle->TheWorld.AddEntity(temp);

	MAIN_PICKER.optPickingMode = PMOD_LAND;

	return true;
}

bool SampleCore::Frame()
{
	if (CurrentScene->MainCamera)
	{
		MAIN_PICKER.setMatrix(nullptr, &(CurrentScene->MainCamera->View), &(CurrentScene->MainCamera->Projection));
	}

	EditorCore::Frame();
	MainWorld.CleanUp();

	SceneChange();
	CurrentScene->Frame();

	return true;
}

bool SampleCore::Render()
{
	EditorCore::Render();
	CurrentScene->Render();

	//float dt = Timer::GetInstance()->SecondPerFrame;
	//MainWorld.Tick(dt);

	Input::GetInstance()->cleanUpWheelState();

	return true;
}

bool SampleCore::Release()
{
	//server Release
	gpHost->Release();
	delete(gpHost);
	
	gpClient->Release();
	delete(gpClient);

	///////////////////////
	//EFFECTUTIL CleanUp
	///////////////////////
	EFFECTUTIL::EFFECT_MGR.release();
	EFFECTUTIL::SPRITE_MGR.release();
	EFFECTUTIL::FILEIO_MGR.release();
	EFFECTUTIL::DXSTATE_MGR.release();

	BV_MGR.release();

	FBXLoader::GetInstance()->Release();
	return EditorCore::Release();

	Loading->Release();
	delete Loading;
	Title->Release();
	delete Title;
	Map->Release();
	delete Map;
	Battle->Release();
	delete Battle;
	CardView->Release();
	delete CardView;
	Clear->Release();
	delete Clear;
	GameOver->Release();
	delete GameOver;
}


void SampleCore::LoadTexture()
{
	// ī�� �ؽ�ó �ε�
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Card/0_Strike.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Card/0_Strike.png");
		CardTextureList.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Card/1_Defend.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Card/1_Defend.png");
		CardTextureList.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Card/2_PommelStrike.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Card/2_PommelStrike.png");
		CardTextureList.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Card/3_ShrugItOff.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Card/3_ShrugItOff.png");
		CardTextureList.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Card/4_Hemokinesis.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Card/4_Hemokinesis.png");
		CardTextureList.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Card/5_Bludgeon.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Card/5_Bludgeon.png");
		CardTextureList.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Card/6_IronWave.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Card/6_IronWave.png");
		CardTextureList.push_back(tc);
	}


	// ���� �ؽ�ó �ε�, �ϴ� ������
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/red/0.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/red/0.png");
		NumberTextureList_Red.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/red/1.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/red/1.png");
		NumberTextureList_Red.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/red/2.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/red/2.png");
		NumberTextureList_Red.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/red/3.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/red/3.png");
		NumberTextureList_Red.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/red/4.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/red/4.png");
		NumberTextureList_Red.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/red/5.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/red/5.png");
		NumberTextureList_Red.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/red/6.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/red/6.png");
		NumberTextureList_Red.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/red/7.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/red/7.png");
		NumberTextureList_Red.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/red/8.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/red/8.png");
		NumberTextureList_Red.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/red/9.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/red/9.png");
		NumberTextureList_Red.push_back(tc);
	}

	// ���� �ؽ�ó �ε�, �����
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/black/0.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/black/0.png");
		NumberTextureList_Black.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/black/1.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/black/1.png");
		NumberTextureList_Black.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/black/2.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/black/2.png");
		NumberTextureList_Black.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/black/3.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/black/3.png");
		NumberTextureList_Black.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/black/4.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/black/4.png");
		NumberTextureList_Black.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/black/5.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/black/5.png");
		NumberTextureList_Black.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/black/6.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/black/6.png");
		NumberTextureList_Black.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/black/7.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/black/7.png");
		NumberTextureList_Black.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/black/8.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/black/8.png");
		NumberTextureList_Black.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/black/9.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/black/9.png");
		NumberTextureList_Black.push_back(tc);
	}
}

void SampleCore::LoadBGM()
{
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/BGM/Aru_Piano.mp3", SoundType::BGM))
	{
		bgm_Title = FMODSoundManager::GetInstance()->GetSound(L"Aru_Piano.mp3");
		bgm_Title->Play();
		bgm_Title->SetVolume(0.3); // ���� 0 ~ 1 ���� ��.
		bgm_Title->SetLoop(true); // Effect���� Loop true ����.
		bgm_Title->Stop();
	}
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/BGM/Map.ogg", SoundType::BGM))
	{
		bgm_Map = FMODSoundManager::GetInstance()->GetSound(L"Map.ogg");
		bgm_Map->Play();
		bgm_Map->SetVolume(0.3); // ���� 0 ~ 1 ���� ��.
		bgm_Map->SetLoop(true); // Effect���� Loop true ����.
		bgm_Map->Stop();
	}
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/BGM/Battle1.mp3", SoundType::BGM))
	{
		bgm_Battle = FMODSoundManager::GetInstance()->GetSound(L"Battle1.mp3");
		bgm_Battle->Play();
		bgm_Battle->SetVolume(0.1); // ���� 0 ~ 1 ���� ��.
		bgm_Battle->SetLoop(true); // Effect���� Loop true ����.
		bgm_Battle->Stop();
	}
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/BGM/Aru.mp3", SoundType::BGM))
	{
		bgm_Clear = FMODSoundManager::GetInstance()->GetSound(L"Aru.mp3");
		bgm_Clear->Play();
		bgm_Clear->SetVolume(0.3); // ���� 0 ~ 1 ���� ��.
		bgm_Clear->SetLoop(true); // Effect���� Loop true ����.
		bgm_Clear->Stop();
	}
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/BGM/GameOver.mp3", SoundType::BGM))
	{
		bgm_GameOver = FMODSoundManager::GetInstance()->GetSound(L"GameOver.mp3");
		bgm_GameOver->Play();
		bgm_GameOver->SetVolume(0.3); // ���� 0 ~ 1 ���� ��.
		bgm_GameOver->SetLoop(true); // Effect���� Loop true ����.
		bgm_GameOver->Stop();
	}
}

void SampleCore::SceneChange()
{
	if (CurrentScene->SS != MAINTAIN)
	{
		switch (CurrentScene->SS)
		{
		case LOADING:
		{
			CurrentScene->SS = MAINTAIN;
			CurrentScene = Loading;
		}break;

		case TITLE:
		{
			CurrentScene->SS = MAINTAIN;
			CurrentScene = Title;

			bgm_Current->Stop();
			bgm_Current = bgm_Title;
			bgm_Current->Play();
		}break;

		case MULTISELECT:
		{
			CurrentScene->SS = MAINTAIN;
			CurrentScene = Select;

			bgm_Current->Stop();
			//bgm_Current = bgm_Title;
			//bgm_Current->Play();
		}break;

		case MAP:
		{
			CurrentScene->SS = MAINTAIN;
			CurrentScene = Map;

			bgm_Current->Stop();
			bgm_Current = bgm_Map;
			bgm_Current->Play();
		}break;

		case BATTLE:
		{
			CurrentScene->SS = MAINTAIN;
			CurrentScene = Battle;

			bgm_Current->Stop();
			bgm_Current = bgm_Battle;
			bgm_Current->Play();
			bgm_Current->SetVolume(0.3);
		}break;

		case MULTIBATTLE:
		{
			CurrentScene->SS = MAINTAIN;
			CurrentScene = MultiBattle;
			MultiBattle->CurrentPlayer = MultiBattle->player1;
			MultiBattle->playerNum = 1;

			bgm_Current->Stop();
			bgm_Current = bgm_Battle;
			bgm_Current->Play();
			bgm_Current->SetVolume(0.3);
		}break;

		case DECKVIEW:
		{
			CardView->BeforeScene = CurrentScene->ID;
			CurrentScene->SS = MAINTAIN;
			CurrentScene = CardView;
			CardView->Update(deck);
		}break;

		case REMAINVIEW:
		{
			CardView->BeforeScene = CurrentScene->ID;
			CurrentScene->SS = MAINTAIN;
			CurrentScene = CardView;
			CardView->Update(remain);
		}break;

		case DISCARDVIEW:
		{
			CardView->BeforeScene = CurrentScene->ID;
			CurrentScene->SS = MAINTAIN;
			CurrentScene = CardView;
			CardView->Update(discard);
		}break;

		case CLEAR: 
		{			
			CurrentScene->SS = MAINTAIN;
			CurrentScene = Clear;

			bgm_Current->Stop();
			bgm_Current = bgm_Clear;
			bgm_Current->Play();
		}break;


		case GAMEOVER: 
		{			
			CurrentScene->SS = MAINTAIN;
			CurrentScene = GameOver;

			bgm_Current->Stop();
			bgm_Current = bgm_GameOver;
			bgm_Current->Play();
		}break;

		}
	}
}