#pragma once
#include "Define.h"
#include "BaseCore\DXShaderManager.h"
#include "BaseCore\DXTextureManager.hpp"
#include "BaseCore\DXSamplerState.hpp"

class DXRenderTarget
{
private:
	// Device
	ID3D11Device*			m_pd3dDevice = nullptr;
	ID3D11DeviceContext*	m_pImmediateContext = nullptr;

	// Texture Render Target View
	D3D11_VIEWPORT					m_viewPort = D3D11_VIEWPORT();

	ComPtr<ID3D11RenderTargetView>	m_pRenderTargetView = nullptr;
	DXTexture*						m_pTexture = nullptr;
	
	ComPtr<ID3D11DepthStencilView>	m_pDepthStencilView = nullptr;
	ComPtr<ID3D11Texture2D>			m_pDSTexture = nullptr;

	// For Save Prev View Infomation.
	ID3D11RenderTargetView*			m_pOldRenderTargetView = nullptr;
	ID3D11DepthStencilView*			m_pOldDepthStencilView = nullptr;
	D3D11_VIEWPORT					m_OldViewPort[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX] = {};

	// For Render
	ID3D11InputLayout*				InputLayout = nullptr;

	ID3D11VertexShader*				VertexShader = nullptr;
	ID3D11PixelShader*				PixelShader = nullptr;

	ID3D11SamplerState*				SamplerState = nullptr;
	ID3D11RasterizerState*			RSState = nullptr;

	ID3D11Buffer*					VertexBuffer = nullptr;
	ID3D11Buffer*					IndexBuffer = nullptr;

	std::vector<Vertex>				VertexList;
	std::vector<DWORD>				IndexList;

public:
	DXRenderTarget();

public:
	void SetDevice(ID3D11Device* device, ID3D11DeviceContext* context);
	bool Create(float x, float y, float width, float height);
	bool Begin();
	bool Render();
	bool End();
	bool Release();
};