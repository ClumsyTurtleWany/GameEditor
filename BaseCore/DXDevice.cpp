#include "DXDevice.hpp"


ID3D11Device*			DXDevice::g_pd3dDevice = nullptr;
ID3D11DeviceContext*	DXDevice::g_pImmediateContext = nullptr;
IDXGIFactory*			DXDevice::g_pGIFactory = nullptr;
IDXGISwapChain*			DXDevice::g_pSwapChain = nullptr;
ID3D11RenderTargetView* DXDevice::g_pRTV = nullptr;
ID3D11Texture2D*		DXDevice::g_pDSTexture = nullptr;
ID3D11DepthStencilView* DXDevice::g_pDepthStencilView = nullptr;
D3D11_VIEWPORT			DXDevice::g_ViewPort;
HWND					DXDevice::g_hWnd = NULL;

bool DXDevice::Create(HWND hWnd)
{
	g_hWnd = hWnd;

	//////////////////////////////////////////////////////////////////////
	// 0) 팩토리 생성, 원래는 2번에서 생성 했으나 먼저 생성해도 무관하고, Full Screen 막기 위한 Adapter 생성에 먼저 필요.
	//////////////////////////////////////////////////////////////////////
	//if (FAILED(createFactory()))
	//{
	//	OutputDebugStringA("DXDevice::Failed Create Factory.\n");
	//	return false;
	//}

	//////////////////////////////////////////////////////////////////////
	// 1) 디바이스 생성
	//////////////////////////////////////////////////////////////////////

	if (FAILED(CreateDevice())) // 0도 성공으로 들어가 있어서 SUCCEED or FAILED로만 사용할 것.
	{
		OutputDebugStringA("DXDevice::Failed Create Device.\n");
		return false;
	}

	////////////////////////////////////////////////////////////////////////
	//// 2) 팩토리 생성
	////////////////////////////////////////////////////////////////////////
	if (FAILED(CreateFactory()))
	{
		OutputDebugStringA("DXDevice::Failed Create Factory.\n");
		return false;
	}

	//////////////////////////////////////////////////////////////////////
	// 3) 스왑체인 생성
	//////////////////////////////////////////////////////////////////////
	RECT rc;
	GetClientRect(g_hWnd, &rc);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;
	if (FAILED(CreateSwapChain(width, height)))
	{
		OutputDebugStringA("DXDevice::Failed Create Swap Chain.\n");
		return false;
	}

	//////////////////////////////////////////////////////////////////////
	// 4) 랜더 타겟뷰 생성
	//////////////////////////////////////////////////////////////////////

	if (FAILED(CreateRenderTargetView()))
	{
		OutputDebugStringA("DXDevice::Failed Create Render Target View.\n");
		return false;
	}

	if (FAILED(CreateDepthStencilView()))
	{
		OutputDebugStringA("DXDevice::Failed Create Depth Stencil View.\n");
		return false;
	}

	//////////////////////////////////////////////////////////////////////
	// 5) 뷰 포트 설정
	//////////////////////////////////////////////////////////////////////
	CreateViewPort(0, 0, width, height);

	return true;
}

bool DXDevice::Resize(int x, int y, int width, int height)
{
	if (g_pd3dDevice == nullptr)
	{
		OutputDebugString(L"DXDevice::Resize::Device is NULL.");
		return false;
	}

	g_pImmediateContext->ClearState();

	g_pImmediateContext->OMSetRenderTargets(0, nullptr, NULL);
	g_pRTV->Release();
	g_pRTV = nullptr;

	// Release Depth Stencil View
	g_pDepthStencilView->Release();
	g_pDepthStencilView = nullptr;

	g_pDSTexture->Release();
	g_pDSTexture = nullptr;

	//m_pSwapChain->Release();
	//m_pSwapChain = nullptr;

	// 변경된 윈도우의 크기를 얻고 백 버퍼의 크기를 재 조정.
	DXGI_SWAP_CHAIN_DESC desc;
	g_pSwapChain->GetDesc(&desc);
	//desc.BufferDesc.Width = width; // 클라이언트 Width // 클라이언트 크기보다 작게 만들면 안됨.
	//desc.BufferDesc.Height = height; // 클라이언트 Height
	HRESULT rst = g_pSwapChain->ResizeBuffers(desc.BufferCount, width, height, desc.BufferDesc.Format, 0);
	if (FAILED(rst))
	{
		OutputDebugString(L"DXDevice::Resize::Failed Resize Buffer.");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC desc2;
	g_pSwapChain->GetDesc(&desc2);

	/*HRESULT rst = CreateSwapChain(width, height);
	if (FAILED(rst))
	{
		return false;
	}*/

	// 변경된 백 버퍼의 크기를 얻고 렌더타켓 뷰를 다시 생성 및 적용.
	rst = CreateRenderTargetView();
	if (FAILED(rst))
	{
		OutputDebugString(L"DXDevice::Resize::Failed Create Render Target View.");
		return false;
	}

	// 소멸했던 깊이 스텐실 버퍼와 깊이 스텐실 뷰 다시 생성 및 적용

	rst = CreateDepthStencilView();
	if (FAILED(rst))
	{
		OutputDebugString(L"DXDevice::Resize::Failed Create Depth Stencil View.");
		return false;
	}
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRTV, g_pDepthStencilView);

	// 뷰포트 재 지정.
	CreateViewPort(x, y, width, height);

	return true;
}

bool DXDevice::Release()
{
	if (g_pDepthStencilView != nullptr)
	{
		g_pDepthStencilView->Release();
		g_pDepthStencilView = nullptr;
	}

	if (g_pDSTexture != nullptr)
	{
		g_pDSTexture->Release();
		g_pDSTexture = nullptr;
	}

	if (g_pRTV != nullptr)
	{
		g_pRTV->Release();
		g_pRTV = nullptr;
	}

	if (g_pSwapChain != nullptr)
	{
		g_pSwapChain->Release();
		g_pSwapChain = nullptr;
	}

	if (g_pGIFactory != nullptr)
	{
		g_pGIFactory->Release();
		g_pGIFactory = nullptr;
	}

	if (g_pImmediateContext != nullptr)
	{
		g_pImmediateContext->ClearState();
		g_pImmediateContext->Release();
		g_pImmediateContext = nullptr;
	}

	if (g_pd3dDevice != nullptr)
	{
		g_pd3dDevice->Release();
		g_pd3dDevice = nullptr;
	}

	return true;
}

HRESULT DXDevice::CreateDevice()
{
	//////////////////////////////////////////////////////////////////////
	// 1) 디바이스 생성
	//////////////////////////////////////////////////////////////////////
	//
	// HRESULT rst; // Dx의 모든 반환값은 HRESULT 사용
	// IDXGIAdapter* pAdapter = nullptr; // 그래픽 카드, nullptr을 넣으면 현재 사용 중인 그래픽 카드 사용 하겠다
	// D3D_DRIVER_TYPE DriverType = D3D_DRIVER_TYPE_HARDWARE; // 기본은 D3D_DRIVER_TYPE_HARDWARE (GPU 처리)를 쓰는데 그래픽 카드가 성능이 좋지 않거나 기능을 지원하지 않으면, Software(CPU 처리) 등을 사용
	// HMODULE Software = NULL; // DirverType과 마찬가지로 CPU 처리를 위한 것. NULL은 사용 안함.
	// UINT Flags = 0;
	// D3D_FEATURE_LEVEL pFeatureLevels[] = { /*D3D_FEATURE_LEVEL_12_0,*/ D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 }; // 12를 사용하되 지원 안되면 11, 10 순으로
	// UINT FeatureLevels = 2; // pFeatureLevels 인자 갯수
	// UINT SDKVersion = D3D11_SDK_VERSION; // 무조건 최신으로 넣을 것
	// ID3D11Device** ppDevice;
	// D3D_FEATURE_LEVEL* pFeatureLevel = nullptr; // 안받아도 됨.
	// rst = D3D11CreateDevice(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, &m_pd3dDevice, pFeatureLevel, &m_pImmediateContext); // 디바이스 생성

	// 축약 버전
	//HRESULT rst; // Dx의 모든 반환값은 HRESULT 사용
	D3D_FEATURE_LEVEL pFeatureLevel; // = nullptr;
	D3D_FEATURE_LEVEL pFeatureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
	//ID3D11Device** ppDevice;
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; // 기본은 0인데, 이 플래그를 넣어야 랜더 타겟 생성이 지원이 된다.
#ifdef _DEBUG 
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif 
	//rst = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, pFeatureLevels, 1, D3D11_SDK_VERSION, &m_pd3dDevice, pFeatureLevel, &m_pImmediateContext); // 디바이스 생성

	return D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, pFeatureLevels, 1, D3D11_SDK_VERSION, &g_pd3dDevice, &pFeatureLevel, &g_pImmediateContext);
}

HRESULT DXDevice::CreateFactory()
{
	//////////////////////////////////////////////////////////////////////
	// 2) 팩토리 생성
	//////////////////////////////////////////////////////////////////////
	// IDXGIFactory* m_pGIFactory = nullptr;
	// IDXGISwapChain* m_pSwapChain = nullptr;
	// 일반적인 놈이라면 CreateDXGIFactory(IDXGIFactory, (void**)&m_pGIFactory) 로 생성 했겠지만 (new IDXGIFactory 하는 것 처럼)
	// COM(Component Object Model) 에서는 CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&m_pGIFactory) 처럼 사용 해야됨.

	return CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&g_pGIFactory);
}

HRESULT DXDevice::CreateSwapChain(int width, int height)
{
	//////////////////////////////////////////////////////////////////////
	// 3) 스왑체인 생성
	//////////////////////////////////////////////////////////////////////
	// IUnknown* pDevice,
	// DXGI_SWAP_CHAIN_DESC* pDesc,
	// IDXGISwapChain** ppSwapChain
	// 후면버퍼(백버퍼) 생성 <-> 전면 버퍼

	DXGI_SWAP_CHAIN_DESC Desc;
	// DXGI_SWAP_CHAIN_DESC 구조체
	// DXGI_MODE_DESC BufferDesc;
	// DXGI_SAMPLE_DESC SampleDesc;
	// DXGI_USAGE BufferUsage;
	// UINT BufferCount;
	// HWND OutputWindow;
	// BOOL Windowed;
	// DXGI_SWAP_EFFECT SwapEffect;
	// UINT Flags;

	ZeroMemory(&Desc, sizeof(Desc));

	// UINT BufferCount;
	Desc.BufferCount = 1; // 백 버퍼 개수

	// DXGI_MODE_DESC BufferDesc; // 가장 중요
	
	Desc.BufferDesc.Width = width; // 클라이언트 Width // 클라이언트 크기보다 작게 만들면 안됨.
	Desc.BufferDesc.Height = height; // 클라이언트 Height
	Desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32비트 RGBA 사용
	Desc.BufferDesc.RefreshRate.Numerator = 60; // 모니터 주사율
	Desc.BufferDesc.RefreshRate.Denominator = 1; // 분모, Numerator / Denominator == 60 / 1 == 60 Hz

	// DXGI_USAGE BufferUsage;
	// 위의 버퍼가 랜더링 대상이라는 것을 지정. 
	// 스왑체인의 버퍼를 생성하려면 반드시 DXGI_USAGE_RENDER_TARGET_OUTPUT 이어야 함.
	Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// HWND OutputWindow;
	Desc.OutputWindow = g_hWnd; // 해당 윈도우에 출력

	// DXGI_SAMPLE_DESC SampleDesc;
	// 샘플링할 수(신호 및 시스템, 연속적인 시간그래프를 시분할 하여 잘게 쪼게는 것, 1로 나누면 아무 것도 안한 것과 같다.)
	// 여기서는 프레임당 몇번 뿌릴 것이냐로 해석 가능.
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;

	// BOOL Windowed;
	Desc.Windowed = true; // 창모드로 띄울거냐(true) 풀 스크린(전체화면 false)으로 띄울거냐, 풀 스크린은 타이틀 라인이 안보임. 최대화와 다름.

	// UINT Flags;
	Desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	//Desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	// DXGI_SWAP_EFFECT SwapEffect;
	// 페이지 플립핑 이후 백 버퍼를 삭제할지 보존할지 결정하는 클래그
	// DXGI_SWAP_EFFECT_DISCARD = 0 : Present 호출 시 백 버퍼 내용 삭제
	// DXGI_SWAP_EFFECT_SEQUENTIAL = 1 : Present 호출 시 백 버퍼 내용 보존
	// 플래그는 멀티 샘플링과 함께 사용 불가!

	return g_pGIFactory->CreateSwapChain(g_pd3dDevice, &Desc, &g_pSwapChain);
}

HRESULT DXDevice::CreateRenderTargetView()
{
	//////////////////////////////////////////////////////////////////////
	// 4) 랜더 타겟뷰 생성
	//////////////////////////////////////////////////////////////////////
	HRESULT rst;

	ID3D11Texture2D* pBackBuffer = nullptr;
	g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer); // 위에서 만든 0번 버퍼를 가져옴. 복사본이 오는 것임.
	if (pBackBuffer != nullptr)
	{
		rst = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRTV);
		pBackBuffer->Release(); // 복사본 릴리즈
		return rst;
	}

	return S_FALSE;
}

void DXDevice::CreateViewPort(int x, int y, int width, int height)
{
	//////////////////////////////////////////////////////////////////////
	// 5) 뷰 포트 설정
	//////////////////////////////////////////////////////////////////////
	g_ViewPort.Width = static_cast<float>(width);
	g_ViewPort.Height = static_cast<float>(height);
	g_ViewPort.TopLeftX = static_cast<float>(x);
	g_ViewPort.TopLeftY = static_cast<float>(y);
	g_ViewPort.MaxDepth = 1.0f;
	g_ViewPort.MinDepth = 0.0f;

	g_pImmediateContext->RSSetViewports(1, &g_ViewPort);
}

HRESULT DXDevice::CreateDepthStencilView()
{
	// 깊이 스텐실 뷰는 직접 생성해야만 한다. 얘도 클라이언트 리사이징 할 때 초기화 필요.
	// 1. 텍스처 생성.
	// 2. 깊이 스텐실 뷰 생성.
	// 3. 뷰 적용. (OMSetRenderTargets)
	// 4. 깊이 스텐실 뷰 상태 객체 생성해서 적용.

	// 1. 텍스처 생성.
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	DXGI_SWAP_CHAIN_DESC refDesc;
	g_pSwapChain->GetDesc(&refDesc);

	desc.Width = refDesc.BufferDesc.Width;
	desc.Height = refDesc.BufferDesc.Height;
	desc.MipLevels = 1; // 0으로 하면 9단계 까지 생성
	desc.ArraySize = 1; // 1개만 생성
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //DXGI_FORMAT_R24G8_TYPELESS; // 32bit, 24bit는 Z버퍼(Depth)로 사용하고, 8bit는 스텐실 버퍼로 사용 하겠다.
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL; // 중요! 어디에 적용 할 것인지 정하는 것.
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	HRESULT rst = g_pd3dDevice->CreateTexture2D(&desc, NULL, &g_pDSTexture);
	if (FAILED(rst))
	{
		return rst;
	}

	// 2. 깊이 스텐실 뷰 생성.
	D3D11_DEPTH_STENCIL_VIEW_DESC DSdesc;
	ZeroMemory(&DSdesc, sizeof(DSdesc));
	DSdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Depth는 노멀라이즈 하지말고, 스텐실은 정수형으로 생성. Texture 생성과 비트를 맞춰 줘야함.
	DSdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSdesc.Flags = 0;

	rst = g_pd3dDevice->CreateDepthStencilView(g_pDSTexture, &DSdesc, &g_pDepthStencilView);
	if (FAILED(rst))
	{
		return rst;
	}

	return rst;
}
