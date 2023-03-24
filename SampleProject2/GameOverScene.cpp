#include "GameOverScene.h"

bool GameOverScene::Init()
{
	UI_Loader Loader;

	// Actor ����
	Actor* actor = new Actor;
	auto tc = actor->AddComponent<WidgetComponent>();
	Loader.FileLoad(tc, L"../resource/UI/Save/GameOver.txt");

	// ���Ϳ� ī�޶� �߰�.
	MainCamera = actor->AddComponent<Camera>();
	MainCamera->CreateViewMatrix(Vector3(0.0f, 0.0f, -100.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0, 0.0f));
	MainCamera->CreateProjectionMatrix(1.0f, 500.0f, PI * 0.25, (DXDevice::g_ViewPort.Width) / (DXDevice::g_ViewPort.Height));

	// ���� ���忡 ���� �߰�.
	TheWorld.AddEntity(actor);

	// ī�޶� �ý��� �� ������ �ý��� �߰�.
	TheWorld.AddSystem(new CameraSystem);
	TheWorld.AddSystem(new RenderSystem);
	TheWorld.AddSystem(new WidgetRenderSystem);

	return true;
}
