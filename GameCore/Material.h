#pragma once
#include "Define.h"
#include "DXTexture.hpp"
#include "DXDevice.hpp"
#include "DXShaderManager.h"

class Material
{
private:
	ID3D11DeviceContext*			Context;
	ID3D11PixelShader*				PixelShader;
	std::vector<DXTexture*>			Textures;

	DXTexture* DiffuseTexture = nullptr;
	DXTexture* AmbientTexture = nullptr;
	DXTexture* NormalTexture = nullptr;
	DXTexture* SpecularTexture = nullptr;
	DXTexture* TangentTexture = nullptr;

public:
	Material();
	~Material();

public:
	bool Apply();

public:
	bool AddTexture(DXTexture* texture);
	bool SetPixelShader(ID3D11PixelShader* pixelShader);

	void SetContext(ID3D11DeviceContext* context);

	void SetDiffuseTexture(DXTexture* texture);
	void SetAmbientTexture(DXTexture* texture);
	void SetNormalTexture(DXTexture* texture);
	void SetSpecularTexture(DXTexture* texture);
	void SetTangentTexture(DXTexture* texture);
};

inline Material::Material()
{
	Context = DXShaderManager::getInstance()->GetContext();
}

inline Material::~Material()
{
	Textures.clear();
}

inline bool Material::Apply()
{
	Context->PSSetShader(PixelShader, NULL, 0);
	/*for (size_t idx = 0; idx < Textures.size(); idx++)
	{
		ID3D11ShaderResourceView* resourceView = Textures[idx]->getResourceView();
		Context->PSSetShaderResources(idx, 1, &resourceView);
	}*/

	if (DiffuseTexture != nullptr)
	{
		ID3D11ShaderResourceView* resourceView = DiffuseTexture->getResourceView();
		Context->PSSetShaderResources(0, 1, &resourceView);
	}

	return true;
}

inline bool Material::AddTexture(DXTexture* texture)
{
	Textures.push_back(texture);
	return true;
}

inline bool Material::SetPixelShader(ID3D11PixelShader* pixelShader)
{
	PixelShader = pixelShader;
	return true;
}

inline void Material::SetContext(ID3D11DeviceContext* context)
{
	Context = context;
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

inline void Material::SetTangentTexture(DXTexture* texture)
{
	TangentTexture = texture;
}