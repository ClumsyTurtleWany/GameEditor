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
	// 1. Actor ����
	Actor* actor = new Actor;
	
	// 2. Static Mesh Component �߰�.
	auto comp = actor->AddComponent<StaticMeshComponent>();

	// 3. Material ����
	Material* material = new Material;

	// 4. �ؽ��� �ε� �� ���׸��� �߰�.
	DXTexture* texture = nullptr;
	if (DXTextureManager::getInstance()->Load(L"../resource/Default.bmp"))
	{
		texture = DXTextureManager::getInstance()->getTexture(L"../resource/Default.bmp");
	}
	if (texture != nullptr)
	{
		material->SetDiffuseTexture(texture);
	}

	// 5. ��� �޽� ���� �� ���׸��� ����. 
	PlaneComponent* plane = new PlaneComponent;
	plane->SetMaterial(material);

	// 6. ����ƽ �޽��� ��� �޽� �߰�.
	comp->Meshes.push_back(*plane);
	
	// 7. ���Ϳ� Ʈ���� �� �߰�.
	auto transformComp = actor->AddComponent<TransformComponent>();
	transformComp->Translation = Vector3(0.0f, 0.0, 0.0f);
	transformComp->Rotation = Vector3(0.0f, 3.14f / 4.0f, 0.0f);
	transformComp->Scale = Vector3(20.0f, 20.0f, 1.0f);
	
	// 8. ���Ϳ� ī�޶� �߰�.
	DebugCamera = actor->AddComponent<Camera>();
	DebugCamera->CreateViewMatrix(Vector3(0.0f, 0.0f, -100.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0, 0.0f));
	DebugCamera->CreateProjectionMatrix(1.0f, 500.0f, PI * 0.25, (Device.m_ViewPort.Width) / (Device.m_ViewPort.Height));

	// 9. ���� ���忡 ���� �߰�.
	MainWorld.AddEntity(actor);

	// 10. ī�޶� �ý��� �� ������ �ý��� �߰�.
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
