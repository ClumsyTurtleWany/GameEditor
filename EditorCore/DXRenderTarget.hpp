#pragma once
#include "Define.h"
#include "BaseCore\DXShaderManager.h"
#include "BaseCore\DXTextureManager.hpp"
#include "BaseCore\DXSamplerState.hpp"
struct TIndex
{
	int							Index = 0;
	int							empty0 = 0;
	int							empty1 = 0;
	int							empty2 = 0;
};

class DXRenderTarget
{
public:
	// Texture Render Target View
	D3D11_VIEWPORT					m_viewPort = D3D11_VIEWPORT();

	DXTexture*						m_pTexture = nullptr;
	ID3D11Texture2D*				m_pDSTexture = nullptr;

	// For Save Prev View Infomation.
	ID3D11RenderTargetView*			m_pOldRenderTargetView = nullptr;
	ID3D11DepthStencilView*			m_pOldDepthStencilView = nullptr;
	D3D11_VIEWPORT					m_OldViewPort[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX] = {};

	// For Render
	ID3D11InputLayout*				InputLayout = nullptr;

	ID3D11VertexShader*				VertexShader = nullptr;
	ID3D11PixelShader*				PixelShader = nullptr;
	ID3D11PixelShader*				ArrayTexturePixelShader = nullptr;

	ID3D11Buffer*					VertexBuffer = nullptr;
	ID3D11Buffer*					IndexBuffer = nullptr;
	ID3D11Buffer*					TransformBuffer = nullptr;
	ID3D11Buffer*					CameraMatrixBuffer = nullptr;
	TransformMatrix					TransformData;
	CameraMatrix					CameraMatrixData;

	ID3D11Buffer*					TextureIndexBuffer = nullptr;
	TIndex							TextureIndex;

	std::vector<Vertex>				VertexList;
	std::vector<DWORD>				IndexList;

	UINT Count = 0;
	std::vector<ID3D11RenderTargetView*> RTVList;
	std::vector<ID3D11DepthStencilView*> DSVList;

	ID3D11ShaderResourceView* DSVShaderResourceView = nullptr;

public:
	DXRenderTarget();
	virtual ~DXRenderTarget();

public:
	bool Create(float x, float y, float width, float height, UINT count = 1, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	bool Begin(UINT index = 0);
	bool Render(UINT index, float x, float y, float width, float height);
	bool RenderDepthTexture(UINT index, float x, float y, float width, float height);
	bool End(UINT index = 0);
	bool Release();
};