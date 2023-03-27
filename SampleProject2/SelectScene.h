#pragma once
#include "BaseScene.h"
class SelectScene : public BaseScene
{
public:
	virtual bool Init() override;
	virtual bool Frame() override;

public:
	WidgetObject* SinglePlayButton;
	WidgetObject* MultiPlayButton;
};

