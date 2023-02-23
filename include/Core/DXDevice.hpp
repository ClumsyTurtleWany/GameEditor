#pragma once
#include "Define.h"

class DXDevice //: public Singleton<DXDevice>
{
private:
	//friend class Singleton<DXDevice>;

public:
	// �������̽� -> �ϵ���� ���� ���� -> ȹ��

	// ���ҽ��� ������ ���õ� �۾��� ���. ����̽� ��ġ Ȥ�� ����̽� ��ü�� �θ�
	ID3D11Device*				m_pd3dDevice = nullptr;

	// ������ ���ҽ����� ����Ͽ� ������ ó�� �� ����, ��� ������
	ID3D11DeviceContext*		m_pImmediateContext = nullptr;

	// Swap Chain�� ������ ���õ� �۾��� ���.
	IDXGIFactory*				m_pGIFactory = nullptr;

	// ���� ���� �� �ĸ� ���۵��� ���� �� ����. ���� ���۸�
	IDXGISwapChain*				m_pSwapChain = nullptr;

	// Render Target View: ������ �ĸ� ���۵� �߿��� ������(���) �� ��� ���۸� ����. 
	ID3D11RenderTargetView*		m_pRTV = nullptr;

	// Depth Stencil View
	ID3D11Texture2D*			m_pDSTexture = nullptr;
	ID3D11DepthStencilView*		m_pDepthStencilView = nullptr;

	D3D11_VIEWPORT				m_ViewPort;

private:
	HWND						m_hWnd;

public:
	bool Create(HWND hWnd);
	bool Resize(int x, int y, int width, int height);
	bool Release();
	
	ID3D11Device*			GetDevice();
	ID3D11DeviceContext*	GetContext();
	IDXGISwapChain*			GetSwapChain();

protected:
	// 1) ����̽� ����
	HRESULT CreateDevice();
	// 2) ���丮 ����
	HRESULT CreateFactory();
	// 3) ����ü�� ����
	HRESULT CreateSwapChain(int width, int height);
	// 4) ���� Ÿ�ٺ� ����
	HRESULT CreateRenderTargetView();
	// 5) �� ��Ʈ ����
	void CreateViewPort(int x, int y, int width, int height);

	// Depth Stencil View
	HRESULT CreateDepthStencilView();
};

//#define DEVICE DXDevice::getInstance()
//#define CONTEXT DXDevice::getInstance()->GetContext()