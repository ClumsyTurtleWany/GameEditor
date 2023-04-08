#include "DXRenderTargetManager.h"

bool DXRenderTargetManager::Initialize()
{
	return true;
}

bool DXRenderTargetManager::Release()
{
	for (auto& it : RenderTargetList)
	{
		if (it == nullptr)
		{
			continue;
		}

		it->Release();
		delete it;
		it = nullptr;
	}
	RenderTargetList.clear();

	return true;
}

DXRenderTarget* DXRenderTargetManager::Create(float x, float y, float width, float height, UINT count, DXGI_FORMAT format)
{
	DXRenderTarget* newRenderTarget = new DXRenderTarget;
	if (!newRenderTarget->Create(x, y, width, height, count, format))
	{
		newRenderTarget->Release();
		delete newRenderTarget;
		return nullptr;
	}

	RenderTargetList.push_back(newRenderTarget);

	return newRenderTarget;
}
