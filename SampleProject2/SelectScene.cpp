#include "SelectScene.h"
#include "Service.h"
bool SelectScene::Init()
{
	UI_Loader Loader;

	// Actor ����
	Actor* UI = new Actor;
	auto wc = UI->AddComponent<WidgetComponent>();
	Loader.FileLoad(wc, L"../resource/UI/Save/Select.txt");

	SinglePlayButton = wc->FindObj(L"SinglePlay");
	MultiPlayButton = wc->FindObj(L"MultiPlay");
	RoomMakeButton = wc->FindObj(L"RoomMake");
	RoomFindButton = wc->FindObj(L"RoomFind");
	LoadingImage = wc->FindObj(L"Loading");
	CancleButton = wc->FindObj(L"Cancle");

	RoomMakeButton->m_bIsDead = true;
	RoomFindButton->m_bIsDead = true;
	CancleButton->m_bIsDead = true;
	LoadingImage->m_bIsDead = true;

	// ���� ���忡 ���� �߰�.
	TheWorld.AddEntity(UI);

	// ������ �ý��� �߰�.
	TheWorld.AddSystem(new WidgetRenderSystem);

	return true;
}

bool SelectScene::Frame()
{
	BaseScene::Frame();

	if (SinglePlayButton->m_bClicked)
	{
		SinglePlayButton->m_bClicked = false;
		SS = MAP;
	}

	else if (MultiPlayButton->m_bClicked)
	{
		MultiPlayButton->m_bClicked = false;

		SinglePlayButton->m_bIsDead = true;
		MultiPlayButton->m_bIsDead = true;

		RoomMakeButton->m_bIsDead = false;
		RoomFindButton->m_bIsDead = false;
	}

	// �� ����� ��ư
	else if (RoomMakeButton->m_bClicked)
	{
		RoomMakeButton->m_bClicked = false;
		RoomMakeButton->m_bIsDead = true;
		RoomFindButton->m_bIsDead = true;
		CancleButton->m_bIsDead = false;
		LoadingImage->m_bIsDead = false;

		if (gpHost == nullptr)
		{
			gpHost = new Host(L"127.0.0.1", 7777);
			gpHost->Init();
		}
		else
		{
			gpHost->GetService()->_listener->addAcceptEvent();
		}
	}

	// �� ã�� ��ư
	else if (RoomFindButton->m_bClicked)
	{
		RoomFindButton->m_bClicked = false;
		RoomMakeButton->m_bIsDead = true;
		RoomFindButton->m_bIsDead = true;
		CancleButton->m_bIsDead = false;
		LoadingImage->m_bIsDead = false;

		if (gpClient == nullptr)
		{
			gpClient = new Client(L"127.0.0.1", 7777);
			gpClient->Init();
		}
		else
			gpClient->GetClientservice()->Reconnect();
	}

	// ��� ��ư 
	else if (CancleButton->m_bClicked) 
	{
		CancleButton->m_bClicked = false;
		RoomMakeButton->m_bIsDead = false;
		RoomFindButton->m_bIsDead = false;
		CancleButton->m_bIsDead = true;
		LoadingImage->m_bIsDead = true;

		gpClient->CancelConnect();
	}



	// ���� ������ �� ����, �ϴ��� �ٷ� ��Ƽ ����������
	if (gpHost != nullptr && gpHost->IsConnected())
	{
		SS = MULTIBATTLE;
	}
	if (gpClient!=nullptr && gpClient->IsConnected())
	{
		SS = MULTIBATTLE;
	}

	return true;
}