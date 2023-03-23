#include "TitleScene.h"

bool TitleScene::Init()
{
	UI_Loader Loader;

	// Actor 생성
	Actor* TitleUI = new Actor;
	//TitleUI->Name = L"Title";
	auto tc = TitleUI->AddComponent<WidgetComponent>();
	Loader.FileLoad(tc, L"../resource/UI/Save/Title_1.txt");
	StartButton = tc->FindObj(L"Button_Start");


	// 액터에 카메라 추가.
	MainCamera = TitleUI->AddComponent<Camera>();
	MainCamera->CreateViewMatrix(Vector3(0.0f, 0.0f, -100.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0, 0.0f));
	MainCamera->CreateProjectionMatrix(1.0f, 500.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));

	// 메인 월드에 액터 추가.
	TheWorld.AddEntity(TitleUI);

	// 카메라 시스템 및 랜더링 시스템 추가.
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
