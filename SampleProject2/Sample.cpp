#include "Sample.h"
//#include "WindowsClient.h"
//#include "Actor.h"
//#include "PlaneComponent.h"
//#include "CameraSystem.h"
//#include "RenderSystem.h"
//#include "LightSystem.h"
//#include "DirectionalLight.h"
//#include "SkyBoxComponent.h"
#include "FBXLoader.hpp"
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

///////////////////
//Effect Include
///////////////////
#include "EffectInclude\ParticleEffect.h"

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
	Loading = new LoadingScene;
	Loading->Init();
	Loading->Frame();
	Loading->Render();

	Title = new TitleScene;
	Title->Init();
	Map = new MapScene;
	Map->Init();
	Battle = new BattleScene;
	Battle->Dick = Dick;
	Battle->CardTextureList = CardTextureList;
	Battle->NumberTextureList = NumberTextureList_Red;
	Battle->Init();
	CardView = new CardViewScene;
	CardView->Dick = Dick;
	CardView->CardTextureList = CardTextureList;
	CardView->Init();

	CurrentScene = Title;
	//CurrentScene = Battle;

	///////////////////////////////////////
	//EFFECTUTIL Initialization
	///////////////////////////////////////
	EFFECTUTIL::DXSTATE_MGR.init(DXDevice::g_pd3dDevice, DXDevice::g_pImmediateContext);
	EFFECTUTIL::FILEIO_MGR.init();
	EFFECTUTIL::SPRITE_MGR.init();
	EFFECTUTIL::EFFECT_MGR.init();

	////테스트 시스템 추가
	////ECS::TestSystem* test = new ECS::TestSystem;
	////MainWorld.AddSystem(test);
	////MainWorld.RemoveSystem(test);

	//// 1. Actor 생성
	//Actor* actor = new Actor;

	//// 2. Static Mesh Component 추가.
	//auto comp = actor->AddComponent<StaticMeshComponent>();

	//// 3. Material 생성
	//Material* material = new Material;

	//// 4. 텍스쳐 로드 및 머테리얼에 추가.
	//DXTexture* texture = nullptr;
	//if (DXTextureManager::GetInstance()->Load(L"../resource/Default.bmp"))
	//{
	//	texture = DXTextureManager::GetInstance()->GetTexture(L"../resource/Default.bmp");
	//}
	//if (texture != nullptr)
	//{
	//	material->SetDiffuseTexture(texture);
	//}

	//DXTexture* normalMaptexture = nullptr;
	//if (DXTextureManager::GetInstance()->Load(L"../resource/NormalMap/normal1.bmp"))
	//{
	//	normalMaptexture = DXTextureManager::GetInstance()->GetTexture(L"../resource/NormalMap/normal1.bmp");
	//}
	//if (normalMaptexture != nullptr)
	//{
	//	material->SetNormalTexture(normalMaptexture);
	//}

	//material->SetPixelShader(DXShaderManager::GetInstance()->GetPixelShader(L"NormalMap"));

	//// 5. 평면 메쉬 생성 후 머테리얼 세팅. 
	//PlaneComponent* plane = new PlaneComponent;
	//plane->SetMaterial(material);
	//plane->CalcTangent();

	//// 6. 스태틱 메쉬에 평면 메쉬 추가.
	//comp->Meshes.push_back(*plane);

	//// 7. 액터에 트랜스 폼 추가.
	//auto transformComp = actor->AddComponent<TransformComponent>();
	//transformComp->Translation = Vector3(0.0f, 0.0, 100.0f);
	////transformComp->Rotation = Vector3(0.0f, 3.14f / 4.0f, 0.0f);
	//transformComp->Rotation = Vector3(0.0f, 0.0f, 0.0f);
	//transformComp->Scale = Vector3(20.0f, 20.0f, 1.0f);

	////<커스텀 이벤트 테스트>
	////---------------------------------------------

	//MainWorld.emit<CustomEvent>({856, FALSE});

	////---------------------------------------------


	//// 8. 액터에 카메라 추가.
	//Actor* cameraActor = new Actor;
	//DebugCamera = cameraActor->AddComponent<Camera>();
	//DebugCamera->CreateViewMatrix(Vector3(0.0f, 5.0f, -100.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0, 0.0f));
	//DebugCamera->CreateProjectionMatrix(1.0f, 10000.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));

	//// Sky Box
	//Actor* skyActor = new Actor;
	//auto skyBox = skyActor->AddComponent<SkyBoxComponent>();
	//if (DXTextureManager::GetInstance()->Load(L"../resource/Sky/ft.bmp"))
	//{
	//	skyBox->SetFrontTexture(DXTextureManager::GetInstance()->GetTexture(L"../resource/Sky/ft.bmp"));
	//}

	//if (DXTextureManager::GetInstance()->Load(L"../resource/Sky/bk.bmp"))
	//{
	//	skyBox->SetBackTexture(DXTextureManager::GetInstance()->GetTexture(L"../resource/Sky/bk.bmp"));
	//}

	//if (DXTextureManager::GetInstance()->Load(L"../resource/Sky/lt.bmp"))
	//{
	//	skyBox->SetLeftTexture(DXTextureManager::GetInstance()->GetTexture(L"../resource/Sky/lt.bmp"));
	//}

	//if (DXTextureManager::GetInstance()->Load(L"../resource/Sky/rt.bmp"))
	//{
	//	skyBox->SetRightTexture(DXTextureManager::GetInstance()->GetTexture(L"../resource/Sky/rt.bmp"));
	//}

	//if (DXTextureManager::GetInstance()->Load(L"../resource/Sky/up.bmp"))
	//{
	//	skyBox->SetUpperTexture(DXTextureManager::GetInstance()->GetTexture(L"../resource/Sky/up.bmp"));
	//}

	//if (DXTextureManager::GetInstance()->Load(L"../resource/Sky/dn.bmp"))
	//{
	//	skyBox->SetLowerTexture(DXTextureManager::GetInstance()->GetTexture(L"../resource/Sky/dn.bmp"));
	//}
	//float scale = 500.0f;
	//auto skyTransformComp = skyActor->AddComponent<TransformComponent>();
	//skyTransformComp->Translation = Vector3(0.0f, 0.0, 0.0f);
	//skyTransformComp->Rotation = Vector3(0.0f, 0.0f, 0.0f);
	//skyTransformComp->Scale = Vector3(scale, scale, scale);
	//


	//// Fbx Loader Test
	//Character* fbxActor = new Character;
	//auto fbxMeshComp = fbxActor->AddComponent<SkeletalMeshComponent>();
	//if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Hulk_fbx/hulk_removeTwist.FBX"))
	//{
	//	//FBXLoader::GetInstance()->GenerateStaticMeshFromFileData(L"../resource/FBX/charMob.FBX", fbxMeshComp);
	//	FBXLoader::GetInstance()->GenerateSkeletalMeshFromFileData(L"../resource/FBX/Hulk_fbx/hulk_removeTwist.FBX", fbxMeshComp);
	//}
	//auto fbxAnimComp = fbxActor->AddComponent<AnimationComponent>();
	//if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Hulk_fbx/Mutant_Punch_Retargeted.FBX"))
	//{
	//	FBXLoader::GetInstance()->GenerateAnimationFromFileData(L"../resource/FBX/Hulk_fbx/Mutant_Punch_Retargeted.FBX", fbxAnimComp);
	//}
	//auto fbxTransformComp = fbxActor->GetComponent<TransformComponent>();
	//fbxTransformComp->Scale = Vector3(10.f, 10.f, 10.f);
	//fbxTransformComp->Rotation = Vector3(0.0f, 90.0f, 0.0f);
	//fbxTransformComp->Translation = Vector3(0.0f, 0.0f, 100.0f);

	//auto fbxMovementComp = fbxActor->GetComponent<MovementComponent>();
	//fbxMovementComp->Speed = 10.0f;
	//fbxMovementComp->Destination = Vector3(0.0f, 0.0f, -100.0f);
	//MainWorld.AddEntity(fbxActor);

	//Actor* landscapeActor = new Actor;
	//auto landscape = landscapeActor->AddComponent<Landscape>();
	//landscape->Build(8, 8, 7);
	//landscape->SetCamera(DebugCamera);
	//MainWorld.AddEntity(landscapeActor);

	///*Actor* skySphereActor = new Actor;
	//auto skySphereComp = skySphereActor->AddComponent<StaticMeshComponent>();
	//if (FBXLoader::GetInstance()->Load(L"../resource/FBX/SkySphere.fbx"))
	//{
	//	FBXLoader::GetInstance()->GenerateStaticMeshFromFileData(L"../resource/FBX/SkySphere.fbx", skySphereComp);
	//}
	//MainWorld.AddEntity(skySphereActor);
	//auto skySphereTransform = skySphereActor->GetComponent<TransformComponent>();
	//skySphereTransform->Scale = Vector3(20.0f, 20.0f, 20.0f);*/


	///*Actor* skyDomeActor = new Actor;
	//auto skyDomeComp = skyDomeActor->AddComponent<StaticMeshComponent>();
	//if (FBXLoader::GetInstance()->Load(L"../resource/FBX/CloudDome.fbx"))
	//{
	//	FBXLoader::GetInstance()->GenerateStaticMeshFromFileData(L"../resource/FBX/CloudDome.fbx", skyDomeComp);
	//}
	//Material* skyDomeMaterial = new Material;
	//skyDomeMaterial->PixelShaderCodeName = L"T_UI";
	//skyDomeMaterial->DiffuseTextureName = L"../resource/FBX/CloudSky2.png";
	//MaterialManager::GetInstance()->AddMaterial(L"SkyDome", skyDomeMaterial);
	//for (auto& mesh : skyDomeComp->Meshes)
	//{
	//	mesh.MaterialSlot = skyDomeMaterial;
	//}
	//MainWorld.AddEntity(skyDomeActor);
	//auto skyDomeTransform = skyDomeActor->GetComponent<TransformComponent>();
	//skyDomeTransform->Scale = Vector3(10.0f, 10.0f, 10.0f);*/

	//Actor* skyDomeActor = new Actor;
	//auto skyDomeComp = skyDomeActor->AddComponent<SkyDomeComponent>(); 
	//skyDomeComp->Scale = Vector3(500.0f, 500.0f, 500.0f);
	//MainWorld.AddEntity(skyDomeActor);
	//MainWorld.AddSystem(new SkyRenderSystem);


	//// 9. 메인 월드에 액터 추가.
	//MainWorld.AddEntity(actor);
	//MainWorld.AddEntity(cameraActor);
	////MainWorld.AddEntity(skyActor);

	//DirectionalLight* light = new DirectionalLight;
	//auto lightComp = light->GetComponent<DirectionalLightComponent>();
	//lightComp->Color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//lightComp->Direction = Vector4(1.0f, -1.0f, 1.0f, 1.0f);
	//MainWorld.AddEntity(light);

	//DirectionalLight* light2 = new DirectionalLight;
	//auto lightComp2 = light2->GetComponent<DirectionalLightComponent>();
	//lightComp2->Color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//lightComp2->Direction = Vector4(-1.0f, 1.0f, 1.0f, 1.0f);
	//MainWorld.AddEntity(light2);

	//// 10. 카메라 시스템 및 랜더링 시스템 추가.
	//LightSystem* lightSystem = new LightSystem;
	//lightSystem->Initialize();
	//MainWorld.AddSystem(lightSystem);
	//MainWorld.AddSystem(new CameraSystem);
	//MainWorld.AddSystem(new RenderSystem);
	//MainWorld.AddSystem(new MovementSystem);

	//// 추가
	//MainWorld.AddSystem(new UpdateAnimSystem);

	////Effect Test
	//ParticleEffect* testEffect1 = new ParticleEffect(L"Fire");
	//ParticleEffect* testEffect2 = new ParticleEffect(L"Smoke");
	//ParticleEffect* testEffect3 = new ParticleEffect(L"Spark");
	//auto testEffectTransform1 = testEffect1->GetComponent<TransformComponent>();
	//auto testEffectTransform2 = testEffect2->GetComponent<TransformComponent>();
	//auto testEffectTransform3 = testEffect3->GetComponent<TransformComponent>();
	//testEffectTransform1->Translation = { 10.0f, 0.0f, 0.0f };
	//testEffectTransform2->Translation = { 20.0f, 0.0f, 0.0f };
	//testEffectTransform3->Translation = { 30.0f, 0.0f, 0.0f };
	//MainWorld.AddEntity(testEffect1);
	//MainWorld.AddEntity(testEffect2);
	//MainWorld.AddEntity(testEffect3);

	return true;
}

bool SampleCore::Frame()
{
	EditorCore::Frame();
	SceneChange();
	CurrentScene->Frame();

	/*KeyState btnA = Input::GetInstance()->getKey('A');
	if (btnA == KeyState::Hold || btnA == KeyState::Down)
	{
		DebugCamera->Yaw -= 0.001f;
	}

	KeyState btnD = Input::GetInstance()->getKey('D');
	if (btnD == KeyState::Hold || btnD == KeyState::Down)
	{
		DebugCamera->Yaw += 0.001f;
	}

	KeyState btnW = Input::GetInstance()->getKey('W');
	if (btnW == KeyState::Hold || btnW == KeyState::Down)
	{
		DebugCamera->Pitch -= 0.001f;
	}

	KeyState btnS = Input::GetInstance()->getKey('S');
	if (btnS == KeyState::Hold || btnS == KeyState::Down)
	{
		DebugCamera->Pitch += 0.001f;
	}

	KeyState btnQ = Input::GetInstance()->getKey('Q');
	if (btnQ == KeyState::Hold || btnQ == KeyState::Down)
	{
		DebugCamera->Pos.z += 0.01f;
	}

	KeyState btnE = Input::GetInstance()->getKey('E');
	if (btnE == KeyState::Hold || btnE == KeyState::Down)
	{
		DebugCamera->Pos.z -= 0.01f;
	}*/

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
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/red/0.png"));
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/red/0.png");
		NumberTextureList_Red.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/red/1.png"));
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/red/1.png");
		NumberTextureList_Red.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/red/2.png"));
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/red/2.png");
		NumberTextureList_Red.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/red/3.png"));
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/red/3.png");
		NumberTextureList_Red.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/red/4.png"));
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/red/4.png");
		NumberTextureList_Red.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/red/5.png"));
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/red/5.png");
		NumberTextureList_Red.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/red/6.png"));
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/red/6.png");
		NumberTextureList_Red.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/red/7.png"));
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/red/7.png");
		NumberTextureList_Red.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/red/8.png"));
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/red/8.png");
		NumberTextureList_Red.push_back(tc);
	}
	if (DXTextureManager::GetInstance()->Load(L"../resource/UI/Number/red/9.png"));
	{
		DXTexture* tc = DXTextureManager::GetInstance()->GetTexture(L"../resource/UI/Number/red/9.png");
		NumberTextureList_Red.push_back(tc);
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