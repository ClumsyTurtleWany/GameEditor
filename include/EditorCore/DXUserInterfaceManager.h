#pragma once
#include "Define.h"
#include "DXUserInterface.h"

class DXUserInterfaceManager : public Singleton<DXUserInterfaceManager>
{
private:
	friend class Singleton<DXUserInterfaceManager>;

private:
	ID3D11DeviceContext* Context = nullptr;
	ID3D11InputLayout* InputLayout = nullptr;
	ID3D11VertexShader* VertexShader = nullptr;


private:
	std::vector<DXUserInterface*> UIList;

public:
	bool Initialize();
	bool Frame();
	bool Render();
	bool Release();

public:
	void AddUserInterface(DXUserInterface* ui);
};