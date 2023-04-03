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

	// ���� ���忡 ���� �߰�.
	TheWorld.AddEntity(TitleUI);

	// ������ �ý��� �߰�.
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
