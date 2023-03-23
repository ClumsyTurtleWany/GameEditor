#include "Sample.h"
#include "WindowsClient.h"
#include "RenderSystem.h"
#include "Actor.h"
#include "StaticMeshComponent.h"
#include "CameraSystem.h"
#include "PlaneComponent.h"
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

	//Effect Test
	ECS::EffectSystem* ESystem = new ECS::EffectSystem;
	ESystem->init(&MainWorld);
	MainWorld.AddSystem(ESystem);

	ParticleEffect* testEffect1 = new ParticleEffect(L"Fire", { true, 10.0, 0.0, 1.0 });
	/*ParticleEffect* testEffect2 = new ParticleEffect(L"Hit2", { true, 0.5, 0.0, 1.0 });
	ParticleEffect* testEffect3 = new ParticleEffect(L"Hit3", { true, 0.5, 0.0, 1.0 });
	ParticleEffect* testEffect4 = new ParticleEffect(L"Hit4", { true, 0.5, 0.0, 1.0 });
	ParticleEffect* testEffect5 = new ParticleEffect(L"Hit5", { true, 0.5, 0.0, 1.0 });*/

	auto testEffectTransform1 = testEffect1->GetComponent<TransformComponent>();
	/*auto testEffectTransform2 = testEffect2->GetComponent<TransformComponent>();
	auto testEffectTransform3 = testEffect3->GetComponent<TransformComponent>();
	auto testEffectTransform4 = testEffect1->GetComponent<TransformComponent>();
	auto testEffectTransform5 = testEffect2->GetComponent<TransformComponent>();*/

	testEffectTransform1->Translation = { 10.0f, 0.0f, 0.0f };
	/*testEffectTransform2->Translation = { 20.0f, 0.0f, 0.0f };
	testEffectTransform3->Translation = { 30.0f, 0.0f, 0.0f };
	testEffectTransform4->Translation = { 40.0f, 0.0f, 0.0f };
	testEffectTransform5->Translation = { 50.0f, 0.0f, 0.0f };*/

	MainWorld.AddEntity(testEffect1);
	/*TheWorld.AddEntity(testEffect2);
	TheWorld.AddEntity(testEffect3);
	TheWorld.AddEntity(testEffect4);
	TheWorld.AddEntity(testEffect5);*/

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
