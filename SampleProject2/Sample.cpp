#include "Sample.h"
#include "WindowsClient.h"
#include "Actor.h"
#include "PlaneComponent.h"
#include "CameraSystem.h"
#include "RenderSystem.h"
#include "LightSystem.h"
#include "DirectionalLight.h"
#include "SkyBoxComponent.h"
#include "FBXLoader.hpp"

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
}

SampleCore::~SampleCore()
{
}

bool SampleCore::Initialize()
{
	EditorCore::Initialize();
	FBXLoader::GetInstance()->Initialize();


	// 1. Actor 생성
	Actor* actor = new Actor;

	// 2. Static Mesh Component 추가.
	auto comp = actor->AddComponent<StaticMeshComponent>();

	// 3. Material 생성
	Material* material = new Material;

	// 4. 텍스쳐 로드 및 머테리얼에 추가.
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



	// 5. 평면 메쉬 생성 후 머테리얼 세팅. 
	PlaneComponent* plane = new PlaneComponent;
	plane->SetMaterial(material);
	plane->CalcTangent();

	// 6. 스태틱 메쉬에 평면 메쉬 추가.
	comp->Meshes.push_back(*plane);

	// 7. 액터에 트랜스 폼 추가.
	auto transformComp = actor->AddComponent<TransformComponent>();
	transformComp->Translation = Vector3(0.0f, 0.0, 100.0f);
	//transformComp->Rotation = Vector3(0.0f, 3.14f / 4.0f, 0.0f);
	transformComp->Rotation = Vector3(0.0f, 0.0f, 0.0f);
	transformComp->Scale = Vector3(20.0f, 20.0f, 1.0f);

	// 8. 액터에 카메라 추가.
	Actor* cameraActor = new Actor;
	DebugCamera = cameraActor->AddComponent<Camera>();
	DebugCamera->CreateViewMatrix(Vector3(0.0f, 0.0f, -100.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0, 0.0f));
	DebugCamera->CreateProjectionMatrix(1.0f, 10000.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));

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
	


	// Fbx Loader Test
	Actor* fbxActor = new Actor;
	auto fbxMeshComp = fbxActor->AddComponent<StaticMeshComponent>();
	FBXLoader::GetInstance()->Load(L"../resource/FBX/charMob.FBX", fbxMeshComp);
	MainWorld.AddEntity(fbxActor);


	// 9. 메인 월드에 액터 추가.
	MainWorld.AddEntity(actor);
	MainWorld.AddEntity(cameraActor);
	MainWorld.AddEntity(skyActor);

	DirectionalLight* light = new DirectionalLight;
	auto lightComp = light->GetComponent<DirectionalLightComponent>();
	lightComp->Color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	lightComp->Direction = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	MainWorld.AddEntity(light);

	// 10. 카메라 시스템 및 랜더링 시스템 추가.
	LightSystem* lightSystem = new LightSystem;
	lightSystem->Initialize();
	MainWorld.AddSystem(lightSystem);
	MainWorld.AddSystem(new CameraSystem);
	MainWorld.AddSystem(new RenderSystem);

	return true;
}

bool SampleCore::Frame()
{
	EditorCore::Frame();

	KeyState btnA = Input::GetInstance()->getKey('A');
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
	}

	return true;
}

bool SampleCore::Render()
{
	EditorCore::Render();

	float dt = Timer::GetInstance()->SecondPerFrame;
	MainWorld.Tick(dt);
	return true;
}

bool SampleCore::Release()
{
	FBXLoader::GetInstance()->Release();
	return EditorCore::Release();
}
