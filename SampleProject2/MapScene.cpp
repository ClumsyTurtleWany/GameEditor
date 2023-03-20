#include "MapScene.h"

bool MapScene::Init()
{
	ID = map;

	UI_Loader Loader;

	// Actor ����
	Actor* MapUI = new Actor;
	//MapUI->Name = L"Map";
	auto mc = MapUI->AddComponent<WidgetComponent>();
	Loader.FileLoad(mc, L"../resource/UI/Save/Map.txt");
	BackButton = mc->FindObj(L"Back");
	Mon1 = mc->FindObj(L"Mon1");

	// ���Ϳ� ī�޶� �߰�.
	MainCamera = MapUI->AddComponent<Camera>();
	MainCamera->CreateViewMatrix(Vector3(0.0f, 0.0f, -100.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0, 0.0f));
	MainCamera->CreateProjectionMatrix(1.0f, 500.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));

	// ���� ���忡 ���� �߰�.
	TheWorld.AddEntity(MapUI);

	// ī�޶� �ý��� �� ������ �ý��� �߰�.
	TheWorld.AddSystem(new CameraSystem);
	TheWorld.AddSystem(new RenderSystem);
	TheWorld.AddSystem(new WidgetRenderSystem);

	return true;
}

bool MapScene::Frame()
{
	if (Mon1->m_bClicked) 
	{
		Mon1->m_bClicked = false;
		SS = battle;
	}

	BaseScene::Frame();
	return true;
}

bool MapScene::Render()
{
	BaseScene::Render();
	return true;
}

bool MapScene::Release()
{
	return true;
}
