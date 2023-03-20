#pragma once
#include "BaseScene.h"

class TitleScene : public BaseScene
{
public:
	virtual bool Init() override;
	virtual bool Frame() override;
	virtual bool Render() override;
	virtual bool Release() override;

public:
	WidgetObject* StartButton;
	WidgetObject* SettingButton;
};

