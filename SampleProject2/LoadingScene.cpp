#include "LoadingScene.h"

bool LoadingScene::Init()
{
	UI_Loader Loader;

	// Actor 생성
	Actor* Loading = new Actor;
	//Loading->Name = L"Loading";
	auto tc = Loading->AddComponent<WidgetComponent>();
	Loader.FileLoad(tc, L"../resource/UI/Save/Loading.txt");

	// 메인 월드에 액터 추가.
	TheWorld.AddEntity(Loading);

	TheWorld.AddSystem(new WidgetRenderSystem);
	return true;
}

bool LoadingScene::Frame()
{
	BaseScene::Frame();
	return true;
}

bool LoadingScene::Render()
{
	BaseScene::Render();
	return true;
}

bool LoadingScene::Release()
{
	return true;
}
