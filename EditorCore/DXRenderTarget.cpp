#include "DXRenderTarget.hpp"

DXRenderTarget::DXRenderTarget()
{
}

void DXRenderTarget::SetDevice(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_pd3dDevice = device;
	m_pImmediateContext = context;
}

bool DXRenderTarget::Create(float x, float y, float width, float height)
{
	// View Port
	m_viewPort.TopLeftX = x;
	m_viewPort.TopLeftY = y;
	m_viewPort.Width = width;
	m_viewPort.Height = height;
	m_viewPort.MinDepth = 0.0f;
	m_viewPort.MaxDepth = 1.0f;

	// Texture Render Target View
	if (m_pTexture == nullptr)
	{
		m_pTexture = new DXTexture;
		m_pTexture->SetDevice(m_pd3dDevice, m_pImmediateContext);
		if (!m_pTexture->CreateRenderTarget(width, height))
		{
			OutputDebugStringA("DXRenderTarget::create::Failed Create Render Target Texture.\n");
			//delete m_pTexture;
			//m_pTexture = nullptr;
			return false;
		}
	}

	HRESULT rst = m_pd3dDevice->CreateRenderTargetView(m_pTexture->GetResource(), NULL, m_pRenderTargetView.GetAddressOf());
	if (FAILED(rst))
	{
		OutputDebugStringA("DXRenderTarget::create::Failed Create Render Target View.\n");
		return false;
	}

	// Depth Stencil View
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1; // 0으로 하면 9단계 까지 생성
	desc.ArraySize = 1; // 1개만 생성
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //DXGI_FORMAT_R24G8_TYPELESS; // 32bit, 24bit는 Z버퍼(Depth)로 사용하고, 8bit는 스텐실 버퍼로 사용 하겠다.
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL; // 중요! 어디에 적용 할 것인지 정하는 것.
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	rst = m_pd3dDevice->CreateTexture2D(&desc, NULL, m_pDSTexture.GetAddressOf());
	if (FAILED(rst))
	{
		OutputDebugStringA("DXRenderTarget::create::Failed Create Depth Stencil Texture.\n");
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC DSdesc;
	ZeroMemory(&DSdesc, sizeof(DSdesc));
	DSdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Depth는 노멀라이즈 하지말고, 스텐실은 정수형으로 생성. Texture 생성과 비트를 맞춰 줘야함.
	DSdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSdesc.Flags = 0;

	rst = m_pd3dDevice->CreateDepthStencilView(m_pDSTexture.Get(), &DSdesc, m_pDepthStencilView.GetAddressOf());
	if (FAILED(rst))
	{
		OutputDebugStringA("DXRenderTarget::create::Failed Create Depth Stencil View.\n");
		return false;
	}

	// Create Shader
	if (1)
	{
		//ID3DBlob* vsCode = DXShaderManager::GetInstance()->GetVSCode("../include/Core/HLSL/VS_StaticMesh.hlsl");
		//D3D11_INPUT_ELEMENT_DESC InputElementDescs[] =
		//{
		//	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		//	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		//	{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 12 == float * 3 // Vertex의 Color 시작 바이트.
		//	{"TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0} // 28 == float * 28 // Vertex의 Texture 시작 바이트.
		//};

		//UINT NumElements = sizeof(InputElementDescs) / sizeof(InputElementDescs[0]);
		//HRESULT result = m_pd3dDevice->CreateInputLayout(InputElementDescs, NumElements, vsCode->GetBufferPointer(), vsCode->GetBufferSize(), &InputLayout);

	
		InputLayout = DXShaderManager::GetInstance()->GetInputLayout(L"StaticMesh");

		VertexList.assign(4, Vertex());
		VertexList[0].Pos = { -1.0f, +1.0f, 0.0f }; // p1-LT
		VertexList[1].Pos = { +1.0f, +1.0f, 0.0f }; // p2-RT
		VertexList[2].Pos = { -1.0f, -1.0f, 0.0f }; // p3-LB
		VertexList[3].Pos = { +1.0f, -1.0f, 0.0f }; // p4-LB

		VertexList[0].Color = { 1.0f, 1.0f, 1.0f, 1.0f }; // p1-LT
		VertexList[1].Color = { 1.0f, 1.0f, 1.0f, 1.0f }; // p2-RT
		VertexList[2].Color = { 1.0f, 1.0f, 1.0f, 1.0f }; // p3-LB
		VertexList[3].Color = { 1.0f, 1.0f, 1.0f, 1.0f }; // p4-LB

		VertexList[0].Texture = { 0.0f, 0.0f }; // p1-LT
		VertexList[1].Texture = { 1.0f, 0.0f }; // p2-RT
		VertexList[2].Texture = { 0.0f, 1.0f }; // p3-LB
		VertexList[3].Texture = { 1.0f, 1.0f }; // p4-LB

		IndexList.assign(6, 0);
		IndexList[0] = 0;
		IndexList[1] = 1;
		IndexList[2] = 2;
		IndexList[3] = 2;
		IndexList[4] = 1;
		IndexList[5] = 3;

		VertexBuffer = DXShaderManager::GetInstance()->CreateVertexBuffer(VertexList);
		IndexBuffer = DXShaderManager::GetInstance()->CreateIndexBuffer(IndexList);
		
		VertexShader = DXShaderManager::GetInstance()->GetVertexShader(L"StaticMesh");
		PixelShader = DXShaderManager::GetInstance()->GetPixelShader(L"StaticMesh");

		SamplerState = DXSamplerState::pDefaultSamplerState;
		RSState = DXSamplerState::pDefaultRSSolid;
	}

	return true;
}

bool DXRenderTarget::Begin()
{
	ID3D11ShaderResourceView* resourceView = NULL;
	m_pImmediateContext->PSSetShaderResources(0, 1, &resourceView); // 레지스터 0번

	// Save Old
	UINT nViewPorts = 1;
	m_pImmediateContext->RSGetViewports(&nViewPorts, m_OldViewPort);
	m_pImmediateContext->OMGetRenderTargets(1, &m_pOldRenderTargetView, &m_pOldDepthStencilView);

	if (m_pOldRenderTargetView != nullptr)
	{
		float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // R, G, B, A 순 0 ~ 1.0사이 값 1.0 == 255
		m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), color);
	}

	if (m_pOldDepthStencilView != nullptr)
	{
		m_pImmediateContext->ClearDepthStencilView(m_pOldDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	// Apply New
	ID3D11RenderTargetView* pRenderTargetView = nullptr;
	m_pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr); // nullptr로 되어있는 렌더 타겟 뷰로 한번 초기화 해줘야 함. 직접 nullptr넣으면 에러 생김.
	m_pImmediateContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());

	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // R, G, B, A 순 0 ~ 1.0사이 값 1.0 == 255
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), color);
	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_pImmediateContext->RSSetViewports(1, &m_viewPort);

	return true;
}

bool DXRenderTarget::End()
{
	//ID3D11ShaderResourceView* resourceView = NULL;
	//m_pImmediateContext->PSSetShaderResources(0, 1, &resourceView); // 레지스터 0번

	ID3D11RenderTargetView* pRenderTargetView = nullptr;
	m_pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);
	m_pImmediateContext->OMSetRenderTargets(1, &m_pOldRenderTargetView, m_pOldDepthStencilView);

	m_pImmediateContext->RSSetViewports(1, &m_OldViewPort[0]);

	if (m_pOldRenderTargetView != nullptr)
	{
		m_pOldRenderTargetView->Release();
		m_pOldRenderTargetView = nullptr;
	}

	if (m_pOldDepthStencilView != nullptr)
	{
		m_pOldDepthStencilView->Release();
		m_pOldDepthStencilView = nullptr;
	}

	return true;
}

bool DXRenderTarget::Render()
{
	m_pImmediateContext->IASetInputLayout(InputLayout);
	UINT Strides = sizeof(Vertex); // 정점 1개의 바이트 용량
	UINT Offsets = 0; // 정점 버퍼에서 출발 지점(바이트)
	m_pImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Strides, &Offsets);
	m_pImmediateContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_pImmediateContext->VSSetShader(VertexShader, NULL, 0);
	m_pImmediateContext->PSSetShader(PixelShader, NULL, 0);

	m_pImmediateContext->UpdateSubresource(VertexBuffer, 0, NULL, &VertexList.at(0), 0, 0);

	ID3D11ShaderResourceView* resourceView = m_pTexture->GetResourceView();
	m_pImmediateContext->PSSetShaderResources(0, 1, &resourceView);

	m_pImmediateContext->RSSetState(RSState);

	m_pImmediateContext->DrawIndexed(static_cast<UINT>(IndexList.size()), 0, 0);

	return true;
}

bool DXRenderTarget::Release()
{
	/*if (m_pRenderTargetView != nullptr)
	{
		m_pRenderTargetView->Release();
		m_pRenderTargetView = nullptr;
	}

	if (m_pDepthStencilView != nullptr)
	{
		m_pDepthStencilView->Release();
		m_pDepthStencilView = nullptr;
	}

	if (m_pDSTexture != nullptr)
	{
		m_pDSTexture->Release();
		m_pDSTexture = nullptr;
	}*/

	if (m_pTexture != nullptr)
	{
		m_pTexture->Release();
		delete m_pTexture;
		m_pTexture = nullptr;
	}

	if (m_pOldRenderTargetView != nullptr)
	{
		m_pOldRenderTargetView->Release();
		m_pOldRenderTargetView = nullptr;
	}

	if (m_pOldDepthStencilView != nullptr)
	{
		m_pOldDepthStencilView->Release();
		m_pOldDepthStencilView = nullptr;
	}

	return true;
}