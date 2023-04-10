#include "DXRenderTarget.hpp"

DXRenderTarget::DXRenderTarget()
{
}

DXRenderTarget::~DXRenderTarget()
{
	Release();
}

bool DXRenderTarget::Create(float x, float y, float width, float height, UINT count, DXGI_FORMAT format)
{
	// View Port
	m_viewPort.TopLeftX = x;
	m_viewPort.TopLeftY = y;
	m_viewPort.Width = width;
	m_viewPort.Height = height;
	m_viewPort.MinDepth = 0.0f;
	m_viewPort.MaxDepth = 1.0f;

	Count = count;

	// Texture Render Target View
	if (m_pTexture == nullptr)
	{
		m_pTexture = new DXTexture;
		m_pTexture->SetDevice(DXDevice::g_pd3dDevice, DXDevice::g_pImmediateContext);
		if (!m_pTexture->CreateRenderTarget(width, height, count, format))
		{
			OutputDebugStringA("DXRenderTarget::create::Failed Create Render Target Texture.\n");
			delete m_pTexture;
			m_pTexture = nullptr;
			return false;
		}
	}

	if (Count > 1)
	{
		D3D11_RENDER_TARGET_VIEW_DESC descRTV;
		ZeroMemory(&descRTV, sizeof(descRTV));
		descRTV.Format = format;
		descRTV.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		descRTV.Texture2DArray.ArraySize = 1;
		//descRTV.Texture2DArray.FirstArraySlice = 0;
		descRTV.Texture2DArray.MipSlice = 0;

		for (UINT idx = 0; idx < Count; idx++)
		{
			ID3D11RenderTargetView* rtv = nullptr;
			descRTV.Texture2DArray.FirstArraySlice = idx;
			HRESULT rst = DXDevice::g_pd3dDevice->CreateRenderTargetView(m_pTexture->GetResource(), &descRTV, &rtv);
			if (FAILED(rst))
			{
				OutputDebugStringA("DXRenderTarget::create::Failed Create Render Target View.\n");
				return false;
			}
			RTVList.push_back(rtv);
		}
	}
	else
	{
		ID3D11RenderTargetView* rtv = nullptr;
		HRESULT rst = DXDevice::g_pd3dDevice->CreateRenderTargetView(m_pTexture->GetResource(), NULL, &rtv);
		if (FAILED(rst))
		{
			OutputDebugStringA("DXRenderTarget::create::Failed Create Render Target View.\n");
			return false;
		}
		RTVList.push_back(rtv);
	}

	// Depth Stencil View
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = static_cast<UINT>(width);
	desc.Height = static_cast<UINT>(height);
	desc.MipLevels = 1; // 0으로 하면 9단계 까지 생성
	desc.ArraySize = Count; // 1개만 생성
	desc.Format = DXGI_FORMAT_R24G8_TYPELESS; //DXGI_FORMAT_D24_UNORM_S8_UINT; //DXGI_FORMAT_R24G8_TYPELESS; // 32bit, 24bit는 Z버퍼(Depth)로 사용하고, 8bit는 스텐실 버퍼로 사용 하겠다.
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE; // 중요! 어디에 적용 할 것인지 정하는 것.
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	HRESULT rst = DXDevice::g_pd3dDevice->CreateTexture2D(&desc, NULL, &m_pDSTexture);
	if (FAILED(rst))
	{
		OutputDebugStringA("DXRenderTarget::create::Failed Create Depth Stencil Texture.\n");
		return false;
	}


	// Depth Stencil View
	if (Count > 1)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC DSdesc;
		ZeroMemory(&DSdesc, sizeof(DSdesc));
		DSdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Depth는 노멀라이즈 하지말고, 스텐실은 정수형으로 생성. Texture 생성과 비트를 맞춰 줘야함.
		DSdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		DSdesc.Texture2DArray.ArraySize = 1;
		//DSdesc.Texture2DArray.FirstArraySlice = 0;
		DSdesc.Texture2DArray.MipSlice = 0;

		for (UINT idx = 0; idx < Count; idx++)
		{
			ID3D11DepthStencilView* dsv = nullptr;
			DSdesc.Texture2DArray.FirstArraySlice = idx;
			rst = DXDevice::g_pd3dDevice->CreateDepthStencilView(m_pDSTexture, &DSdesc, &dsv);
			if (FAILED(rst))
			{
				OutputDebugStringA("DXRenderTarget::create::Failed Create Depth Stencil View.\n");
				return false;
			}
			DSVList.push_back(dsv);
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC dsvSrvDesc;
		ZeroMemory(&dsvSrvDesc, sizeof(dsvSrvDesc));
		dsvSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		dsvSrvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		dsvSrvDesc.Texture2DArray.ArraySize = Count;
		dsvSrvDesc.Texture2DArray.FirstArraySlice = 0;
		dsvSrvDesc.Texture2DArray.MostDetailedMip = 0;
		dsvSrvDesc.Texture2DArray.MipLevels = 1;

		rst = DXDevice::g_pd3dDevice->CreateShaderResourceView(m_pDSTexture, &dsvSrvDesc, &DSVShaderResourceView);
		if (FAILED(rst))
		{
			OutputDebugStringA("DXRenderTarget::Create::Failed Create Depth Stencil Texture Shader Resource View.\n");
			return false;
		}
	}
	else
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC DSdesc;
		ZeroMemory(&DSdesc, sizeof(DSdesc));
		DSdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Depth는 노멀라이즈 하지말고, 스텐실은 정수형으로 생성. Texture 생성과 비트를 맞춰 줘야함.
		DSdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		DSdesc.Flags = 0;

		ID3D11DepthStencilView* dsv = nullptr;
		rst = DXDevice::g_pd3dDevice->CreateDepthStencilView(m_pDSTexture, &DSdesc, &dsv);
		if (FAILED(rst))
		{
			OutputDebugStringA("DXRenderTarget::Create::Failed Create Depth Stencil View.\n");
			return false;
		}
		DSVList.push_back(dsv);

		rst = DXDevice::g_pd3dDevice->CreateShaderResourceView(m_pDSTexture, NULL, &DSVShaderResourceView);
		if (FAILED(rst))
		{
			OutputDebugStringA("DXRenderTarget::Create::Failed Create Depth Stencil Texture Shader Resource View.\n");
			return false;
		}
	}

	if (Count > 1)
	{
		TextureIndexBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<TIndex>(TextureIndex);
	}


	// Create Shader
	if (1)
	{
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
		PixelShader = DXShaderManager::GetInstance()->GetPixelShader(L"T_UI");
		ArrayTexturePixelShader = DXShaderManager::GetInstance()->GetPixelShader(L"2DArray");

		TransformBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<TransformMatrix>(TransformData);
		CameraMatrixBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<CameraMatrix>(CameraMatrixData);

		DXDevice::g_pImmediateContext->UpdateSubresource(CameraMatrixBuffer, 0, NULL, &CameraMatrixData, 0, 0);
		DXDevice::g_pImmediateContext->UpdateSubresource(TransformBuffer, 0, NULL, &TransformData, 0, 0);
	}

	return true;
}

bool DXRenderTarget::Begin(UINT index)
{
	if (RTVList.empty() || DSVList.empty())
	{
		return false;
	}

	ID3D11ShaderResourceView* resourceView = NULL;
	DXDevice::g_pImmediateContext->PSSetShaderResources(0, 1, &resourceView); // 레지스터 0번

	// Save Old
	UINT nViewPorts = 1;
	DXDevice::g_pImmediateContext->RSGetViewports(&nViewPorts, m_OldViewPort);
	DXDevice::g_pImmediateContext->OMGetRenderTargets(1, &m_pOldRenderTargetView, &m_pOldDepthStencilView);

	if (m_pOldRenderTargetView != nullptr)
	{
		float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // R, G, B, A 순 0 ~ 1.0사이 값 1.0 == 255
		DXDevice::g_pImmediateContext->ClearRenderTargetView(m_pOldRenderTargetView, color);
	}

	if (m_pOldDepthStencilView != nullptr)
	{
		DXDevice::g_pImmediateContext->ClearDepthStencilView(m_pOldDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	// Apply New
	ID3D11RenderTargetView* pRenderTargetView = nullptr;
	ID3D11DepthStencilView* pDepthStencilView = nullptr;
	DXDevice::g_pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView); // nullptr로 되어있는 렌더 타겟 뷰로 한번 초기화 해줘야 함. 직접 nullptr넣으면 에러 생김.
	DXDevice::g_pImmediateContext->OMSetRenderTargets(1, &RTVList[index], DSVList[index]);

	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // R, G, B, A 순 0 ~ 1.0사이 값 1.0 == 255
	DXDevice::g_pImmediateContext->ClearRenderTargetView(RTVList[index], color);
	//DXDevice::g_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	DXDevice::g_pImmediateContext->ClearDepthStencilView(DSVList[index], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	DXDevice::g_pImmediateContext->RSSetViewports(1, &m_viewPort);
	
	return true;
}

bool DXRenderTarget::End(UINT index)
{
	if (RTVList.empty() || DSVList.empty())
	{
		return false;
	}
	//ID3D11ShaderResourceView* resourceView = NULL;
	//DXDevice::g_pImmediateContext->PSSetShaderResources(0, 1, &resourceView); // 레지스터 0번

	ID3D11RenderTargetView* pRenderTargetView = nullptr;
	ID3D11DepthStencilView* pDepthStencilView = nullptr;
	DXDevice::g_pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);
	DXDevice::g_pImmediateContext->OMSetRenderTargets(1, &m_pOldRenderTargetView, m_pOldDepthStencilView);

	DXDevice::g_pImmediateContext->RSSetViewports(1, &m_OldViewPort[0]);

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

bool DXRenderTarget::Render(UINT index, float x, float y, float width, float height)
{
	if (RTVList.empty() || DSVList.empty())
	{
		return false;
	}

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = x;
	viewport.TopLeftY = y;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	DXDevice::g_pImmediateContext->RSSetViewports(1, &viewport);

	TextureIndex.Index = index;

	DXDevice::g_pImmediateContext->IASetInputLayout(InputLayout);
	UINT Strides = sizeof(Vertex); // 정점 1개의 바이트 용량
	UINT Offsets = 0; // 정점 버퍼에서 출발 지점(바이트)
	DXDevice::g_pImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Strides, &Offsets);
	DXDevice::g_pImmediateContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DXDevice::g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DXDevice::g_pImmediateContext->VSSetConstantBuffers(0, 1, &TransformBuffer);
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(1, 1, &CameraMatrixBuffer);

	DXDevice::g_pImmediateContext->VSSetShader(VertexShader, NULL, 0);

	if (Count > 1)
	{
		DXDevice::g_pImmediateContext->PSSetShader(ArrayTexturePixelShader, NULL, 0);
	}
	else
	{
		DXDevice::g_pImmediateContext->PSSetShader(PixelShader, NULL, 0);
	}

	DXDevice::g_pImmediateContext->UpdateSubresource(TextureIndexBuffer, 0, NULL, &TextureIndex, 0, 0);
	DXDevice::g_pImmediateContext->PSSetConstantBuffers(0, 1, &TextureIndexBuffer);

	DXDevice::g_pImmediateContext->UpdateSubresource(VertexBuffer, 0, NULL, &VertexList.at(0), 0, 0);

	ID3D11ShaderResourceView* resourceView = m_pTexture->GetResourceView();
	DXDevice::g_pImmediateContext->PSSetShaderResources(0, 1, &resourceView);

	DXDevice::g_pImmediateContext->RSSetState(DXSamplerState::pDefaultRSSolid);

	DXDevice::g_pImmediateContext->DrawIndexed(static_cast<UINT>(IndexList.size()), 0, 0);

	return true;
}

bool DXRenderTarget::RenderDepthTexture(UINT index, float x, float y, float width, float height)
{
	if (RTVList.empty() || DSVList.empty())
	{
		return false;
	}

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = x;
	viewport.TopLeftY = y;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	DXDevice::g_pImmediateContext->RSSetViewports(1, &viewport);

	TextureIndex.Index = index;
	
	DXDevice::g_pImmediateContext->IASetInputLayout(InputLayout);
	UINT Strides = sizeof(Vertex); // 정점 1개의 바이트 용량
	UINT Offsets = 0; // 정점 버퍼에서 출발 지점(바이트)
	DXDevice::g_pImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Strides, &Offsets);
	DXDevice::g_pImmediateContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DXDevice::g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DXDevice::g_pImmediateContext->VSSetConstantBuffers(0, 1, &TransformBuffer);
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(1, 1, &CameraMatrixBuffer);

	DXDevice::g_pImmediateContext->VSSetShader(VertexShader, NULL, 0);

	if (Count > 1)
	{
		DXDevice::g_pImmediateContext->PSSetShader(ArrayTexturePixelShader, NULL, 0);
	}
	else
	{
		DXDevice::g_pImmediateContext->PSSetShader(PixelShader, NULL, 0);
	}

	DXDevice::g_pImmediateContext->UpdateSubresource(TextureIndexBuffer, 0, NULL, &TextureIndex, 0, 0);
	DXDevice::g_pImmediateContext->PSSetConstantBuffers(0, 1, &TextureIndexBuffer);

	DXDevice::g_pImmediateContext->UpdateSubresource(VertexBuffer, 0, NULL, &VertexList.at(0), 0, 0);

	ID3D11ShaderResourceView* resourceView = DSVShaderResourceView;
	DXDevice::g_pImmediateContext->PSSetShaderResources(0, 1, &resourceView);

	DXDevice::g_pImmediateContext->RSSetState(DXSamplerState::pDefaultRSSolid);

	DXDevice::g_pImmediateContext->DrawIndexed(static_cast<UINT>(IndexList.size()), 0, 0);

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
	}*/

	if (m_pDSTexture != nullptr)
	{
		m_pDSTexture->Release();
		m_pDSTexture = nullptr;
	}

	for (size_t idx = 0; idx < RTVList.size(); idx++)
	{
		if (RTVList[idx] != nullptr)
		{
			RTVList[idx]->Release();
			RTVList[idx] = nullptr;
		}
	}
	RTVList.clear();

	for (size_t idx = 0; idx < DSVList.size(); idx++)
	{
		if (DSVList[idx] != nullptr)
		{
			DSVList[idx]->Release();
			DSVList[idx] = nullptr;
		}
	}
	DSVList.clear();

	if (DSVShaderResourceView != nullptr)
	{
		DSVShaderResourceView->Release();
		DSVShaderResourceView = nullptr;
	}

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