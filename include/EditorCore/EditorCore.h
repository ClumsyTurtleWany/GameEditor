#pragma once
#include "Define.h"

class EditorCore : public BaseCore
{
public:
	EditorCore();
	virtual ~EditorCore();

protected:
	virtual bool Initialize() override;
	virtual bool Frame() override;
	virtual bool Render() override;
	virtual bool Release() override;
};