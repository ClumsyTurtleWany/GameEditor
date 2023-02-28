#pragma once
#include "Define.h"

class EditorCore : public BaseCore
{
protected:
	virtual bool Initialize() override;
	virtual bool Frame() override;
	virtual bool Render() override;
	virtual bool Release() override;
};