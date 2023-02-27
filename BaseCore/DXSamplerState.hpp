#pragma once
#include "Define.h"
#include "DXDevice.hpp"

class DXSamplerState
{
public:
	static ID3D11SamplerState* pDefaultSamplerState;
	static ID3D11SamplerState* pDefaultMirrorSamplerState;
	static ID3D11RasterizerState* pDefaultRSWireFrame;
	static ID3D11RasterizerState* pRSWireFrame_CullNone;
	static ID3D11RasterizerState* pRSWireFrame_CullFront;
	static ID3D11RasterizerState* pDefaultRSSolid;
	static ID3D11RasterizerState* pRSSolid_CullNone;
	static ID3D11RasterizerState* pRSSolid_CullFront;
	static ID3D11BlendState* pBlendSamplerState; // 투명도 조절

	static ID3D11DepthStencilState* pDefaultDepthStencil; // Less Equal
	static ID3D11DepthStencilState* pGreaterDepthStencil; // Greater

public:
	static bool SetDevice(ID3D11Device* _pd3dDevice);
	static bool Release();
};
