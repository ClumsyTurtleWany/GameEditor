#pragma once
#include "BaseScene.h"

enum viewState
{
	deck,
	remain,
	discard,
};

class CardViewScene : public BaseScene
{
public:
	virtual bool Init() override;
	virtual bool Frame() override;
	virtual bool Render() override;
	virtual bool Release() override;

public:
	void Update(viewState vs);

public:
	Deck* Dick;
	std::vector<DXTexture*> CardTextureList;
	WidgetObject* CardList[10] = { 0, };
	WidgetObject* BackButton;

	SceneState BeforeScene;
};

