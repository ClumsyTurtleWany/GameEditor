#include "GameCore.h"

bool GameCore::CoreInitialize()
{
	if (!DXDevice::getInstance()->Create(_hWnd))
	{
		OutputDebugString(L"WanyCore::DXDevice::Failed Initialize.\n");
		return false;
	}

	Input::getInstance()->setWndHandle(_hWnd);
	if (!Input::getInstance()->initialize())
	{
		OutputDebugString(L"WanyCore::Input::Failed Initialize.\n");
		return false;
	}

	if (!Timer::getInstance()->initialize())
	{
		OutputDebugString(L"WanyCore::Timer::Failed Initialize.\n");
		return false;
	}

	/*DXShaderManager::getInstance()->setDevice(m_pd3dDevice, m_pImmediateContext);
	if (!DXShaderManager::getInstance()->initialize())
	{
		OutputDebugString(L"WanyCore::DXShaderManager::Failed Initialize.\n");
		return false;
	}*/

	DXTextureManager::getInstance()->setDevice(DEVICE->GetDevice(), DEVICE->GetContext());

	/*DXShaderBorderManager::getInstance()->setDevice(m_pd3dDevice, m_pImmediateContext);
	if (!DXShaderBorderManager::getInstance()->initialize())
	{
		OutputDebugString(L"WanyCore::DXShaderBorderManager::Failed Initialize.\n");
		return false;
	}*/

	if (!FMODSoundManager::getInstance()->Initialize())
	{
		OutputDebugString(L"WanyCore::FMODSoundManager::Failed Initialize.\n");
		return false;
	}

	// Sampler State
	if (!DXSamplerState::setState(DEVICE->GetDevice()))
	{
		OutputDebugString(L"WanyCore::DXSamplerState::Failed Set State.\n");
		return false;
	}


	// DXWriter
	if (!DXWriter::getInstance()->initialize())
	{
		OutputDebugString(L"WanyCore::DXWriter::Failed Initialize.\n");
		return false;
	}
	else
	{
		IDXGISurface* pBackBuffer;
		// Buffer�� Surface Ÿ������ ���� �� ���̴�. 0�� ������ ���纻(pBackBuffer��) ����.
		HRESULT rst = DEVICE->GetSwapChain()->GetBuffer(0, __uuidof(IDXGISurface), (void**)&pBackBuffer);
		if (SUCCEEDED(rst))
		{
			if (!DXWriter::getInstance()->setBuffer(pBackBuffer))
			{
				OutputDebugString(L"WanyCore::DXWriter::Failed Set Buffer.\n");
				return false;
			}
			pBackBuffer->Release();
		}
	}

	return Initialize();
}

bool GameCore::CoreFrame()
{
	if (!Input::getInstance()->frame())
	{
		OutputDebugString(L"WanyCore::Input::Failed Frame.\n");
		return false;
	}

	if (!Timer::getInstance()->frame())
	{
		OutputDebugString(L"WanyCore::Timer::Failed Frame.\n");
		return false;
	}

	if (!DXWriter::getInstance()->frame())
	{
		OutputDebugString(L"WanyCore::DXWriter::Failed Frame.\n");
		return false;
	}

	if (!FMODSoundManager::getInstance()->Frame())
	{
		OutputDebugString(L"WanyCore::FMODSoundManager::Failed frame.\n");
		return false;
	}

	return Frame();
}

bool GameCore::PreRender()
{
	//m_pImmediateContext->OMSetRenderTargets(1, &m_pRTV, NULL); // m_pRTV �� �Ѹ���.
	DEVICE->GetContext()->OMSetRenderTargets(1, &m_pRTV, m_pDepthStencilView); // Depth Stencil View �߰�.
	//float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // R, G, B, A �� 0 ~ 1.0���� �� 1.0 == 255
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // R, G, B, A �� 0 ~ 1.0���� �� 1.0 == 255
	DEVICE->GetContext()->ClearRenderTargetView(m_pRTV, color);
	DEVICE->GetContext()->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0); // Depth�� 1.0f, Stencil�� 0���� Ŭ����.
	//m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// View Port ����
	// ������ client ũ��� ����������, �̴ϸ��� ��� ����Ʈ ������ �ٲ� ��� ����.
	RECT rcClient = g_pWindow->getClientRect();
	float width = rcClient.right - rcClient.left;
	float height = rcClient.bottom - rcClient.top;
	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = width; //1366; 
	viewPort.Height = height; //786;
	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;
	DEVICE->GetContext()->RSSetViewports(1, &viewPort);

	// Sampler State ����.
	//m_pImmediateContext->PSSetSamplers(0, 1, &DXSamplerState::pDefaultSamplerState);
	DEVICE->GetContext()->PSSetSamplers(0, 1, &DXSamplerState::pDefaultMirrorSamplerState);

	// Rasterizer State ����.
	//m_pImmediateContext->RSSetState(DXSamplerState::pDefaultRSWireFrame);
	DEVICE->GetContext()->RSSetState(DXSamplerState::pDefaultRSSolid);

	// Blend State ����.
	DEVICE->GetContext()->OMSetBlendState(DXSamplerState::pBlendSamplerState, 0, -1);

	// Depth Stencil State ����.
	DEVICE->GetContext()->OMSetDepthStencilState(DXSamplerState::pDefaultDepthStencil, 0xff);
	//m_pImmediateContext->OMSetDepthStencilState(DXSamplerState::pGreaterDepthStencil, 0xff); // Depth ū�� ����ϰ� ���� ��.

	return true;
}

bool GameCore::PostRender()
{
	DEVICE->GetSwapChain()->Present(0, 0); // 0�� ���۸� �Ѹ���.
	return true;
}

bool GameCore::CoreRender()
{
	PreRender();
	///////////////////////////////////
	// ���⼭ ������ �ʿ�.

	if (!Render())
	{
		OutputDebugString(L"WanyCore::GameCore::Failed Render.\n");
		return false;
	}

#ifdef DEBUG_DISPLAY
	if (!Input::getInstance()->render())
	{
		OutputDebugString(L"WanyCore::Input::Failed Render.\n");
		return false;
	}

	if (!Timer::getInstance()->render())
	{
		OutputDebugString(L"WanyCore::Timer::Failed Render.\n");
		return false;
	}

	Timer* timer = Timer::getInstance();
	std::wstring info = L"Time: " + std::to_wstring(timer->getPlayTime()) + L", Fps: " + std::to_wstring(timer->getFPS()) + L"\n";
	DXWriter::getInstance()->setString(info);

	if (!DXWriter::getInstance()->render())
	{
		OutputDebugString(L"WanyCore::DXWriter::Failed Render.\n");
		return false;
	}
#endif
	///////////////////////////////////
	PostRender();
	return true;
}

bool GameCore::CoreRelease()
{
	Release();
	// initialize �������� release �� ��!
	FMODSoundManager::getInstance()->Release();
	DXTextureManager::getInstance()->release();
	//DXShaderBorderManager::getInstance()->release();
	//DXShaderManager::getInstance()->release();
	DXSamplerState::release();
	DXWriter::getInstance()->release();
	Input::getInstance()->release();
	Timer::getInstance()->release();
	
	return true;
}

bool GameCore::Run()
{
	if (CoreInitialize())
	{
		bool bGameRun = true;
		while (bGameRun)
		{
			CoreFrame();
			CoreRender();
		}
		CoreRelease();

		return true;
	}
	else
	{
		return false;
	}
}