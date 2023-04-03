#include "CardViewScene.h"

bool CardViewScene::Init()
{
	ID = DECKVIEW; // ���..��.. ��������̱��ѵ�.. �Ƹ���

	UI_Loader Loader;

	// Actor ����
	Actor* UI = new Actor;
	//UI->Name = L"CardView";
	auto bc = UI->AddComponent<WidgetComponent>();
	Loader.FileLoad(bc, L"../resource/UI/Save/CardView.txt");

	// ī�� ������Ʈ���� ����Ʈ�� �־���
	CardList[0] = bc->FindObj(L"Card1");
	CardList[1] = bc->FindObj(L"Card2");
	CardList[2] = bc->FindObj(L"Card3");
	CardList[3] = bc->FindObj(L"Card4");
	CardList[4] = bc->FindObj(L"Card5");
	CardList[5] = bc->FindObj(L"Card6");
	CardList[6] = bc->FindObj(L"Card7");
	CardList[7] = bc->FindObj(L"Card8");
	CardList[8] = bc->FindObj(L"Card9");
	CardList[9] = bc->FindObj(L"Card10");

	BackButton = bc->FindObj(L"Back");

	// ���� ���忡 ���� �߰�.
	TheWorld.AddEntity(UI);

	// ������ �ý��� �߰�.
	TheWorld.AddSystem(new WidgetRenderSystem);

	return true;
}

bool CardViewScene::Frame()
{
	BaseScene::Frame();
	return true;
}

bool CardViewScene::Render()
{
	BaseScene::Render();
	if (BackButton->m_bClicked) { SS = BeforeScene; BackButton->m_bClicked = false; }
	return true;
}

bool CardViewScene::Release()
{
	return true;
}

void CardViewScene::Update(viewState vs)
{
	switch (vs) 
	{
	case deck: 
	{
		for (int card = 0; card < Dick->DeckList.size(); card++)
		{
			for (int ci = 0; ci < 4; ci++)
			{
				CardList[card]->m_pCutInfoList[ci]->tc = CardTextureList[Dick->DeckList[card]];
			}
			CardList[card]->m_bIsDead = false;
		}

		for (int card = Dick->DeckList.size(); card < 10; card++) // for (int card = handSize; card < CardList.size(); card++)
		{
			CardList[card]->m_bIsDead = true;
		}
	}break;

	case remain: 
	{
		for (int card = 0; card < Dick->RemainingCardList.size(); card++)
		{
			for (int ci = 0; ci < 4; ci++)
			{
				CardList[card]->m_pCutInfoList[ci]->tc = CardTextureList[Dick->RemainingCardList[card]];
			}
			CardList[card]->m_bIsDead = false;
		}

		for (int card = Dick->RemainingCardList.size(); card < 10; card++) // for (int card = handSize; card < CardList.size(); card++)
		{
			CardList[card]->m_bIsDead = true;
		}
	}break;

	case discard: 
	{
		for (int card = 0; card < Dick->DiscardList.size(); card++)
		{
			for (int ci = 0; ci < 4; ci++)
			{
				CardList[card]->m_pCutInfoList[ci]->tc = CardTextureList[Dick->DiscardList[card]];
			}
			CardList[card]->m_bIsDead = false;
		}

		for (int card = Dick->DiscardList.size(); card < 10; card++) // for (int card = handSize; card < CardList.size(); card++)
		{
			CardList[card]->m_bIsDead = true;
		}
	}break;
	}
}

