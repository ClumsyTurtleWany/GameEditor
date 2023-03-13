#pragma once
#include "Define.h"

class Material
{
private:
	ID3D11PixelShader* PixelShader;

	DXTexture* DiffuseTexture = nullptr;
	DXTexture* AmbientTexture = nullptr;
	DXTexture* NormalTexture = nullptr;
	DXTexture* SpecularTexture = nullptr;
	DXTexture* EmissiveTexture = nullptr;

public:
	std::wstring PixelShaderCodeName;

	std::wstring DiffuseTextureName;
	std::wstring AmbientTextureName;
	std::wstring NormalTextureName;
	std::wstring SpecularTextureName;
	std::wstring EmissiveTextureName;

public:
	Material();
	~Material();

public:
	bool Initialize();
	bool Apply();
	bool Save(std::wstring filename);
	bool Load(std::wstring filename);

public:
	bool SetPixelShader(ID3D11PixelShader* pixelShader);

	void SetDiffuseTexture(DXTexture* texture);
	void SetAmbientTexture(DXTexture* texture);
	void SetNormalTexture(DXTexture* texture);
	void SetSpecularTexture(DXTexture* texture);
	void SetEmissiveTexture(DXTexture* texture);
};