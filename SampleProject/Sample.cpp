#include "Sample.h"
#include "WindowsClient.h"
#include "RenderSystem.h"
#include "Actor.h"
#include "StaticMeshComponent.h"
#include "CameraSystem.h"
#include "PlaneComponent.h"

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
	// 1. Actor 생성
	Actor* actor = new Actor;
	
	// 2. Static Mesh Component 추가.
	auto comp = actor->AddComponent<StaticMeshComponent>();

	// 3. Material 생성
	Material* material = new Material;

	// 4. 텍스쳐 로드 및 머테리얼에 추가.
	DXTexture* texture = nullptr;
	if (DXTextureManager::getInstance()->Load(L"../resource/Default.bmp"))
	{
		texture = DXTextureManager::getInstance()->getTexture(L"../resource/Default.bmp");
	}
	if (texture != nullptr)
	{
		material->SetDiffuseTexture(texture);
	}

	// 5. 평면 메쉬 생성 후 머테리얼 세팅. 
	PlaneComponent* plane = new PlaneComponent;
	plane->SetMaterial(material);

	// 6. 스태틱 메쉬에 평면 메쉬 추가.
	comp->Meshes.push_back(*plane);
	
	// 7. 액터에 트랜스 폼 추가.
	auto transformComp = actor->AddComponent<TransformComponent>();
	transformComp->Translation = Vector3(0.0f, 0.0, 0.0f);
	transformComp->Rotation = Vector3(0.0f, 3.14f / 4.0f, 0.0f);
	transformComp->Scale = Vector3(20.0f, 20.0f, 1.0f);
	
	// 8. 액터에 카메라 추가.
	DebugCamera = actor->AddComponent<Camera>();
	DebugCamera->CreateViewMatrix(Vector3(0.0f, 0.0f, -100.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0, 0.0f));
	DebugCamera->CreateProjectionMatrix(1.0f, 500.0f, PI * 0.25, (Device.m_ViewPort.Width) / (Device.m_ViewPort.Height));

	// 9. 메인 월드에 액터 추가.
	MainWorld.AddEntity(actor);

	// 10. 카메라 시스템 및 랜더링 시스템 추가.
	MainWorld.AddSystem(new CameraSystem);
	MainWorld.AddSystem(new RenderSystem);

	return true;
}

bool SampleCore::Frame()
{
	
	return true;
}

bool SampleCore::Render()
{
	float dt = Timer::getInstance()->secondPerFrame;
	MainWorld.Tick(dt);
	return true;
}

bool SampleCore::Release()
{
	return true;
}
