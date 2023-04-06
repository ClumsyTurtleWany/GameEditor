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
	RoomMakeButton->m_bIsDead = true;
	RoomFindButton = wc->FindObj(L"RoomFind");
	RoomFindButton->m_bIsDead = true;

	// ���� ���忡 ���� �߰�.
	TheWorld.AddEntity(UI);

	// ������ �ý��� �߰�.
	TheWorld.AddSystem(new WidgetRenderSystem);

	return true;
}

bool SelectScene::Frame()
{
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

	else if (RoomMakeButton->m_bClicked)
	{
		RoomMakeButton->m_bClicked = false;
		// �� ����� ��ư ������ ��� ��Ⱑ ����

		if (gpHost == nullptr)
		{
			gpHost = new Host(L"127.0.0.1", 7777);
			gpHost->Init();
		}
		else
		{
			gpHost->GetService()->_listener->addAcceptEvent();
		}
		gpHost->CancelAccept();
	}

	else if (RoomFindButton->m_bClicked)
	{
		RoomFindButton->m_bClicked = false;
		// �� ã�� ��ư ������ ��� ��Ⱑ ����
		if (gpClient == nullptr)
		{
			gpClient = new Client(L"192.168.0.93", 7777);
			gpClient->Init();
		}
		else
			gpClient->GetClientservice()->Reconnect();
		gpClient->CancelConnect();
	}

	BaseScene::Frame();
	return true;
}