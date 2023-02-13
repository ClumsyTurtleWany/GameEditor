#pragma once
#include "Define.h"
#include "DXTexture.hpp"
#include "DXDevice.hpp"

class Material
{
private:
	ID3D11PixelShader*				PixelShader;
	std::vector<DXTexture*>			Textures;

public:
	bool Apply();
};

bool Material::Apply()
{
	CONTEXT->PSSetShader(PixelShader, NULL, 0);
	for (size_t idx = 0; idx < Textures.size(); idx++)
	{
		ID3D11ShaderResourceView* resourceView = Textures[idx]->getResourceView();
		CONTEXT->PSSetShaderResources(idx, 1, &resourceView);
	}

}