#include "GameCore.h"

bool GameCore::CoreInitialize()
{
	if (!DXDevice::getInstance()->Create(_hWnd))
	{
		OutputDebugStringA("WanyCore::DXDevice::Failed Initialize.\n");
		return false;
	}

	DXShaderManager::getInstance()->SetDevice(DEVICE->GetDevice(), DEVICE->GetContext());
	if (!DXShaderManager::getInstance()->Initialize())
	{
		OutputDebugStringA("WanyCore::DXShaderManager::Failed Initialize.\n");
		return false;
	}

	/*RECT rc;
	GetClientRect(_hWnd, &rc);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;
	RenderTarget.SetDevice(DEVICE->GetDevice(), DEVICE->GetContext());
	if (!RenderTarget.Create(0, 0, width, height))
	{
		OutputDebugStringA("WanyCore::DXRenderTarget::Failed Create Render Target.\n");
		return false;
	}*/

	Input::getInstance()->setWndHandle(_hWnd);
	if (!Input::getInstance()->initialize())
	{
		OutputDebugStringA("WanyCore::Input::Failed Initialize.\n");
		return false;
	}

	if (!Timer::getInstance()->initialize())
	{
		OutputDebugStringA("WanyCore::Timer::Failed Initialize.\n");
		return false;
	}

	DXTextureManager::getInstance()->setDevice(DEVICE->GetDevice(), DEVICE->GetContext());

	if (!FMODSoundManager::getInstance()->Initialize())
	{
		OutputDebugStringA("WanyCore::FMODSoundManager::Failed Initialize.\n");
		return false;
	}

	// Sampler State
	if (!DXSamplerState::setState(DEVICE->GetDevice()))
	{
		OutputDebugStringA("WanyCore::DXSamplerState::Failed Set State.\n");
		return false;
	}

	return Initialize();
}

bool GameCore::CoreFrame()
{
	if (!Input::getInstance()->frame())
	{
		OutputDebugStringA("WanyCore::Input::Failed Frame.\n");
		return false;
	}

	if (!Timer::getInstance()->frame())
	{
		OutputDebugStringA("WanyCore::Timer::Failed Frame.\n");
		return false;
	}

	if (!FMODSoundManager::getInstance()->Frame())
	{
		OutputDebugStringA("WanyCore::FMODSoundManager::Failed frame.\n");
		return false;
	}

	return Frame();
}

bool GameCore::PreRender()
{
	//m_pImmediateContext->OMSetRenderTargets(1, &m_pRTV, NULL); // m_pRTV �� �Ѹ���.
	DEVICE->GetContext()->OMSetRenderTargets(1, &DEVICE->m_pRTV, DEVICE->m_pDepthStencilView); // Depth Stencil View �߰�.
	//float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // R, G, B, A �� 0 ~ 1.0���� �� 1.0 == 255
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // R, G, B, A �� 0 ~ 1.0���� �� 1.0 == 255
	DEVICE->GetContext()->ClearRenderTargetView(DEVICE->m_pRTV, color);
	DEVICE->GetContext()->ClearDepthStencilView(DEVICE->m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0); // Depth�� 1.0f, Stencil�� 0���� Ŭ����.
	//m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//// View Port ����
	//// ������ client ũ��� ����������, �̴ϸ��� ��� ����Ʈ ������ �ٲ� ��� ����.
	//RECT rcClient = g_pWindow->getClientRect();
	//float width = rcClient.right - rcClient.left;
	//float height = rcClient.bottom - rcClient.top;
	//D3D11_VIEWPORT viewPort;
	//viewPort.TopLeftX = 0;
	//viewPort.TopLeftY = 0;
	//viewPort.Width = width; //1366; 
	//viewPort.Height = height; //786;
	//viewPort.MinDepth = 0;
	//viewPort.MaxDepth = 1;
	//DEVICE->GetContext()->RSSetViewports(1, &viewPort);

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
	//RenderTarget.End();
	//RenderTarget.Render();
	DEVICE->GetSwapChain()->Present(0, 0); // 0�� ���۸� �Ѹ���.
	return true;
}

bool GameCore::CoreRender()
{
	PreRender();
	//RenderTarget.Begin();
	///////////////////////////////////
	// ���⼭ ������ �ʿ�.

	if (!Render())
	{
		OutputDebugStringA("WanyCore::GameCore::Failed Render.\n");
		return false;
	}

	//RenderTarget.End();
	//RenderTarget.Render();

#ifdef DEBUG_DISPLAY
	if (!Input::getInstance()->render())
	{
		OutputDebugStringA("WanyCore::Input::Failed Render.\n");
		return false;
	}

	if (!Timer::getInstance()->render())
	{
		OutputDebugStringA("WanyCore::Timer::Failed Render.\n");
		return false;
	}

	Timer* timer = Timer::getInstance();
	std::wstring info = "Time: " + std::to_wstring(timer->getPlayTime()) + ", Fps: " + std::to_wstring(timer->getFPS()) + "\n";
	DXWriter::getInstance()->setString(info);

	if (!DXWriter::getInstance()->render())
	{
		OutputDebugStringA("WanyCore::DXWriter::Failed Render.\n");
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
	DXShaderManager::getInstance()->Release();
	DXSamplerState::release();
	Input::getInstance()->release();
	Timer::getInstance()->release();
	DXDevice::getInstance()->Release();
	
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

void GameCore::SetWindowHandle(HWND hWnd)
{
	_hWnd = hWnd;
}
