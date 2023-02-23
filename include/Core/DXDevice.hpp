#pragma once
#include "Define.h"

class DXDevice //: public Singleton<DXDevice>
{
private:
	//friend class Singleton<DXDevice>;

public:
	// 인터페이스 -> 하드웨어 직접 제어 -> 획득

	// 리소스의 생성과 관련된 작업에 사용. 디바이스 장치 혹은 디바이스 객체라 부름
	ID3D11Device*				m_pd3dDevice = nullptr;

	// 생성된 리소스들을 사용하여 랜더링 처리 및 제어, 즉시 랜더링
	ID3D11DeviceContext*		m_pImmediateContext = nullptr;

	// Swap Chain의 생성과 관련된 작업에 사용.
	IDXGIFactory*				m_pGIFactory = nullptr;

	// 전면 버퍼 및 후면 버퍼들을 생성 및 제어. 더블 버퍼링
	IDXGISwapChain*				m_pSwapChain = nullptr;

	// Render Target View: 생성된 후면 버퍼들 중에서 랜더링(출력) 될 대상 버퍼를 지정. 
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
	// 1) 디바이스 생성
	HRESULT CreateDevice();
	// 2) 팩토리 생성
	HRESULT CreateFactory();
	// 3) 스왑체인 생성
	HRESULT CreateSwapChain(int width, int height);
	// 4) 랜더 타겟뷰 생성
	HRESULT CreateRenderTargetView();
	// 5) 뷰 포트 설정
	void CreateViewPort(int x, int y, int width, int height);

	// Depth Stencil View
	HRESULT CreateDepthStencilView();
};

//#define DEVICE DXDevice::getInstance()
//#define CONTEXT DXDevice::getInstance()->GetContext()