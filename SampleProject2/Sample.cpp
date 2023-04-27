#include "Sample.h"
#include "FBXLoader.hpp"
#include "EffectInclude/EffectSystem.h"

#include "ColliderSystem.h"
#include "BoundingBoxComponent.h"
#include "BoundingSphereComponent.h"

//서버 추가 
#include "ServerPacketHandler.h"
#include "ClientPacketHandler.h"

// 밑에 놈들은 BaseScene 아니면 BattleScene 밑으로 갔읍니다 
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
////추가
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
			OutputDebugString(L"Entity 생성 \n");
		}

		virtual void receive(class World* world, const CommonEvents::OnEntityDestroyed& event) override
		{
			OutputDebugString(L"Entity 파괴 \n");
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
	Battle->NumberTextureList_Damage = NumberTextureList_Damage;
	Battle->TextTextureList = TextTextureList;
	Battle->Init();
	MultiBattle = new MultiBattleScene;
	MultiBattle->MyDeck = Dick;
	MultiBattle->CardTextureList = CardTextureList;
	MultiBattle->NumberTextureList_Red = NumberTextureList_Red;
	MultiBattle->NumberTextureList_Black = NumberTextureList_Black;
	MultiBattle->NumberTextureList_Damage = NumberTextureList_Damage;
	MultiBattle->TextTextureList = TextTextureList;
	MultiBattle->Init();
	//serverpackethandler에서 연결하기 위한 포인터
	multyserver = MultiBattle;
	//clientpackethandler에서 연결하기 위한 포인터
	multyclient = MultiBattle;
	CardView = new CardViewScene;
	CardView->Dick = Dick;
	CardView->CardTextureList = CardTextureList;
	CardView->Init();
	Clear = new ClearScene;
	Clear->Init();
	GameOver = new GameOverScene;
	GameOver->Init();

	CurrentScene = Title;
	//CurrentScene = MultiBattle;

	////테스트 시스템 추가
	////ECS::TestSystem* test = new ECS::TestSystem;
	////MainWorld.AddSystem(test);
	////MainWorld.RemoveSystem(test);

	////<커스텀 이벤트 테스트>
	////---------------------------------------------
	//MainWorld.emit<CustomEvent>({856, FALSE});
	////---------------------------------------------

	////Collider System 테스트
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
	//GThreadManager->Join();
	//서버 : 난 쓰레드한태 졌어
	if (gpHost != nullptr)
		delete(gpHost);
	if (gpClient != nullptr)
		delete(gpClient);
	GThreadManager->eraseThreadVector();


	///////////////////////
	//EFFECTUTIL CleanUp
	///////////////////////
	EFFECTUTIL::EFFECT_MGR.release();
	EFFECTUTIL::SPRITE_MGR.release();
	EFFECTUTIL::FILEIO_MGR.release();
	EFFECTUTIL::DXSTATE_MGR.release();

	BV_MGR.release();

	//서버 : 이거 뻑나서 일단 주석
	//Loading->Release();
	//delete Loading;
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
	FBXLoader::GetInstance()->Release();

	//GThreadManager->eraseThreadVector();

	return EditorCore::Release();
}


void SampleCore::LoadTexture()
{
	// 카드 텍스처 로드
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


	// 숫자 텍스처 로드, 일단 빨갱이
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

	// 숫자 텍스처 로드, 까망이
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

	// 숫자 텍스처 로드, 데미지용 멋드러진 폰트
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/damage/0.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/damage/0.png");
		NumberTextureList_Damage.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/damage/1.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/damage/1.png");
		NumberTextureList_Damage.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/damage/2.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/damage/2.png");
		NumberTextureList_Damage.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/damage/3.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/damage/3.png");
		NumberTextureList_Damage.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/damage/4.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/damage/4.png");
		NumberTextureList_Damage.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/damage/5.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/damage/5.png");
		NumberTextureList_Damage.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/damage/6.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/damage/6.png");
		NumberTextureList_Damage.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/damage/7.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/damage/7.png");
		NumberTextureList_Damage.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/damage/8.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/damage/8.png");
		NumberTextureList_Damage.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/damage/9.png"))
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/damage/9.png");
		NumberTextureList_Damage.push_back(tc);
	}

	// 텍스트 텍스처 로드, 일단 주로 툴팁용
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Text/base.png"))	// 베이스, 암것도 없음
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Text/base.png");
		TextTextureList.insert(make_pair(L"base", tc));
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Text/test.png"))	// 테스트
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Text/test.png");
		TextTextureList.insert(make_pair(L"test", tc));
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Text/remain.png"))	// 남은 덱
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Text/remain.png");
		TextTextureList.insert(make_pair(L"remain", tc));
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Text/discard.png"))	// 버린 카드
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Text/discard.png");
		TextTextureList.insert(make_pair(L"discard", tc));
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Text/move.png"))	// 이동 버튼
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Text/move.png");
		TextTextureList.insert(make_pair(L"move", tc));
	}

	// 여기는 적 이름 텍스처 파일 ㅇㅇ
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Text/EnemyName1.png"))	// 적1
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Text/EnemyName1.png");
		TextTextureList.insert(make_pair(L"enemy1", tc));
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Text/EnemyName2.png"))	// 적2
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Text/EnemyName2.png");
		TextTextureList.insert(make_pair(L"enemy2", tc));
	}
}

void SampleCore::LoadBGM()
{
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/BGM/Aru_Piano.mp3", SoundType::BGM))
	{
		bgm_Title = FMODSoundManager::GetInstance()->GetSound(L"Aru_Piano.mp3");
		bgm_Title->Play();
		bgm_Title->SetVolume(0.3); // 볼륨 0 ~ 1 사이 값.
		bgm_Title->SetLoop(true); // Effect여도 Loop true 가능.
		bgm_Title->Stop();
	}
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/BGM/Map.ogg", SoundType::BGM))
	{
		bgm_Map = FMODSoundManager::GetInstance()->GetSound(L"Map.ogg");
		bgm_Map->Play();
		bgm_Map->SetVolume(0.3); // 볼륨 0 ~ 1 사이 값.
		bgm_Map->SetLoop(true); // Effect여도 Loop true 가능.
		bgm_Map->Stop();
	}
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/BGM/Battle1.mp3", SoundType::BGM))
	{
		bgm_Battle = FMODSoundManager::GetInstance()->GetSound(L"Battle1.mp3");
		bgm_Battle->Play();
		bgm_Battle->SetVolume(0.1); // 볼륨 0 ~ 1 사이 값.
		bgm_Battle->SetLoop(true); // Effect여도 Loop true 가능.
		bgm_Battle->Stop();
	}
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/BGM/Aru.mp3", SoundType::BGM))
	{
		bgm_Clear = FMODSoundManager::GetInstance()->GetSound(L"Aru.mp3");
		bgm_Clear->Play();
		bgm_Clear->SetVolume(0.3); // 볼륨 0 ~ 1 사이 값.
		bgm_Clear->SetLoop(true); // Effect여도 Loop true 가능.
		bgm_Clear->Stop();
	}
	if (FMODSoundManager::GetInstance()->Load(L"../resource/Sound/BGM/GameOver.mp3", SoundType::BGM))
	{
		bgm_GameOver = FMODSoundManager::GetInstance()->GetSound(L"GameOver.mp3");
		bgm_GameOver->Play();
		bgm_GameOver->SetVolume(0.3); // 볼륨 0 ~ 1 사이 값.
		bgm_GameOver->SetLoop(true); // Effect여도 Loop true 가능.
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
			bgm_Current = bgm_Map;
			bgm_Current->Play();
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
			if (CurrentScene->ID != DECKVIEW && CurrentScene->ID != REMAINVIEW) 
			{
				bgm_Current->Stop();
				bgm_Current = bgm_Battle;
				bgm_Current->Play();
				bgm_Current->SetVolume(0.3);
			}

			CurrentScene->SS = MAINTAIN;
			CurrentScene = Battle;
		}break;

		case MULTIBATTLE:
		{
			CurrentScene->SS = MAINTAIN;
			CurrentScene = MultiBattle;
			//서버에서 주석
			//MultiBattle->CurrentPlayer = MultiBattle->player1;
			//MultiBattle->playerNum = 1;

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