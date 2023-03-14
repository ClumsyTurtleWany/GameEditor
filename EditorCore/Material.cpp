#include "Material.h"

Material::Material()
{
}

Material::~Material()
{
}

bool Material::Initialize()
{
	PixelShader = DXShaderManager::GetInstance()->GetPixelShader(PixelShaderCodeName);
	if (PixelShader == nullptr)
	{
		PixelShader = DXShaderManager::GetInstance()->GetPixelShader(L"Light");
	}

	if (DXTextureManager::GetInstance()->Load(DiffuseTextureName))
	{
		DiffuseTexture = DXTextureManager::GetInstance()->GetTexture(DiffuseTextureName);
	}
	return true;
}

bool Material::Apply()
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

bool Material::Save(std::wstring filename)
{
	return false;
}

bool Material::Load(std::wstring filename)
{
	return false;
}

bool Material::SetPixelShader(ID3D11PixelShader* pixelShader)
{
	PixelShader = pixelShader;
	return true;
}

void Material::SetDiffuseTexture(DXTexture* texture)
{
	DiffuseTexture = texture;
}

void Material::SetAmbientTexture(DXTexture* texture)
{
	AmbientTexture = texture;
}

void Material::SetNormalTexture(DXTexture* texture)
{
	NormalTexture = texture;
}

void Material::SetSpecularTexture(DXTexture* texture)
{
	SpecularTexture = texture;
}

void Material::SetEmissiveTexture(DXTexture* texture)
{
	EmissiveTexture = texture;
}