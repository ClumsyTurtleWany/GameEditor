#include "TitleScene.h"

bool TitleScene::Init()
{
	UI_Loader Loader;

	// Actor ����
	Actor* TitleUI = new Actor;
	//TitleUI->Name = L"Title";
	auto tc = TitleUI->AddComponent<WidgetComponent>();
	Loader.FileLoad(tc, L"../resource/UI/Save/Title_1.txt");
	StartButton = tc->FindObj(L"Button_Start");


	// ���Ϳ� ī�޶� �߰�.
	MainCamera = TitleUI->AddComponent<Camera>();
	MainCamera->CreateViewMatrix(Vector3(0.0f, 0.0f, -100.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0, 0.0f));
	MainCamera->CreateProjectionMatrix(1.0f, 500.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));

	// ���� ���忡 ���� �߰�.
	TheWorld.AddEntity(TitleUI);

	// ī�޶� �ý��� �� ������ �ý��� �߰�.
	TheWorld.AddSystem(new CameraSystem);
	TheWorld.AddSystem(new RenderSystem);
	TheWorld.AddSystem(new WidgetRenderSystem);

	return true;
}

bool TitleScene::Frame()
{
	if (StartButton->m_bClicked) 
	{ 
		StartButton->m_bClicked = false;
		SS = map; 
	}
	BaseScene::Frame();
	return true;
}

bool TitleScene::Render()
{
	BaseScene::Render();
	return true;
}

bool TitleScene::Release()
{
	return true;
}
