#include "SelectScene.h"

bool SelectScene::Init()
{
	UI_Loader Loader;

	// Actor 생성
	Actor* UI = new Actor;
	auto wc = UI->AddComponent<WidgetComponent>();
	Loader.FileLoad(wc, L"../resource/UI/Save/Select.txt");
	SinglePlayButton = wc->FindObj(L"SinglePlay");
	MultiPlayButton = wc->FindObj(L"MultiPlay");

	// 액터에 카메라 추가.
	//MainCamera = TitleUI->AddComponent<Camera>();
	//MainCamera->CreateViewMatrix(Vector3(0.0f, 0.0f, -100.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0, 0.0f));
	//MainCamera->CreateProjectionMatrix(1.0f, 500.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));

	// 메인 월드에 액터 추가.
	TheWorld.AddEntity(UI);

	// 카메라 시스템 및 랜더링 시스템 추가.
	//TheWorld.AddSystem(new CameraSystem);
	//TheWorld.AddSystem(new RenderSystem);
	TheWorld.AddSystem(new WidgetRenderSystem);

	return true;
}

bool SelectScene::Frame()
{
	if (SinglePlayButton->m_bClicked)
	{
		SinglePlayButton->m_bClicked = false;
		SS = map;
	}

	if (MultiPlayButton->m_bClicked)
	{
		MultiPlayButton->m_bClicked = false;

		/* 멀티플레이 버튼이 눌렸다면
		   여기 내용이 실행됩니다 */
	}

	BaseScene::Frame();
	return true;
}
