#pragma once
#include "BaseCore\BaseCore.h"

class EditorCore : public BaseCore
{
public:
	virtual bool Initialize();
	virtual bool Frame();
	virtual bool Render();
	virtual bool Release();
};