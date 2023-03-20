#pragma once
#include "BaseScene.h"

class MapScene : public BaseScene
{
public:
	virtual bool Init() override;
	virtual bool Frame() override;
	virtual bool Render() override;
	virtual bool Release() override;

public:
	WidgetObject* Mon1;
	WidgetObject* BackButton;
};

