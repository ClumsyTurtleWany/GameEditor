#pragma once
#include "Define.h"

#define MATERIAL_EXTENSION L".material"

class Material
{
public:
	std::wstring Name;

	Vector3 AmbientColor;
	Vector3 DiffuseColor;
	Vector3 SpecularColor;
	Vector3 EmissiveColor;
	Vector3 ColorFilter;

	float Reflection;
	float Refraction;
	float Alpha;

	std::wstring PixelShaderCodeName;
	std::wstring DiffuseTextureName;
	std::wstring AmbientTextureName;
	std::wstring NormalTextureName;
	std::wstring SpecularTextureName;
	std::wstring EmissiveTextureName;
	std::wstring AlphaTextureName;

public:
	ID3D11PixelShader* PixelShader;

	DXTexture* DiffuseTexture = nullptr;
	DXTexture* AmbientTexture = nullptr;
	DXTexture* NormalTexture = nullptr;
	DXTexture* SpecularTexture = nullptr;
	DXTexture* EmissiveTexture = nullptr;
	DXTexture* AlphaTexture = nullptr;

public:
	Material();
	~Material();

public:
	bool Initialize();
	bool Apply();
	bool Save(std::wstring path);
	bool Load(std::wstring filename);

	bool SplitString(std::string line, char delimiter, std::vector<std::string>& dst);
	
public:
	bool SetPixelShader(ID3D11PixelShader* pixelShader);

	void SetDiffuseTexture(DXTexture* texture);
	void SetAmbientTexture(DXTexture* texture);
	void SetNormalTexture(DXTexture* texture);
	void SetSpecularTexture(DXTexture* texture);
	void SetEmissiveTexture(DXTexture* texture);
};