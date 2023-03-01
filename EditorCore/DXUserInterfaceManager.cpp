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
	Context->IASetInputLayout(InputLayout);
	Context->VSSetShader(VertexShader, NULL, 0);
	Context->HSSetShader(NULL, NULL, 0);
	Context->DSSetShader(NULL, NULL, 0);
	Context->GSSetShader(NULL, NULL, 0);

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

void DXUserInterfaceManager::SetContext(ID3D11DeviceContext* context)
{
	Context = context;
}

ID3D11DeviceContext* DXUserInterfaceManager::GetContext()
{
	return Context;
}

void DXUserInterfaceManager::AddUserInterface(DXUserInterface* ui)
{
	UIList.push_back(ui);
}
