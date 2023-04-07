#include "Sample.h"

#include "SkyRenderSystem.h"
#include "SkyDomeComponent.h"
#include "FBXLoader.hpp"
#include "MaterialManager.h"
#include "LightSystem.h"
#include "DirectionalLight.h"
#include "SkyBoxComponent.h"
#include "FBXLoader.hpp"
#include "Landscape.h"
#include "SkeletalMeshComponent.h"
#include "MaterialManager.h"
//추가
#include "SocketComponent.h"
#include "AnimationComponent.h"
#include "UpdateAnimSystem.h"
#include "MovementSystem.h"
#include "Character.h"
#include "SelectAnimSystem.h"
#include "BoundingBoxComponent.h"
#include "CameraArmComponent.h"

//추가
#include "ColliderSystem.h"
#include "SplineComponent.h"

///////////////////
//Effect Include
///////////////////
#include "EffectInclude/EffectSystem.h"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	WindowsClient sampleWindow;
	sampleWindow.Create(hInstance, L"Window Name", 1024, 768);

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
	toggleCam = false;
}

SampleCore::~SampleCore()
{
}

bool SampleCore::Initialize()
{
	EditorCore::Initialize();
	FBXLoader::GetInstance()->Initialize();

	MainCameraSystem = new CameraSystem;
	MainCameraActor = new Actor;
	MainCamera = MainCameraActor->AddComponent<Camera>();
	MainCamera->CreateViewMatrix(Vector3(0.0f, 50.0f, -70.0f), Vector3(150.0f, 50.0f, 50.0f), Vector3(0.0f, 1.0, 0.0f));
	MainCamera->CreateProjectionMatrix(1.0f, 10000.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));
	MainCameraSystem->MainCamera = MainCamera;
	MainWorld.AddEntity(MainCameraActor);

	SubCameraActor = new Actor;
	SubCamera = SubCameraActor->AddComponent<Camera>();

	TRANSFORM_KEY p0(0.0f, { -300.0f, 20.0f, 0.0f }, { 0.0f, -90.0f, 0.0f });
	TRANSFORM_KEY p1(5.0f, { -100.0f, 40.0f, 120.0f }, { 0.0f, -45.0f, 0.0f });
	TRANSFORM_KEY p2(10.0f, { 100.0f, 120.0f, 240.0f }, { 0.0f, 0.0f, 0.0f });
	TRANSFORM_KEY p3(15.0f, { 20.0f, 80.0f, -120.0f }, { 0.0f, 45.0f, 0.0f });

	std::vector<TRANSFORM_KEY> CPList;
	CPList.push_back(p0);
	CPList.push_back(p1);
	CPList.push_back(p2);
	CPList.push_back(p3);
	SubCamera->CreateViewMatrix(Vector3(0.0f, 50.0f, -70.0f), Vector3(150.0f, 50.0f, 50.0f), Vector3(0.0f, 1.0, 0.0f));
	SubCamera->CreateProjectionMatrix(1.0f, 10000.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));

	auto Spline = SubCameraActor->AddComponent<SplineComponent>(SPLINE_LOOP_OPT::SPLOPT_LOOP_ONEWAY, CPList.data(), CPList.size(), true, -1.0f);
	MainWorld.AddEntity(SubCameraActor);

	Spline->start();

	//// 지형 액터 추가.
	Landscape* landscape = new Landscape;
	auto landscapeComponents = landscape->GetComponent<LandscapeComponents>();
	landscapeComponents->Build(16, 16, 7, 10);
	landscapeComponents->SetCamera(MainCameraSystem->MainCamera);
	MainWorld.AddEntity(landscape);

	// Sky Dome 추가.
	Actor* skyDomeActor = new Actor;
	auto skyDomeComp = skyDomeActor->AddComponent<SkyDomeComponent>();
	skyDomeComp->Scale = Vector3(5000.0f, 5000.0f, 5000.0f);
	MainWorld.AddEntity(skyDomeActor);
	SkyRenderSystem* skyRenderSystem = new SkyRenderSystem;
	skyRenderSystem->MainCamera = MainCameraSystem->MainCamera;
	MainWorld.AddSystem(skyRenderSystem);

	DirectionalLight* light = new DirectionalLight;
	auto lightComp = light->GetComponent<DirectionalLightComponent>();
	lightComp->Color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	lightComp->Direction = Vector4(1.0f, -1.0f, 1.0f, 1.0f);
	MainWorld.AddEntity(light);

	DirectionalLight* light2 = new DirectionalLight;
	auto lightComp2 = light2->GetComponent<DirectionalLightComponent>();
	lightComp2->Color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	lightComp2->Direction = Vector4(-1.0f, 1.0f, 1.0f, 1.0f);
	MainWorld.AddEntity(light2);

	for (int cnt = 0; cnt < 4; cnt++)
	{
		Actor* backgroundBuliding = new Actor;
		auto backgroundBulidingStaticMesh = backgroundBuliding->AddComponent<StaticMeshComponent>();
		auto boundBox = backgroundBuliding->AddComponent<BoundingBoxComponent>(Vector3(1.2f, 0.5f, 1.0f));
		if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Map/Warehouse/Warehouse.FBX"))
		{
			FBXLoader::GetInstance()->GenerateStaticMeshFromFileData(L"../resource/FBX/Map/Warehouse/Warehouse.FBX", backgroundBulidingStaticMesh);
		}
		auto backgroundBulidingTransform = backgroundBuliding->GetComponent<TransformComponent>();
		backgroundBulidingTransform->Scale = Vector3(100.0f, 100.0f, 100.0f);
		backgroundBulidingTransform->Rotation = Vector3(0.0f, 0.0f, 0.0f);
		backgroundBulidingTransform->Translation = Vector3(-500.0f + static_cast<float>(cnt) * 250, 40.0f, 300.0f);
		MainWorld.AddEntity(backgroundBuliding);
	}

	for (int cnt = 0; cnt < 4; cnt++)
	{
		Actor* backgroundBuliding = new Actor;
		auto backgroundBulidingStaticMesh = backgroundBuliding->AddComponent<StaticMeshComponent>();
		auto boundBox = backgroundBuliding->AddComponent<BoundingBoxComponent>(Vector3(1.2f, 0.5f, 1.0f));
		if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Map/Warehouse/Warehouse.FBX"))
		{
			FBXLoader::GetInstance()->GenerateStaticMeshFromFileData(L"../resource/FBX/Map/Warehouse/Warehouse.FBX", backgroundBulidingStaticMesh);
		}
		auto backgroundBulidingTransform = backgroundBuliding->GetComponent<TransformComponent>();
		backgroundBulidingTransform->Scale = Vector3(100.0f, 100.0f, 100.0f);
		backgroundBulidingTransform->Rotation = Vector3(0.0f, 0.0f, 0.0f);
		backgroundBulidingTransform->Translation = Vector3(-500.0f + static_cast<float>(cnt) * 250, 40.0f, -300.0f);
		MainWorld.AddEntity(backgroundBuliding);
	}

	for (int cnt = 0; cnt < 8; cnt++)
	{
		Actor* container = new Actor;
		auto staticMesh = container->AddComponent<StaticMeshComponent>();
		auto boundBox = container->AddComponent<BoundingBoxComponent>(Vector3(150.0f, 250.0f, 300.0f));
		if (FBXLoader::GetInstance()->Load(L"../resource/FBX/Map/Container/Shipping_Container_A_-_Model.FBX"))
		{
			FBXLoader::GetInstance()->GenerateStaticMeshFromFileData(L"../resource/FBX/Map/Container/Shipping_Container_A_-_Model.FBX", staticMesh);
		}
		auto transform = container->GetComponent<TransformComponent>();
		transform->Scale = Vector3(0.2f, 0.2f, 0.2f);
		transform->Rotation = Vector3(0.0f, 0.0f, 0.0f);
		transform->Translation = Vector3(500.0f, 0.0f, -500.0f + static_cast<float>(cnt) * 125);
		MainWorld.AddEntity(container);
	}

	// 카메라 시스템 및 랜더링 시스템 추가.
	MainWorld.AddSystem(MainCameraSystem);
	MainWorld.AddSystem(new ColliderSystem);
	MainRenderSystem = new RenderSystem;
	MainRenderSystem->MainCamera = MainCameraSystem->MainCamera;
	MainWorld.AddSystem(MainRenderSystem);

	MainWorld.AddSystem(new MovementSystem);
	MainWorld.AddSystem(new UpdateAnimSystem);
	// SelectAnimSystem 추가
	MainWorld.AddSystem(new SelectAnimSystem);

	LightSystem* lightSystem = new LightSystem;
	lightSystem->MainCamera = MainCameraSystem->MainCamera;
	lightSystem->Initialize();
	MainWorld.AddSystem(lightSystem);

	return true;
}

bool SampleCore::Frame()
{
	if (Input::GetInstance()->getKey(VK_F2) == KeyState::Down)
	{
		swaping(MainCamera, SubCamera, temp);

		MainCameraSystem->MainCamera = MainCamera;
		MainRenderSystem->MainCamera = MainCamera;
	}

	return true;
}

bool SampleCore::Render()
{
	float dt = Timer::GetInstance()->SecondPerFrame;
	MainWorld.Tick(dt);
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
