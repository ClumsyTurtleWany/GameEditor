#include "DXUserInterfaceManager.h"

bool DXUserInterfaceManager::Initialize()
{
	//VertexShader = DXShaderManager::GetInstance()->GetVertexShader(L"../include/HLSL/VS_UserInterface.hlsl");

	return true;
}

bool DXUserInterfaceManager::Frame()
{
	for (auto& UI : UIList)
	{
		UI->Frame();
	}

	return true;
}

bool DXUserInterfaceManager::Render()
{
	DXDevice::g_pImmediateContext->IASetInputLayout(InputLayout);
	DXDevice::g_pImmediateContext->VSSetShader(VertexShader, NULL, 0);
	DXDevice::g_pImmediateContext->HSSetShader(NULL, NULL, 0);
	DXDevice::g_pImmediateContext->DSSetShader(NULL, NULL, 0);
	DXDevice::g_pImmediateContext->GSSetShader(NULL, NULL, 0);

	for (auto& UI : UIList)
	{
		UI->Render();
	}

	return true;
}

bool DXUserInterfaceManager::Release()
{
	for (auto& UI : UIList)
	{
		UI->Release();
	}

	return true;
}

void DXUserInterfaceManager::AddUserInterface(DXUserInterface* ui)
{
	UIList.push_back(ui);
}
