#pragma once
#include "Define.h"

class DXDevice : public Singleton<DXDevice>
{
private:
	friend class Singleton<DXDevice>;

public:
	// �������̽� -> �ϵ���� ���� ���� -> ȹ��

	// ���ҽ��� ������ ���õ� �۾��� ���. ����̽� ��ġ Ȥ�� ����̽� ��ü�� �θ�
	ComPtr<ID3D11Device>			m_pd3dDevice = nullptr;

	// ������ ���ҽ����� ����Ͽ� ������ ó�� �� ����, ��� ������
	ComPtr<ID3D11DeviceContext>		m_pImmediateContext = nullptr;

	// Swap Chain�� ������ ���õ� �۾��� ���.
	ComPtr<IDXGIFactory>			m_pGIFactory = nullptr;

	// ���� ���� �� �ĸ� ���۵��� ���� �� ����. ���� ���۸�
	ComPtr<IDXGISwapChain>			m_pSwapChain = nullptr;

	// Render Target View: ������ �ĸ� ���۵� �߿��� ������(���) �� ��� ���۸� ����. 
	ComPtr<ID3D11RenderTargetView>	m_pRTV = nullptr;

	// Depth Stencil View
	ComPtr<ID3D11Texture2D>			m_pDSTexture = nullptr;
	ComPtr<ID3D11DepthStencilView>	m_pDepthStencilView = nullptr;

	D3D11_VIEWPORT					m_ViewPort;

private:
	HWND							m_hWnd;

public:
	bool Create(HWND hWnd);
	bool Resize();
	
	ID3D11Device*			GetDevice();
	ID3D11DeviceContext*	GetContext();
	IDXGISwapChain*			GetSwapChain();

protected:
	// 1) ����̽� ����
	HRESULT CreateDevice();
	// 2) ���丮 ����
	HRESULT CreateFactory();
	// 3) ����ü�� ����
	HRESULT CreateSwapChain();
	// 4) ���� Ÿ�ٺ� ����
	HRESULT CreateRenderTargetView();
	// 5) �� ��Ʈ ����
	void CreateViewPort();

	// Depth Stencil View
	HRESULT CreateDepthStencilView();
};

#define DEVICE DXDevice::getInstance()