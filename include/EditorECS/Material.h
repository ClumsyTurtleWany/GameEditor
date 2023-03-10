#pragma once
#include "Define.h"
#include "DXTexture.hpp"
#include "DXDevice.hpp"
#include "DXShaderManager.h"

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
	std::wstring DiffuseTextureName;
	std::wstring AmbientTextureName;
	std::wstring NormalTextureName;
	std::wstring SpecularTextureName;
	std::wstring EmissiveTextureName;

public:
	Material();
	~Material();

public:
	bool Apply();

public:
	bool SetPixelShader(ID3D11PixelShader* pixelShader);

	void SetDiffuseTexture(DXTexture* texture);
	void SetAmbientTexture(DXTexture* texture);
	void SetNormalTexture(DXTexture* texture);
	void SetSpecularTexture(DXTexture* texture);
	void SetEmissiveTexture(DXTexture* texture);
};

inline Material::Material()
{
}

inline Material::~Material()
{
}

inline bool Material::Apply()
{
	DXDevice::g_pImmediateContext->PSSetShader(PixelShader, NULL, 0);

	if (DiffuseTexture != nullptr)
	{
		ID3D11ShaderResourceView* resourceView = DiffuseTexture->GetResourceView();
		DXDevice::g_pImmediateContext->PSSetShaderResources(0, 1, &resourceView);
	}

	if (NormalTexture != nullptr)
	{
		ID3D11ShaderResourceView* resourceView = NormalTexture->GetResourceView();
		DXDevice::g_pImmediateContext->PSSetShaderResources(1, 1, &resourceView);
	}

	return true;
}

inline bool Material::SetPixelShader(ID3D11PixelShader* pixelShader)
{
	PixelShader = pixelShader;
	return true;
}

inline void Material::SetDiffuseTexture(DXTexture* texture)
{
	DiffuseTexture = texture;
}

inline void Material::SetAmbientTexture(DXTexture* texture)
{
	AmbientTexture = texture;
}

inline void Material::SetNormalTexture(DXTexture* texture)
{
	NormalTexture = texture;
}

inline void Material::SetSpecularTexture(DXTexture* texture)
{
	SpecularTexture = texture;
}

inline void Material::SetEmissiveTexture(DXTexture* texture)
{
	EmissiveTexture = texture;
}