#pragma once
#include "Define.h"

class DXUserInterface
{
private:
	friend class DXUserInterfaceManager;

protected:
	virtual bool Initialize() = 0;
	virtual bool Frame() = 0;
	virtual bool Render() = 0;
	virtual bool Release() = 0;
};