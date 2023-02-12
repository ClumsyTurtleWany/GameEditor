#include "DXDevice.hpp"

bool DXDevice::Create(HWND hWnd)
{
	m_hWnd = hWnd;

	//////////////////////////////////////////////////////////////////////
	// 0) ���丮 ����, ������ 2������ ���� ������ ���� �����ص� �����ϰ�, Full Screen ���� ���� Adapter ������ ���� �ʿ�.
	//////////////////////////////////////////////////////////////////////
	//if (FAILED(createFactory()))
	//{
	//	OutputDebugString(L"WanyCore::DXDevice::Failed Create Factory.\n");
	//	return false;
	//}

	//////////////////////////////////////////////////////////////////////
	// 1) ����̽� ����
	//////////////////////////////////////////////////////////////////////

	if (FAILED(CreateDevice())) // 0�� �������� �� �־ SUCCEED or FAILED�θ� ����� ��.
	{
		OutputDebugString(L"WanyCore::DXDevice::Failed Create Device.\n");
		return false;
	}

	////////////////////////////////////////////////////////////////////////
	//// 2) ���丮 ����
	////////////////////////////////////////////////////////////////////////
	if (FAILED(CreateFactory()))
	{
		OutputDebugString(L"WanyCore::DXDevice::Failed Create Factory.\n");
		return false;
	}

	//////////////////////////////////////////////////////////////////////
	// 3) ����ü�� ����
	//////////////////////////////////////////////////////////////////////

	if (FAILED(CreateSwapChain()))
	{
		OutputDebugString(L"WanyCore::DXDevice::Failed Create Swap Chain.\n");
		return false;
	}


	//////////////////////////////////////////////////////////////////////
	// 4) ���� Ÿ�ٺ� ����
	//////////////////////////////////////////////////////////////////////

	if (FAILED(CreateRenderTargetView()))
	{
		OutputDebugString(L"WanyCore::DXDevice::Failed Create Render Target View.\n");
		return false;
	}

	if (FAILED(CreateDepthStencilView()))
	{
		OutputDebugString(L"WanyCore::DXDevice::Failed Create Depth Stencil View.\n");
		return false;
	}

	//////////////////////////////////////////////////////////////////////
	// 5) �� ��Ʈ ����
	//////////////////////////////////////////////////////////////////////
	CreateViewPort();

	return false;
}

bool DXDevice::Resize()
{
	return false;
}

ID3D11Device* DXDevice::GetDevice()
{
	return m_pd3dDevice.Get();
}

ID3D11DeviceContext* DXDevice::GetContext()
{
	return m_pImmediateContext.Get();
}

IDXGISwapChain* DXDevice::GetSwapChain()
{
	return m_pSwapChain.Get();
}

HRESULT DXDevice::CreateDevice()
{
	//////////////////////////////////////////////////////////////////////
	// 1) ����̽� ����
	//////////////////////////////////////////////////////////////////////
	//
	// HRESULT rst; // Dx�� ��� ��ȯ���� HRESULT ���
	// IDXGIAdapter* pAdapter = nullptr; // �׷��� ī��, nullptr�� ������ ���� ��� ���� �׷��� ī�� ��� �ϰڴ�
	// D3D_DRIVER_TYPE DriverType = D3D_DRIVER_TYPE_HARDWARE; // �⺻�� D3D_DRIVER_TYPE_HARDWARE (GPU ó��)�� ���µ� �׷��� ī�尡 ������ ���� �ʰų� ����� �������� ������, Software(CPU ó��) ���� ���
	// HMODULE Software = NULL; // DirverType�� ���������� CPU ó���� ���� ��. NULL�� ��� ����.
	// UINT Flags = 0;
	// D3D_FEATURE_LEVEL pFeatureLevels[] = { /*D3D_FEATURE_LEVEL_12_0,*/ D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 }; // 12�� ����ϵ� ���� �ȵǸ� 11, 10 ������
	// UINT FeatureLevels = 2; // pFeatureLevels ���� ����
	// UINT SDKVersion = D3D11_SDK_VERSION; // ������ �ֽ����� ���� ��
	// ID3D11Device** ppDevice;
	// D3D_FEATURE_LEVEL* pFeatureLevel = nullptr; // �ȹ޾Ƶ� ��.
	// rst = D3D11CreateDevice(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, &m_pd3dDevice, pFeatureLevel, &m_pImmediateContext); // ����̽� ����

	// ��� ����
	//HRESULT rst; // Dx�� ��� ��ȯ���� HRESULT ���
	D3D_FEATURE_LEVEL* pFeatureLevel = nullptr;
	D3D_FEATURE_LEVEL pFeatureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
	//ID3D11Device** ppDevice;
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; // �⺻�� 0�ε�, �� �÷��׸� �־�� ���� Ÿ�� ������ ������ �ȴ�.
#ifdef _DEBUG 
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif 
	//rst = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, pFeatureLevels, 1, D3D11_SDK_VERSION, &m_pd3dDevice, pFeatureLevel, &m_pImmediateContext); // ����̽� ����

	return D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, pFeatureLevels, 1, D3D11_SDK_VERSION, &m_pd3dDevice, pFeatureLevel, &m_pImmediateContext);
}

HRESULT DXDevice::CreateFactory()
{
	//////////////////////////////////////////////////////////////////////
	// 2) ���丮 ����
	//////////////////////////////////////////////////////////////////////
	// IDXGIFactory* m_pGIFactory = nullptr;
	// IDXGISwapChain* m_pSwapChain = nullptr;
	// �Ϲ����� ���̶�� CreateDXGIFactory(IDXGIFactory, (void**)&m_pGIFactory) �� ���� �߰����� (new IDXGIFactory �ϴ� �� ó��)
	// COM(Component Object Model) ������ CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&m_pGIFactory) ó�� ��� �ؾߵ�.

	return CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&m_pGIFactory);
}

HRESULT DXDevice::CreateSwapChain()
{
	//////////////////////////////////////////////////////////////////////
	// 3) ����ü�� ����
	//////////////////////////////////////////////////////////////////////
	// IUnknown* pDevice,
	// DXGI_SWAP_CHAIN_DESC* pDesc,
	// IDXGISwapChain** ppSwapChain
	// �ĸ����(�����) ���� <-> ���� ����

	DXGI_SWAP_CHAIN_DESC Desc;
	// DXGI_SWAP_CHAIN_DESC ����ü
	// DXGI_MODE_DESC BufferDesc;
	// DXGI_SAMPLE_DESC SampleDesc;
	// DXGI_USAGE BufferUsage;
	// UINT BufferCount;
	// HWND OutputWindow;
	// BOOL Windowed;
	// DXGI_SWAP_EFFECT SwapEffect;
	// UINT Flags;

	ZeroMemory(&Desc, sizeof(DXGI_SWAP_CHAIN_DESC));

	// UINT BufferCount;
	Desc.BufferCount = 1; // �� ���� ����

	// DXGI_MODE_DESC BufferDesc; // ���� �߿�
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	Desc.BufferDesc.Width = rc.right - rc.left; // Ŭ���̾�Ʈ Width // Ŭ���̾�Ʈ ũ�⺸�� �۰� ����� �ȵ�.
	Desc.BufferDesc.Height = rc.bottom - rc.top; // Ŭ���̾�Ʈ Height
	Desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32��Ʈ RGBA ���
	Desc.BufferDesc.RefreshRate.Numerator = 60; // ����� �ֻ���
	Desc.BufferDesc.RefreshRate.Denominator = 1; // �и�, Numerator / Denominator == 60 / 1 == 60 Hz

	// DXGI_USAGE BufferUsage;
	// ���� ���۰� ������ ����̶�� ���� ����. 
	// ����ü���� ���۸� �����Ϸ��� �ݵ�� DXGI_USAGE_RENDER_TARGET_OUTPUT �̾�� ��.
	Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// HWND OutputWindow;
	Desc.OutputWindow = m_hWnd; // �ش� �����쿡 ���

	// DXGI_SAMPLE_DESC SampleDesc;
	// ���ø��� ��(��ȣ �� �ý���, �������� �ð��׷����� �ú��� �Ͽ� �߰� �ɰԴ� ��, 1�� ������ �ƹ� �͵� ���� �Ͱ� ����.)
	// ���⼭�� �����Ӵ� ��� �Ѹ� ���̳ķ� �ؼ� ����.
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;

	// BOOL Windowed;
	Desc.Windowed = true; // â���� ���ų�(true) Ǯ ��ũ��(��üȭ�� false)���� ���ų�, Ǯ ��ũ���� Ÿ��Ʋ ������ �Ⱥ���. �ִ�ȭ�� �ٸ�.

	// UINT Flags;
	Desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	//Desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	// DXGI_SWAP_EFFECT SwapEffect;
	// ������ �ø��� ���� �� ���۸� �������� �������� �����ϴ� Ŭ����
	// DXGI_SWAP_EFFECT_DISCARD = 0 : Present ȣ�� �� �� ���� ���� ����
	// DXGI_SWAP_EFFECT_SEQUENTIAL = 1 : Present ȣ�� �� �� ���� ���� ����
	// �÷��״� ��Ƽ ���ø��� �Բ� ��� �Ұ�!

	return m_pGIFactory->CreateSwapChain(m_pd3dDevice.Get(), &Desc, m_pSwapChain.GetAddressOf());
}

HRESULT DXDevice::CreateRenderTargetView()
{
	//////////////////////////////////////////////////////////////////////
	// 4) ���� Ÿ�ٺ� ����
	//////////////////////////////////////////////////////////////////////
	HRESULT rst;

	ID3D11Texture2D* pBackBuffer = nullptr;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer); // ������ ���� 0�� ���۸� ������. ���纻�� ���� ����.
	if (pBackBuffer != nullptr)
	{
		rst = m_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRTV);
		pBackBuffer->Release(); // ���纻 ������
		return rst;
	}

	return S_FALSE;
}

void DXDevice::CreateViewPort()
{
	//////////////////////////////////////////////////////////////////////
	// 5) �� ��Ʈ ����
	//////////////////////////////////////////////////////////////////////
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	m_ViewPort.Width = static_cast<float>(rc.right - rc.left);
	m_ViewPort.Height = static_cast<float>(rc.bottom - rc.top);
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_ViewPort.MaxDepth = 1.0f;
	m_ViewPort.MinDepth = 0.0f;

	m_pImmediateContext->RSSetViewports(1, &m_ViewPort);
}

HRESULT DXDevice::CreateDepthStencilView()
{
	// ���� ���ٽ� ��� ���� �����ؾ߸� �Ѵ�. �굵 Ŭ���̾�Ʈ ������¡ �� �� �ʱ�ȭ �ʿ�.
	// 1. �ؽ�ó ����.
	// 2. ���� ���ٽ� �� ����.
	// 3. �� ����. (OMSetRenderTargets)
	// 4. ���� ���ٽ� �� ���� ��ü �����ؼ� ����.

	// 1. �ؽ�ó ����.
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	DXGI_SWAP_CHAIN_DESC refDesc;
	m_pSwapChain->GetDesc(&refDesc);

	desc.Width = refDesc.BufferDesc.Width;
	desc.Height = refDesc.BufferDesc.Height;
	desc.MipLevels = 1; // 0���� �ϸ� 9�ܰ� ���� ����
	desc.ArraySize = 1; // 1���� ����
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //DXGI_FORMAT_R24G8_TYPELESS; // 32bit, 24bit�� Z����(Depth)�� ����ϰ�, 8bit�� ���ٽ� ���۷� ��� �ϰڴ�.
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL; // �߿�! ��� ���� �� ������ ���ϴ� ��.
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	HRESULT rst = m_pd3dDevice->CreateTexture2D(&desc, NULL, &m_pDSTexture);
	if (FAILED(rst))
	{
		return rst;
	}

	// 2. ���� ���ٽ� �� ����.
	D3D11_DEPTH_STENCIL_VIEW_DESC DSdesc;
	ZeroMemory(&DSdesc, sizeof(DSdesc));
	DSdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Depth�� ��ֶ����� ��������, ���ٽ��� ���������� ����. Texture ������ ��Ʈ�� ���� �����.
	DSdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSdesc.Flags = 0;

	rst = m_pd3dDevice->CreateDepthStencilView(m_pDSTexture.Get(), &DSdesc, &m_pDepthStencilView);
	if (FAILED(rst))
	{
		return rst;
	}

	return rst;
}
