#pragma once
#include "Define.h"

static ID3D11DeviceContext* g_Context = nullptr;

class DXDevice //: public Singleton<DXDevice>
{
private:
	//friend class Singleton<DXDevice>;

public:
	// �������̽� -> �ϵ���� ���� ���� -> ȹ��

	// ���ҽ��� ������ ���õ� �۾��� ���. ����̽� ��ġ Ȥ�� ����̽� ��ü�� �θ�
	static ID3D11Device*			m_pd3dDevice;// = nullptr;

	// ������ ���ҽ����� ����Ͽ� ������ ó�� �� ����, ��� ������
	static ID3D11DeviceContext*		m_pImmediateContext;// = nullptr;

	// Swap Chain�� ������ ���õ� �۾��� ���.
	static IDXGIFactory*			m_pGIFactory;// = nullptr;

	// ���� ���� �� �ĸ� ���۵��� ���� �� ����. ���� ���۸�
	static IDXGISwapChain*			m_pSwapChain;// = nullptr;

	// Render Target View: ������ �ĸ� ���۵� �߿��� ������(���) �� ��� ���۸� ����. 
	static ID3D11RenderTargetView*	m_pRTV;// = nullptr;

	// Depth Stencil View
	static ID3D11Texture2D*			m_pDSTexture;// = nullptr;
	static ID3D11DepthStencilView*	m_pDepthStencilView;// = nullptr;

	static D3D11_VIEWPORT			m_ViewPort;

	static HWND						m_hWnd;

public:
	static bool Create(HWND hWnd);
	static bool Resize(int x, int y, int width, int height);
	static bool Release();

protected:
	// 1) ����̽� ����
	static HRESULT CreateDevice();
	// 2) ���丮 ����
	static HRESULT CreateFactory();
	// 3) ����ü�� ����
	static HRESULT CreateSwapChain(int width, int height);
	// 4) ���� Ÿ�ٺ� ����
	static HRESULT CreateRenderTargetView();
	// 5) �� ��Ʈ ����
	static void CreateViewPort(int x, int y, int width, int height);

	// Depth Stencil View
	static HRESULT CreateDepthStencilView();
};

//#define DEVICE DXDevice::GetInstance()
//#define CONTEXT DXDevice::GetInstance()->GetContext()