#include "BaseCore.h"

bool BaseCore::CoreInitialize()
{
	if (!Device.Create(_hWnd))
	{
		OutputDebugStringA("WanyCore::DXDevice::Failed Initialize.\n");
		return false;
	}

	DXShaderManager::GetInstance()->SetDevice(Device.m_pd3dDevice, Device.m_pImmediateContext);
	if (!DXShaderManager::GetInstance()->Initialize())
	{
		OutputDebugStringA("WanyCore::DXShaderManager::Failed Initialize.\n");
		return false;
	}

	Input::GetInstance()->setWndHandle(_hWnd);
	if (!Input::GetInstance()->initialize())
	{
		OutputDebugStringA("WanyCore::Input::Failed Initialize.\n");
		return false;
	}

	if (!Timer::GetInstance()->Initialize())
	{
		OutputDebugStringA("WanyCore::Timer::Failed Initialize.\n");
		return false;
	}

	DXTextureManager::GetInstance()->setDevice(Device.m_pd3dDevice, Device.m_pImmediateContext);

	if (!FMODSoundManager::GetInstance()->Initialize())
	{
		OutputDebugStringA("WanyCore::FMODSoundManager::Failed Initialize.\n");
		return false;
	}

	// Sampler State
	if (!DXSamplerState::SetDevice(Device.m_pd3dDevice))
	{
		OutputDebugStringA("WanyCore::DXSamplerState::Failed Set State.\n");
		return false;
	}

	return Initialize();
}

bool BaseCore::CoreFrame()
{
	if (!Input::GetInstance()->frame())
	{
		OutputDebugStringA("WanyCore::Input::Failed Frame.\n");
		return false;
	}

	if (!Timer::GetInstance()->Frame())
	{
		OutputDebugStringA("WanyCore::Timer::Failed Frame.\n");
		return false;
	}

	if (!FMODSoundManager::GetInstance()->Frame())
	{
		OutputDebugStringA("WanyCore::FMODSoundManager::Failed frame.\n");
		return false;
	}

	std::string output = "time: " + std::to_string(Timer::GetInstance()->SecondPerFrame) + ", FPS: " + std::to_string(Timer::GetInstance()->Fps) + "\n";
	OutputDebugStringA(output.c_str());

	return Frame();
}

bool BaseCore::PreRender()
{
	//m_pImmediateContext->OMSetRenderTargets(1, &m_pRTV, NULL); // m_pRTV 에 뿌린다.
	Device.m_pImmediateContext->OMSetRenderTargets(1, &Device.m_pRTV, Device.m_pDepthStencilView); // Depth Stencil View 추가.
	//float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // R, G, B, A 순 0 ~ 1.0사이 값 1.0 == 255
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // R, G, B, A 순 0 ~ 1.0사이 값 1.0 == 255
	Device.m_pImmediateContext->ClearRenderTargetView(Device.m_pRTV, color);
	Device.m_pImmediateContext->ClearDepthStencilView(Device.m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0); // Depth는 1.0f, Stencil은 0으로 클리어.
	//m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//// View Port 설정
	//// 보통은 client 크기로 설정하지만, 미니맵의 경우 뷰포트 설정을 바꿔 출력 가능.
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

	// Sampler State 적용.
	//m_pImmediateContext->PSSetSamplers(0, 1, &DXSamplerState::pDefaultSamplerState);
	Device.m_pImmediateContext->PSSetSamplers(0, 1, &DXSamplerState::pDefaultMirrorSamplerState);

	// Rasterizer State 적용.
	//m_pImmediateContext->RSSetState(DXSamplerState::pDefaultRSWireFrame);
	Device.m_pImmediateContext->RSSetState(DXSamplerState::pDefaultRSSolid);

	// Blend State 적용.
	Device.m_pImmediateContext->OMSetBlendState(DXSamplerState::pBlendSamplerState, 0, -1);

	// Depth Stencil State 적용.
	Device.m_pImmediateContext->OMSetDepthStencilState(DXSamplerState::pDefaultDepthStencil, 0xff);
	//m_pImmediateContext->OMSetDepthStencilState(DXSamplerState::pGreaterDepthStencil, 0xff); // Depth 큰것 출력하고 싶을 때.

	return true;
}

bool BaseCore::PostRender()
{
	//RenderTarget.End();
	//RenderTarget.Render();
	Device.m_pSwapChain->Present(0, 0); // 0번 버퍼를 뿌린다.
	return true;
}

bool BaseCore::ResizeDevice(int x, int y, int width, int height)
{
	if (_hWnd == NULL)
	{
		return false;
	}

	Device.Resize(x, y, width, height);
	return true;
}

bool BaseCore::CoreRender()
{
	PreRender();
	//RenderTarget.Begin();
	///////////////////////////////////
	// 여기서 랜더링 필요.

	if (!Render())
	{
		OutputDebugStringA("WanyCore::GameCore::Failed Render.\n");
		return false;
	}

	//RenderTarget.End();
	//RenderTarget.Render();

#ifdef DEBUG_DISPLAY
	if (!Input::GetInstance()->render())
	{
		OutputDebugStringA("WanyCore::Input::Failed Render.\n");
		return false;
	}

	if (!Timer::GetInstance()->render())
	{
		OutputDebugStringA("WanyCore::Timer::Failed Render.\n");
		return false;
	}

	Timer* timer = Timer::GetInstance();
	std::wstring info = "Time: " + std::to_wstring(timer->getPlayTime()) + ", Fps: " + std::to_wstring(timer->getFPS()) + "\n";
	DXWriter::GetInstance()->setString(info);

	if (!DXWriter::GetInstance()->render())
	{
		OutputDebugStringA("WanyCore::DXWriter::Failed Render.\n");
		return false;
	}
#endif
	///////////////////////////////////
	PostRender();
	return true;
}

bool BaseCore::CoreRelease()
{
	Release();
	// initialize 역순으로 release 할 것!
	FMODSoundManager::GetInstance()->Release();
	DXTextureManager::GetInstance()->release();
	DXShaderManager::GetInstance()->Release();
	DXSamplerState::Release();
	Input::GetInstance()->release();
	Timer::GetInstance()->Release();
	Device.Release();

	return true;
}

bool BaseCore::Run()
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

void BaseCore::SetWindowHandle(HWND hWnd)
{
	_hWnd = hWnd;
}
