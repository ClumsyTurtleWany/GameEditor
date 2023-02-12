#pragma once
#include "Define.h"
#include "DXTexture.hpp"

class Material
{
private:
	std::shared_ptr<ID3D11VertexShader>		VertexShader;
	std::shared_ptr<ID3D11HullShader>		HullShader;
	std::shared_ptr<ID3D11DomainShader>		DomainShader;
	std::shared_ptr<ID3D11GeometryShader>	GeometryShader;
	std::shared_ptr<ID3D11PixelShader>		PixelShader;
	std::shared_ptr<ID3D11Buffer>			ConstantBuffer;
	std::vector<DXTexture*>	Textures;

public:
	bool CreateVertexShader(const std::vector<Vertex>& verticies, )
};