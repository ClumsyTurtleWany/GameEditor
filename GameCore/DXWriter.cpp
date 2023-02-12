#include "DXWriter.hpp"
#include "WindowUI.hpp"

bool DXWriter::initialize()
{
	HRESULT rst;
	rst = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pd2dFactory);
	if (FAILED(rst))
	{
		OutputDebugString(L"WanyCore::DXWriter::Failed Create Factory.\n");
		return false;
	}

	rst = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&m_pDWriteFactory);
	if (FAILED(rst))
	{
		OutputDebugString(L"WanyCore::DXWriter::Failed Create Write Factory.\n");
		return false;
	}

	// m_pDWriteFactory->CreateTextFormat
	// WCHAR const* fontFamilyName,
	// IDWriteFontCollection* fontCollection,
	//	DWRITE_FONT_WEIGHT fontWeight,
	//	DWRITE_FONT_STYLE fontStyle,
	//	DWRITE_FONT_STRETCH fontStretch,
	//	FLOAT fontSize,
	//	WCHAR const* localeName,
	//	IDWriteTextFormat** textFormat
	// 폰트랑 사이즈는 실시간 변경 불가함. 색상이랑 글자는 가능.
	rst = m_pDWriteFactory->CreateTextFormat(
		L"고딕", //L"Gabriola ", 
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		20,
		L"ko-kr", // 영문 폰트면 L"en-us"
		&m_pTextFormat);

	if (FAILED(rst))
	{
		OutputDebugString(L"WanyCore::DXWriter::Failed Create Text Format.\n");
		return false;
	}

	if (bDrawLayout)
	{
		m_strDefault = L"DEFAULT";
		// 글자가 박혀있어서 글자는 변경 불가능 하다.
		rst = m_pDWriteFactory->CreateTextLayout(
			m_strDefault.c_str(),
			static_cast<UINT32>(m_strDefault.size()),
			m_pTextFormat,
			1024, 768,
			&m_pTextLayout);

		if (FAILED(rst))
		{
			OutputDebugString(L"WanyCore::DXWriter::Failed Create Text Layout.\n");
			return false;
		}
	}

	return true;
};

bool DXWriter::frame()
{
	return true;
};

bool DXWriter::render()
{
	m_pd2dRenderTarget->BeginDraw();

	// render는 반드시 begin과 end 사이에 넣어야 함.
	// Draw
	float clientWidth = g_pWindow->getClientWidth();
	float clientHeight = g_pWindow->getClientHeight();

	D2D1_RECT_F rt = { 0, 0, clientWidth, clientHeight };
	m_pTextColor->SetColor({ 1, 0, 0, 1 });
	m_pTextColor->SetOpacity(1); // 0에 가까울 수록 투명해짐. 0 ~ 1
	m_pd2dRenderTarget->DrawText(m_strDefault.c_str(), static_cast<UINT32>(m_strDefault.size()), m_pTextFormat, rt, m_pTextColor);

	m_pd2dRenderTarget->EndDraw();
	return true;
};

bool DXWriter::release()
{
	if (m_pTextLayout != nullptr)
	{
		m_pTextLayout->Release();
		m_pTextLayout = nullptr;
	}

	if (m_pTextColor != nullptr)
	{
		m_pTextColor->Release();
		m_pTextColor = nullptr;
	}

	if (m_pTextFormat != nullptr)
	{
		m_pTextFormat->Release();
		m_pTextFormat = nullptr;
	}

	if (m_pDWriteFactory != nullptr)
	{
		m_pDWriteFactory->Release();
		m_pDWriteFactory = nullptr;
	}

	if (m_pd2dRenderTarget != nullptr)
	{
		m_pd2dRenderTarget->Release();
		m_pd2dRenderTarget = nullptr;
	}

	if (m_pd2dFactory != nullptr)
	{
		m_pd2dFactory->Release();
		m_pd2dFactory = nullptr;
	}

	return true;
};

bool DXWriter::releaseDXResource()
{
	if (m_pTextColor != nullptr)
	{
		m_pTextColor->Release();
		m_pTextColor = nullptr;
	}

	if (m_pd2dRenderTarget != nullptr)
	{
		m_pd2dRenderTarget->Release();
		m_pd2dRenderTarget = nullptr;
	}

	return true;
}
bool DXWriter::createDXResource()
{
	HRESULT rst = m_pd2dRenderTarget->CreateSolidColorBrush(D2D1_COLOR_F({ 0.0f, 0.0f, 0.0f, 1.0f }), &m_pTextColor);
	if (FAILED(rst))
	{
		return false;
	}

	return true;
}

bool DXWriter::setBuffer(IDXGISurface* _dxgiSurface)
{
	// 3D -> 2D 연동하는 기능.
	// IDXGISurface* dxgiSurface,
	// D2D1_RENDER_TARGET_PROPERTIES& renderTargetProperties,
	// ID2D1RenderTarget** renderTarget

	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties;
	ZeroMemory(&renderTargetProperties, sizeof(renderTargetProperties));
	renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
	renderTargetProperties.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED); // 3D에 어떻게 들어있는지 몰라서 Unknown.
	renderTargetProperties.dpiX = 96.0f; // inch 당 픽셀이 몇개냐. 보통은 96개를 가지고 있음.
	renderTargetProperties.dpiY = 96.0f; // inch 당 픽셀이 몇개냐. 보통은 96개를 가지고 있음.
	renderTargetProperties.usage = D2D1_RENDER_TARGET_USAGE_NONE;
	renderTargetProperties.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;

	// 중요!
	// CreateDxgiSurfaceRenderTarget 메소드는 DXGI 화면에 랜더링하는 랜더 타겟을 생성하며 반드시 Direct3D 디바이스를
	// 생성시에 반드시 다음과 같이 플래그를 지정해야 한다. Device -> D3D11CreateDevice 의 플래그, DXDevice
	// UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	// #ifdef _DEBUG 
	// createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	// #endif 
	HRESULT rst = m_pd2dFactory->CreateDxgiSurfaceRenderTarget(_dxgiSurface, renderTargetProperties, &m_pd2dRenderTarget);
	if (FAILED(rst))
	{
		OutputDebugString(L"WanyCore::DXWriter::Failed Create DxgiSurface Render Target.\n");
		return false;
	}

	m_pd2dRenderTarget->CreateSolidColorBrush(D2D1_COLOR_F({ 0.0f, 0.0f, 0.0f, 1.0f }), &m_pTextColor);

	return true;
}

void DXWriter::setString(std::wstring _str)
{
	m_strDefault = _str;
}

bool DXWriter::draw(int _x, int _y, std::wstring _str, D2D1_COLOR_F _color, float _alpha)
{
	m_pd2dRenderTarget->BeginDraw();

	// render는 반드시 begin과 end 사이에 넣어야 함.
	float clientRect = g_pWindow->getClientWidth();
	float clientHeight = g_pWindow->getClientHeight();
	D2D1_RECT_F rt = { static_cast<FLOAT>(_x), static_cast<FLOAT>(_y), clientRect, clientHeight };
	m_pTextColor->SetColor(_color);
	m_pTextColor->SetOpacity(_alpha); // 0에 가까울 수록 투명해짐. 0 ~ 1
	m_pd2dRenderTarget->DrawText(_str.c_str(), static_cast<UINT32>(_str.size()), m_pTextFormat, rt, m_pTextColor);

	// Draw Layout
	if (bDrawLayout)
	{
		m_pTextLayout->SetFontSize(static_cast<FLOAT>(_x), { 0, (UINT)m_strDefault.size() });
		m_pTextLayout->SetFontStyle(DWRITE_FONT_STYLE_ITALIC, { 0, (UINT)m_strDefault.size() });
		m_pd2dRenderTarget->DrawTextLayout({ 0, 0 }, m_pTextLayout, m_pTextColor);
	}

	m_pd2dRenderTarget->EndDraw();
	return true;
};
