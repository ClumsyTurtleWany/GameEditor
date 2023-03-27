#include "GameOverScene.h"

bool GameOverScene::Init()
{
	UI_Loader Loader;

	// Actor ����
	Actor* actor = new Actor;
	auto tc = actor->AddComponent<WidgetComponent>();
	Loader.FileLoad(tc, L"../resource/UI/Save/GameOver.txt");

	// ���� ���忡 ���� �߰�.
	TheWorld.AddEntity(actor);

	// �ý��� �߰�.
	TheWorld.AddSystem(new WidgetRenderSystem);

	return true;
}
