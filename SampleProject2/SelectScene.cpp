#include "SelectScene.h"

bool SelectScene::Init()
{
	UI_Loader Loader;

	// Actor ����
	Actor* UI = new Actor;
	auto wc = UI->AddComponent<WidgetComponent>();
	Loader.FileLoad(wc, L"../resource/UI/Save/Select.txt");
	SinglePlayButton = wc->FindObj(L"SinglePlay");
	MultiPlayButton = wc->FindObj(L"MultiPlay");

	// ���Ϳ� ī�޶� �߰�.
	//MainCamera = TitleUI->AddComponent<Camera>();
	//MainCamera->CreateViewMatrix(Vector3(0.0f, 0.0f, -100.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0, 0.0f));
	//MainCamera->CreateProjectionMatrix(1.0f, 500.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));

	// ���� ���忡 ���� �߰�.
	TheWorld.AddEntity(UI);

	// ī�޶� �ý��� �� ������ �ý��� �߰�.
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

		/* ��Ƽ�÷��� ��ư�� ���ȴٸ�
		   ���� ������ ����˴ϴ� */
	}

	BaseScene::Frame();
	return true;
}
