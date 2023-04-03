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

	// 메인 월드에 액터 추가.
	TheWorld.AddEntity(TitleUI);

	// 랜더링 시스템 추가.
	TheWorld.AddSystem(new WidgetRenderSystem);

	return true;
}

bool TitleScene::Frame()
{
	if (StartButton->m_bClicked) 
	{ 
		StartButton->m_bClicked = false;
		SS = MULTISELECT;
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
