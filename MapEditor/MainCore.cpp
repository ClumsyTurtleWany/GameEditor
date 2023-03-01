#include "pch.h"
#include "MainCore.h"

MainCore::MainCore()
{
	DebugCamera = new Camera;
	DebugCamera->SetContext(Device.m_pImmediateContext);
	DebugCamera->CreateViewMatrix(Vector3(0.0f, 0.0f, -10.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0, 0.0f));
	DebugCamera->CreateProjectionMatrix(1.0f, 500.0f, PI * 0.25, (Device.m_ViewPort.Width) / (Device.m_ViewPort.Height));
	
}

MainCore::~MainCore()
{
}

bool MainCore::Resize(int x, int y, int width, int height)
{
	return false;
}

ID3D11Device* MainCore::GetDevice()
{
	return nullptr;
}

ID3D11DeviceContext* MainCore::GetContext()
{
	return nullptr;
}

Camera* MainCore::GetCamera()
{
	return nullptr;
}

bool MainCore::CreateLandscape(int width, int height, int sectionSize)
{
	return false;
}

bool MainCore::AddSelectedEntity()
{
	return false;
}

bool MainCore::Sculpting()
{
	return false;
}
