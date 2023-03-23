#include "Sample.h"
#include "FBXLoader.hpp"
#include "EffectInclude/EffectSystem.h"

#include "ColliderSystem.h"
#include "BoundingBoxComponent.h"
#include "BoundingSphereComponent.h"

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
	TextureLoad();

	// 초기 로딩중에 로딩화면 바로 띄우기.. 인데 뭔가 원하는 그림이 안나오네..
	//Loading = new LoadingScene;
	//Loading->Init();
	//Loading->Frame();
	//Loading->Render();

	Title = new TitleScene;
	Title->Init();
	Map = new MapScene;
	Map->Init();
	Battle = new BattleScene;
	Battle->Dick = Dick;
	Battle->CardTextureList = CardTextureList;
	Battle->NumberTextureList_Red = NumberTextureList_Red;
	Battle->NumberTextureList_Black = NumberTextureList_Black;
	Battle->Init();
	CardView = new CardViewScene;
	CardView->Dick = Dick;
	CardView->CardTextureList = CardTextureList;
	CardView->Init();

	CurrentScene = Title;
	//CurrentScene = Battle;

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

	return true;
}

bool SampleCore::Release()
{
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
}


void SampleCore::TextureLoad()
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
}

void SampleCore::SceneChange()
{
	if (CurrentScene->SS != maintain)
	{
		switch (CurrentScene->SS)
		{
		case loading:
		{
			CurrentScene->SS = maintain;
			CurrentScene = Loading;
		}break;

		case title:
		{
			CurrentScene->SS = maintain;
			CurrentScene = Title;
		}break;

		case map:
		{
			CurrentScene->SS = maintain;
			CurrentScene = Map;
		}break;

		case battle:
		{
			CurrentScene->SS = maintain;
			CurrentScene = Battle;
		}break;

		case deckView:
		{
			CardView->BeforeScene = CurrentScene->ID;
			CurrentScene->SS = maintain;
			CurrentScene = CardView;
			CardView->Update(deck);
		}break;

		case remainView:
		{
			CardView->BeforeScene = CurrentScene->ID;
			CurrentScene->SS = maintain;
			CurrentScene = CardView;
			CardView->Update(remain);
		}break;

		case discardView:
		{
			CardView->BeforeScene = CurrentScene->ID;
			CurrentScene->SS = maintain;
			CurrentScene = CardView;
			CardView->Update(discard);
		}break;

		}
	}
}