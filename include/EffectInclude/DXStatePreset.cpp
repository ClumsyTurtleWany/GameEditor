#include "DXStatePreset.h"

namespace EFFECTUTIL
{
	CPGCDXStateManager::CPGCDXStateManager()
	{
		m_pDevice = nullptr;
		m_pDContext = nullptr;
	}

	CPGCDXStateManager::~CPGCDXStateManager()
	{

	}

	bool CPGCDXStateManager::init(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext)
	{
		if (!pDevice || !pDContext) { return false; }
		else
		{
			m_pDevice = pDevice;
			m_pDContext = pDContext;
		}

		HRESULT hr = S_OK;

		hr = createDefaultStates();

		return true;
	}

	bool CPGCDXStateManager::release()
	{
		m_pDevice = nullptr;
		m_pDContext = nullptr;

		for (auto it : m_SStateMap)
		{
			it.second->Release();
			it.second = nullptr;
		}
		m_SStateMap.clear();

		for (auto it : m_RStateMap)
		{
			it.second->Release();
			it.second = nullptr;
		}
		m_RStateMap.clear();

		for (auto it : m_BStateMap)
		{
			it.second->Release();
			it.second = nullptr;
		}
		m_BStateMap.clear();

		for (auto it : m_DSStateMap)
		{
			it.second->Release();
			it.second = nullptr;
		}
		m_DSStateMap.clear();

		return true;
	}

	HRESULT CPGCDXStateManager::createSState(std::wstring wszSStateName, D3D11_SAMPLER_DESC samplerDesc)
	{
		HRESULT hr = S_OK;

		if (m_SStateMap.find(wszSStateName) == m_SStateMap.end())
		{
			ID3D11SamplerState* newE;

			hr = m_pDevice->CreateSamplerState(&samplerDesc, &newE);
			if (SUCCEEDED(hr)) { m_SStateMap.insert(std::make_pair(wszSStateName, newE)); }
			else { printCreateErr(hr); }
		}
		else { hr = S_FALSE; }

		return hr;
	}

	HRESULT CPGCDXStateManager::createRState(std::wstring wszRStateName, D3D11_RASTERIZER_DESC rasterizerDesc)
	{
		HRESULT hr = S_OK;

		if (m_RStateMap.find(wszRStateName) == m_RStateMap.end())
		{
			ID3D11RasterizerState* newE;

			hr = m_pDevice->CreateRasterizerState(&rasterizerDesc, &newE);
			if (SUCCEEDED(hr)) { m_RStateMap.insert(std::make_pair(wszRStateName, newE)); }
			else { printCreateErr(hr); }
		}
		else { hr = S_FALSE; }

		return hr;
	}

	HRESULT CPGCDXStateManager::createBState(std::wstring wszBStateName, D3D11_BLEND_DESC blendDesc)
	{
		HRESULT hr = S_OK;

		if (m_BStateMap.find(wszBStateName) == m_BStateMap.end())
		{
			ID3D11BlendState* newE;

			hr = m_pDevice->CreateBlendState(&blendDesc, &newE);
			if (SUCCEEDED(hr)) { m_BStateMap.insert(std::make_pair(wszBStateName, newE)); }
			else { printCreateErr(hr); }
		}
		else { hr = S_FALSE; }

		return hr;
	}

	HRESULT CPGCDXStateManager::createDSState(std::wstring wszDSStateName, D3D11_DEPTH_STENCIL_DESC depthStencilDesc)
	{
		HRESULT hr = S_OK;

		if (m_DSStateMap.find(wszDSStateName) == m_DSStateMap.end())
		{
			ID3D11DepthStencilState* newE;

			hr = m_pDevice->CreateDepthStencilState(&depthStencilDesc, &newE);
			if (SUCCEEDED(hr)) { m_DSStateMap.insert(std::make_pair(wszDSStateName, newE)); }
			else { printCreateErr(hr); }
		}
		else { hr = S_FALSE; }

		return hr;
	}

	ID3D11SamplerState* CPGCDXStateManager::getSState(std::wstring wszSStateName)
	{
		auto it = m_SStateMap.find(wszSStateName);
		if (it != m_SStateMap.end()) { return it->second; }

		return nullptr;
	}

	ID3D11RasterizerState* CPGCDXStateManager::getRState(std::wstring wszRStateName)
	{
		auto it = m_RStateMap.find(wszRStateName);
		if (it != m_RStateMap.end()) { return it->second; }

		return nullptr;
	}

	ID3D11BlendState* CPGCDXStateManager::getBState(std::wstring wszBStateName)
	{
		auto it = m_BStateMap.find(wszBStateName);
		if (it != m_BStateMap.end()) { return it->second; }

		return nullptr;
	}

	ID3D11DepthStencilState* CPGCDXStateManager::getDSState(std::wstring wszDSStateName)
	{
		auto it = m_DSStateMap.find(wszDSStateName);
		if (it != m_DSStateMap.end()) { return it->second; }

		return nullptr;
	}

	HRESULT CPGCDXStateManager::createDefaultStates()
	{
		HRESULT hr = S_OK;

		//Sampler State 기본 설정값
		/*Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;		필터값 - 텍스처에서 가져오는 색상의 보간 형식
		AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;			UV주소 모드 - 0.0f~1.0f를 벗어나는 UV좌표에 대한 처리방식
		AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		MipLODBias = 0.0f;								LOD 레벨 보정값
		MaxAnisotropy = 1;								비등방성 밉맵단계 상한
		ComparisonFunc = D3D11_COMPARISON_NEVER;		샘플링 전후 비교 함수
		BorderColor[0] = 1.0f;							//주소 모드 Border지정시 범위를 벗어난 TexCoord좌표에 부여할 색상
		BorderColor[1] = 1.0f;
		BorderColor[2] = 1.0f;
		BorderColor[3] = 1.0f;
		MinLOD = -FLT_MAX;								//최소 밉맵 LOD레벨
		MaxLOD = +FLT_MAX;*/							//최대 밉맵 LOD레벨 : MinLOD보다 커야한다.

		//기본 샘플러 스테이트 - 선형 보간 텍스처 필터링 + 클램프 모드
		D3D11_SAMPLER_DESC SS_DEFAULT;
		ZeroMemory(&SS_DEFAULT, sizeof(D3D11_SAMPLER_DESC));
		SS_DEFAULT.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SS_DEFAULT.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		SS_DEFAULT.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		SS_DEFAULT.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

		hr = createSState(VNAME2WSTR(SS_DEFAULT), SS_DEFAULT);

		//선형 보간 텍스처 필터링 - 래핑 모드
		D3D11_SAMPLER_DESC SS_LINEAR_WRAP;
		ZeroMemory(&SS_LINEAR_WRAP, sizeof(D3D11_SAMPLER_DESC));
		SS_LINEAR_WRAP.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SS_LINEAR_WRAP.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SS_LINEAR_WRAP.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SS_LINEAR_WRAP.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		hr = createSState(VNAME2WSTR(SS_LINEAR_WRAP), SS_LINEAR_WRAP);

		//포인트 텍스처 필터링 - 래핑 모드
		D3D11_SAMPLER_DESC SS_POINT_WRAP;
		ZeroMemory(&SS_POINT_WRAP, sizeof(D3D11_SAMPLER_DESC));
		SS_POINT_WRAP.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		SS_POINT_WRAP.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SS_POINT_WRAP.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SS_POINT_WRAP.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		hr = createSState(VNAME2WSTR(SS_POINT_WRAP), SS_POINT_WRAP);

		//Rasterizer State 기본 설정값
		/*FillMode = D3D11_FILL_SOLID;					//채우기 모드
		CullMode = D3D11_CULL_BACK;						//은면 제거 방식
		FrontCounterClockwise = FALSE;					//앞, 뒤 판정 정점 구성방식
		DepthBias = 0;									//깊이 보정값
		DepthBiasClamp = 0.0f;							//깊이 보정값의 상한/하한
		SlopeScaledDepthBias = 0.0f;					//깊이 기울기의 스케일 보정값
		DepthClipEnable = TRUE;							//깊이에 따른 절단 과정 수행여부
		ScissorEnable = FALSE;							//시저 사각형 활성화 여부
		MultisampleEnable = FALSE;						//멀티 샘플링 사용 여부
		AntialiasedLineEnable = FALSE;*/				//선에 대한 안티 앨리어싱 수행 여부

		//와이어 프레임
		D3D11_RASTERIZER_DESC RS_WIREFRAME;
		ZeroMemory(&RS_WIREFRAME, sizeof(D3D11_RASTERIZER_DESC));
		RS_WIREFRAME.FillMode = D3D11_FILL_WIREFRAME;
		RS_WIREFRAME.CullMode = D3D11_CULL_BACK;
		RS_WIREFRAME.DepthClipEnable = TRUE;

		hr = createRState(VNAME2WSTR(RS_WIREFRAME), RS_WIREFRAME);

		//솔리드
		D3D11_RASTERIZER_DESC RS_SOLID;
		ZeroMemory(&RS_SOLID, sizeof(D3D11_RASTERIZER_DESC));
		RS_SOLID.FillMode = D3D11_FILL_SOLID;
		RS_SOLID.CullMode = D3D11_CULL_BACK;
		RS_SOLID.DepthClipEnable = TRUE;

		hr = createRState(VNAME2WSTR(RS_SOLID), RS_SOLID);

		//솔리드 : 컬링 작업 제외
		D3D11_RASTERIZER_DESC RS_SOLID_NOCULL;
		ZeroMemory(&RS_SOLID_NOCULL, sizeof(D3D11_RASTERIZER_DESC));
		RS_SOLID_NOCULL.FillMode = D3D11_FILL_SOLID;
		RS_SOLID_NOCULL.CullMode = D3D11_CULL_NONE;
		RS_SOLID_NOCULL.DepthClipEnable = TRUE;

		hr = createRState(VNAME2WSTR(RS_SOLID_NOCULL), RS_SOLID_NOCULL);

		//와이어 프레임 : 컬링 작업 제외
		D3D11_RASTERIZER_DESC RS_WIREFRAME_NOCULL;
		ZeroMemory(&RS_WIREFRAME_NOCULL, sizeof(D3D11_RASTERIZER_DESC));
		RS_WIREFRAME_NOCULL.FillMode = D3D11_FILL_WIREFRAME;
		RS_WIREFRAME_NOCULL.CullMode = D3D11_CULL_NONE;
		RS_WIREFRAME_NOCULL.DepthClipEnable = TRUE;

		hr = createRState(VNAME2WSTR(RS_WIREFRAME_NOCULL), RS_WIREFRAME_NOCULL);

		//솔리드 : 앞면 컬링
		D3D11_RASTERIZER_DESC RS_SOLID_FRONTCULL;
		ZeroMemory(&RS_SOLID_FRONTCULL, sizeof(D3D11_RASTERIZER_DESC));
		RS_SOLID_FRONTCULL.FillMode = D3D11_FILL_SOLID;
		RS_SOLID_FRONTCULL.CullMode = D3D11_CULL_FRONT;
		RS_SOLID_FRONTCULL.DepthClipEnable = TRUE;

		hr = createRState(VNAME2WSTR(RS_SOLID_FRONTCULL), RS_SOLID_FRONTCULL);

		//와이어 프레임 : 앞면 컬링
		D3D11_RASTERIZER_DESC RS_WIREFRAME_FRONTCULL;
		ZeroMemory(&RS_WIREFRAME_FRONTCULL, sizeof(D3D11_RASTERIZER_DESC));
		RS_WIREFRAME_FRONTCULL.FillMode = D3D11_FILL_WIREFRAME;
		RS_WIREFRAME_FRONTCULL.CullMode = D3D11_CULL_FRONT;
		RS_WIREFRAME_FRONTCULL.DepthClipEnable = TRUE;

		hr = createRState(VNAME2WSTR(RS_WIREFRAME_FRONTCULL), RS_WIREFRAME_FRONTCULL);

		//Blend State 기본 설정값
		/*AlphaToCoverageEnable = FALSE;											//Alpha To Coverage라는 멀티 샘플링 방법 사용 여부
		IndependentBlendEnable = FALSE;												//독립적인 렌더타겟 블렌딩 사용 여부
		RenderTarget[0].BlendEnable = FALSE;										//블렌딩 옵션 활성화 여부
		RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;									//추가할 대상 색상의 블렌딩 계수
		RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;								//기존 대상 색상의 블렌딩 계수
		RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;								//색상의 블렌딩 연산 방법
		RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;							//추가할 대상 알파값 블렌딩 계수
		RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;							//기존 대상 알파값 블렌딩 계수
		RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;							//알파값의 블렌딩 연산 방법
		RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;*/		//렌더 타겟으로 출력할 색상 채널 마스크

		//기본 블렌드 스테이트 : 블렌딩 없음
		D3D11_BLEND_DESC BS_DEFAULT;
		ZeroMemory(&BS_DEFAULT, sizeof(D3D11_BLEND_DESC));
		BS_DEFAULT.AlphaToCoverageEnable = FALSE;
		BS_DEFAULT.IndependentBlendEnable = FALSE;
		BS_DEFAULT.RenderTarget[0].BlendEnable = FALSE;

		BS_DEFAULT.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		BS_DEFAULT.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		BS_DEFAULT.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		BS_DEFAULT.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		BS_DEFAULT.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		BS_DEFAULT.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		BS_DEFAULT.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		hr = createBState(VNAME2WSTR(BS_DEFAULT), BS_DEFAULT);

		//멀티 샘플링 알파 테스팅 - 알파테스트에 의한 안티 앨리어싱
		D3D11_BLEND_DESC BS_MSALPHATEST;
		ZeroMemory(&BS_MSALPHATEST, sizeof(D3D11_BLEND_DESC));
		BS_MSALPHATEST.AlphaToCoverageEnable = TRUE;
		BS_MSALPHATEST.IndependentBlendEnable = FALSE;
		BS_MSALPHATEST.RenderTarget[0].BlendEnable = TRUE;

		BS_MSALPHATEST.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		BS_MSALPHATEST.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		BS_MSALPHATEST.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

		BS_MSALPHATEST.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		BS_MSALPHATEST.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		BS_MSALPHATEST.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		BS_MSALPHATEST.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		hr = createBState(VNAME2WSTR(BS_MSALPHATEST), BS_MSALPHATEST);



		//알파 블렌딩
		D3D11_BLEND_DESC BS_ALPHABLEND;
		ZeroMemory(&BS_ALPHABLEND, sizeof(D3D11_BLEND_DESC));
		BS_ALPHABLEND.AlphaToCoverageEnable = FALSE;
		BS_ALPHABLEND.IndependentBlendEnable = FALSE;
		BS_ALPHABLEND.RenderTarget[0].BlendEnable = TRUE;

		//FinalColor = DestColor * (1.0f - SrcAlpha) + SrcColor * SrcAlpha
		BS_ALPHABLEND.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		BS_ALPHABLEND.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		BS_ALPHABLEND.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

		BS_ALPHABLEND.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		BS_ALPHABLEND.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		BS_ALPHABLEND.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		BS_ALPHABLEND.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		hr = createBState(VNAME2WSTR(BS_ALPHABLEND), BS_ALPHABLEND);

		//듀얼 소스 블렌딩
		D3D11_BLEND_DESC BS_DUALSOURCEBLEND;
		ZeroMemory(&BS_DUALSOURCEBLEND, sizeof(D3D11_BLEND_DESC));

		BS_DUALSOURCEBLEND.AlphaToCoverageEnable = FALSE;
		BS_DUALSOURCEBLEND.IndependentBlendEnable = FALSE;
		BS_DUALSOURCEBLEND.RenderTarget[0].BlendEnable = TRUE;

		//Src0Color = texColor
		//ALPHA = MAX(Src0Color.R, Src0Color.G, Src0Color.B)
		//Src1Color = (Red : 1.0f - ALPHA, Green : 1.0f - ALPHA, Blue : 1.0f - ALPHA)

		//FinalColor = DestColor * Src1Color + SrcColor * 1.0f
		BS_DUALSOURCEBLEND.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		BS_DUALSOURCEBLEND.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		BS_DUALSOURCEBLEND.RenderTarget[0].DestBlend = D3D11_BLEND_SRC1_COLOR;

		BS_DUALSOURCEBLEND.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		BS_DUALSOURCEBLEND.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		BS_DUALSOURCEBLEND.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

		BS_DUALSOURCEBLEND.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		hr = createBState(VNAME2WSTR(BS_DUALSOURCEBLEND), BS_DUALSOURCEBLEND);


		//Depth-Stencil 기본 설정값
		//DSS_DEFAULT.DepthEnable = TRUE;
		//DSS_DEFAULT.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		//DSS_DEFAULT.DepthFunc = D3D11_COMPARISON_LESS;
		//DSS_DEFAULT.StencilEnable = FALSE;
		//DSS_DEFAULT.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		//DSS_DEFAULT.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		//DSS_DEFAULT.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		//DSS_DEFAULT.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		//DSS_DEFAULT.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		//DSS_DEFAULT.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		//DSS_DEFAULT.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		//DSS_DEFAULT.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		//DSS_DEFAULT.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		//DSS_DEFAULT.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		//기본 깊이 스텐실 스테이트
		D3D11_DEPTH_STENCIL_DESC DSS_DEFAULT;
		ZeroMemory(&DSS_DEFAULT, sizeof(D3D11_DEPTH_STENCIL_DESC));

		DSS_DEFAULT.DepthEnable = TRUE;
		DSS_DEFAULT.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		DSS_DEFAULT.DepthFunc = D3D11_COMPARISON_LESS;

		hr = createDSState(VNAME2WSTR(DSS_DEFAULT), DSS_DEFAULT);

		//후위 순위 먼저 렌더
		D3D11_DEPTH_STENCIL_DESC DSS_COMPGREATER;
		ZeroMemory(&DSS_COMPGREATER, sizeof(D3D11_DEPTH_STENCIL_DESC));

		DSS_COMPGREATER.DepthEnable = TRUE;
		DSS_COMPGREATER.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		DSS_COMPGREATER.DepthFunc = D3D11_COMPARISON_GREATER;

		hr = createDSState(VNAME2WSTR(DSS_COMPGREATER), DSS_COMPGREATER);

		//비교후 기입 안함
		D3D11_DEPTH_STENCIL_DESC DSS_COMPLESS_NOWRITE;
		ZeroMemory(&DSS_COMPLESS_NOWRITE, sizeof(D3D11_DEPTH_STENCIL_DESC));

		DSS_COMPLESS_NOWRITE.DepthEnable = TRUE;
		DSS_COMPLESS_NOWRITE.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		DSS_COMPLESS_NOWRITE.DepthFunc = D3D11_COMPARISON_LESS;

		hr = createDSState(VNAME2WSTR(DSS_COMPLESS_NOWRITE), DSS_COMPLESS_NOWRITE);

		//깊이 스텐실 모두 사용안함
		D3D11_DEPTH_STENCIL_DESC DSS_DISABLE;
		ZeroMemory(&DSS_DISABLE, sizeof(D3D11_DEPTH_STENCIL_DESC));

		DSS_DISABLE.DepthEnable = FALSE;

		hr = createDSState(VNAME2WSTR(DSS_DISABLE), DSS_DISABLE);

		return hr;
	}
}