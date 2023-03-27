#include "ClearScene.h"

bool ClearScene::Init()
{
	UI_Loader Loader;

	// Actor 생성
	Actor* actor = new Actor;
	auto tc = actor->AddComponent<WidgetComponent>();
	Loader.FileLoad(tc, L"../resource/UI/Save/Clear.txt");

	// 메인 월드에 액터 추가.
	TheWorld.AddEntity(actor);

	// 랜더링 시스템 추가.
	TheWorld.AddSystem(new WidgetRenderSystem);

	return true;
}
