#pragma once
#include "Define.h"
#include "DXRenderTarget.hpp"

class DXRenderTargetManager : public Singleton<DXRenderTargetManager>
{
private:
	friend class Singleton<DXRenderTargetManager>;

	std::vector<DXRenderTarget*> RenderTargetList;

public:
	virtual bool Initialize();
	virtual bool Release();

public:
	DXRenderTarget* Create(float x, float y, float width, float height, UINT count = 1, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
};