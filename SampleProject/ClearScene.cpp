#include "ClearScene.h"

bool ClearScene::Init()
{
	UI_Loader Loader;

	// Actor ����
	Actor* actor = new Actor;
	auto tc = actor->AddComponent<WidgetComponent>();
	Loader.FileLoad(tc, L"../resource/UI/Save/Clear.txt");

	// ���� ���忡 ���� �߰�.
	TheWorld.AddEntity(actor);

	// ������ �ý��� �߰�.
	TheWorld.AddSystem(new WidgetRenderSystem);

	return true;
}
