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
	RoomMakeButton = wc->FindObj(L"RoomMake");
	RoomMakeButton->m_bIsDead = true;
	RoomFindButton = wc->FindObj(L"RoomFind");
	RoomFindButton->m_bIsDead = true;

	// 메인 월드에 액터 추가.
	TheWorld.AddEntity(UI);

	// 랜더링 시스템 추가.
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

	else if (MultiPlayButton->m_bClicked)
	{
		MultiPlayButton->m_bClicked = false;

		SinglePlayButton->m_bIsDead = true;
		MultiPlayButton->m_bIsDead = true;

		RoomMakeButton->m_bIsDead = false;
		RoomFindButton->m_bIsDead = false;
	}

	else if (RoomMakeButton->m_bClicked)
	{
		RoomMakeButton->m_bClicked = false;
		// 방 만들기 버튼 눌렀을 경우 요기가 실행
	}

	else if (RoomFindButton->m_bClicked)
	{
		RoomFindButton->m_bClicked = false;
		// 방 찾기 버튼 눌렀을 경우 요기가 실행
	}

	BaseScene::Frame();
	return true;
}
