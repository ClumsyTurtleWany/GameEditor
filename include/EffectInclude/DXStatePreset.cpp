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

		//Sampler State �⺻ ������
		/*Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;		���Ͱ� - �ؽ�ó���� �������� ������ ���� ����
		AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;			UV�ּ� ��� - 0.0f~1.0f�� ����� UV��ǥ�� ���� ó�����
		AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		MipLODBias = 0.0f;								LOD ���� ������
		MaxAnisotropy = 1;								���漺 �Ӹʴܰ� ����
		ComparisonFunc = D3D11_COMPARISON_NEVER;		���ø� ���� �� �Լ�
		BorderColor[0] = 1.0f;							//�ּ� ��� Border������ ������ ��� TexCoord��ǥ�� �ο��� ����
		BorderColor[1] = 1.0f;
		BorderColor[2] = 1.0f;
		BorderColor[3] = 1.0f;
		MinLOD = -FLT_MAX;								//�ּ� �Ӹ� LOD����
		MaxLOD = +FLT_MAX;*/							//�ִ� �Ӹ� LOD���� : MinLOD���� Ŀ���Ѵ�.

		//�⺻ ���÷� ������Ʈ - ���� ���� �ؽ�ó ���͸� + Ŭ���� ���
		D3D11_SAMPLER_DESC SS_DEFAULT;
		ZeroMemory(&SS_DEFAULT, sizeof(D3D11_SAMPLER_DESC));
		SS_DEFAULT.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SS_DEFAULT.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		SS_DEFAULT.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		SS_DEFAULT.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

		hr = createSState(VNAME2WSTR(SS_DEFAULT), SS_DEFAULT);

		//���� ���� �ؽ�ó ���͸� - ���� ���
		D3D11_SAMPLER_DESC SS_LINEAR_WRAP;
		ZeroMemory(&SS_LINEAR_WRAP, sizeof(D3D11_SAMPLER_DESC));
		SS_LINEAR_WRAP.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SS_LINEAR_WRAP.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SS_LINEAR_WRAP.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SS_LINEAR_WRAP.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		hr = createSState(VNAME2WSTR(SS_LINEAR_WRAP), SS_LINEAR_WRAP);

		//����Ʈ �ؽ�ó ���͸� - ���� ���
		D3D11_SAMPLER_DESC SS_POINT_WRAP;
		ZeroMemory(&SS_POINT_WRAP, sizeof(D3D11_SAMPLER_DESC));
		SS_POINT_WRAP.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		SS_POINT_WRAP.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SS_POINT_WRAP.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SS_POINT_WRAP.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		hr = createSState(VNAME2WSTR(SS_POINT_WRAP), SS_POINT_WRAP);

		//Rasterizer State �⺻ ������
		/*FillMode = D3D11_FILL_SOLID;					//ä��� ���
		CullMode = D3D11_CULL_BACK;						//���� ���� ���
		FrontCounterClockwise = FALSE;					//��, �� ���� ���� �������
		DepthBias = 0;									//���� ������
		DepthBiasClamp = 0.0f;							//���� �������� ����/����
		SlopeScaledDepthBias = 0.0f;					//���� ������ ������ ������
		DepthClipEnable = TRUE;							//���̿� ���� ���� ���� ���࿩��
		ScissorEnable = FALSE;							//���� �簢�� Ȱ��ȭ ����
		MultisampleEnable = FALSE;						//��Ƽ ���ø� ��� ����
		AntialiasedLineEnable = FALSE;*/				//���� ���� ��Ƽ �ٸ���� ���� ����

		//���̾� ������
		D3D11_RASTERIZER_DESC RS_WIREFRAME;
		ZeroMemory(&RS_WIREFRAME, sizeof(D3D11_RASTERIZER_DESC));
		RS_WIREFRAME.FillMode = D3D11_FILL_WIREFRAME;
		RS_WIREFRAME.CullMode = D3D11_CULL_BACK;
		RS_WIREFRAME.DepthClipEnable = TRUE;

		hr = createRState(VNAME2WSTR(RS_WIREFRAME), RS_WIREFRAME);

		//�ָ���
		D3D11_RASTERIZER_DESC RS_SOLID;
		ZeroMemory(&RS_SOLID, sizeof(D3D11_RASTERIZER_DESC));
		RS_SOLID.FillMode = D3D11_FILL_SOLID;
		RS_SOLID.CullMode = D3D11_CULL_BACK;
		RS_SOLID.DepthClipEnable = TRUE;

		hr = createRState(VNAME2WSTR(RS_SOLID), RS_SOLID);

		//�ָ��� : �ø� �۾� ����
		D3D11_RASTERIZER_DESC RS_SOLID_NOCULL;
		ZeroMemory(&RS_SOLID_NOCULL, sizeof(D3D11_RASTERIZER_DESC));
		RS_SOLID_NOCULL.FillMode = D3D11_FILL_SOLID;
		RS_SOLID_NOCULL.CullMode = D3D11_CULL_NONE;
		RS_SOLID_NOCULL.DepthClipEnable = TRUE;

		hr = createRState(VNAME2WSTR(RS_SOLID_NOCULL), RS_SOLID_NOCULL);

		//���̾� ������ : �ø� �۾� ����
		D3D11_RASTERIZER_DESC RS_WIREFRAME_NOCULL;
		ZeroMemory(&RS_WIREFRAME_NOCULL, sizeof(D3D11_RASTERIZER_DESC));
		RS_WIREFRAME_NOCULL.FillMode = D3D11_FILL_WIREFRAME;
		RS_WIREFRAME_NOCULL.CullMode = D3D11_CULL_NONE;
		RS_WIREFRAME_NOCULL.DepthClipEnable = TRUE;

		hr = createRState(VNAME2WSTR(RS_WIREFRAME_NOCULL), RS_WIREFRAME_NOCULL);

		//�ָ��� : �ո� �ø�
		D3D11_RASTERIZER_DESC RS_SOLID_FRONTCULL;
		ZeroMemory(&RS_SOLID_FRONTCULL, sizeof(D3D11_RASTERIZER_DESC));
		RS_SOLID_FRONTCULL.FillMode = D3D11_FILL_SOLID;
		RS_SOLID_FRONTCULL.CullMode = D3D11_CULL_FRONT;
		RS_SOLID_FRONTCULL.DepthClipEnable = TRUE;

		hr = createRState(VNAME2WSTR(RS_SOLID_FRONTCULL), RS_SOLID_FRONTCULL);

		//���̾� ������ : �ո� �ø�
		D3D11_RASTERIZER_DESC RS_WIREFRAME_FRONTCULL;
		ZeroMemory(&RS_WIREFRAME_FRONTCULL, sizeof(D3D11_RASTERIZER_DESC));
		RS_WIREFRAME_FRONTCULL.FillMode = D3D11_FILL_WIREFRAME;
		RS_WIREFRAME_FRONTCULL.CullMode = D3D11_CULL_FRONT;
		RS_WIREFRAME_FRONTCULL.DepthClipEnable = TRUE;

		hr = createRState(VNAME2WSTR(RS_WIREFRAME_FRONTCULL), RS_WIREFRAME_FRONTCULL);

		//Blend State �⺻ ������
		/*AlphaToCoverageEnable = FALSE;											//Alpha To Coverage��� ��Ƽ ���ø� ��� ��� ����
		IndependentBlendEnable = FALSE;												//�������� ����Ÿ�� ���� ��� ����
		RenderTarget[0].BlendEnable = FALSE;										//���� �ɼ� Ȱ��ȭ ����
		RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;									//�߰��� ��� ������ ���� ���
		RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;								//���� ��� ������ ���� ���
		RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;								//������ ���� ���� ���
		RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;							//�߰��� ��� ���İ� ���� ���
		RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;							//���� ��� ���İ� ���� ���
		RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;							//���İ��� ���� ���� ���
		RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;*/		//���� Ÿ������ ����� ���� ä�� ����ũ

		//�⺻ ���� ������Ʈ : ���� ����
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

		//��Ƽ ���ø� ���� �׽��� - �����׽�Ʈ�� ���� ��Ƽ �ٸ����
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



		//���� ����
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

		//��� �ҽ� ����
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


		//Depth-Stencil �⺻ ������
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

		//�⺻ ���� ���ٽ� ������Ʈ
		D3D11_DEPTH_STENCIL_DESC DSS_DEFAULT;
		ZeroMemory(&DSS_DEFAULT, sizeof(D3D11_DEPTH_STENCIL_DESC));

		DSS_DEFAULT.DepthEnable = TRUE;
		DSS_DEFAULT.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		DSS_DEFAULT.DepthFunc = D3D11_COMPARISON_LESS;

		hr = createDSState(VNAME2WSTR(DSS_DEFAULT), DSS_DEFAULT);

		//���� ���� ���� ����
		D3D11_DEPTH_STENCIL_DESC DSS_COMPGREATER;
		ZeroMemory(&DSS_COMPGREATER, sizeof(D3D11_DEPTH_STENCIL_DESC));

		DSS_COMPGREATER.DepthEnable = TRUE;
		DSS_COMPGREATER.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		DSS_COMPGREATER.DepthFunc = D3D11_COMPARISON_GREATER;

		hr = createDSState(VNAME2WSTR(DSS_COMPGREATER), DSS_COMPGREATER);

		//���� ���� ����
		D3D11_DEPTH_STENCIL_DESC DSS_COMPLESS_NOWRITE;
		ZeroMemory(&DSS_COMPLESS_NOWRITE, sizeof(D3D11_DEPTH_STENCIL_DESC));

		DSS_COMPLESS_NOWRITE.DepthEnable = TRUE;
		DSS_COMPLESS_NOWRITE.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		DSS_COMPLESS_NOWRITE.DepthFunc = D3D11_COMPARISON_LESS;

		hr = createDSState(VNAME2WSTR(DSS_COMPLESS_NOWRITE), DSS_COMPLESS_NOWRITE);

		//���� ���ٽ� ��� ������
		D3D11_DEPTH_STENCIL_DESC DSS_DISABLE;
		ZeroMemory(&DSS_DISABLE, sizeof(D3D11_DEPTH_STENCIL_DESC));

		DSS_DISABLE.DepthEnable = FALSE;

		hr = createDSState(VNAME2WSTR(DSS_DISABLE), DSS_DISABLE);

		return hr;
	}
}