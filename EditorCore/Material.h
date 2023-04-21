#pragma once
#include "Define.h"

#define MATERIAL_EXTENSION L".material"
#define MATERIAL_MAX_LAYER_CNT (int)4

enum MaterialType
{
	Default = (int)0,
	Light = 1,
	Layer = 2,
	Normal = 3,
	Sky = 4,
};

class Material
{
public:
	std::wstring Name;
	MaterialType Type = MaterialType::Default;

	Vector3 AmbientColor = Vector3(1.0f, 1.0f, 1.0f);
	Vector3 DiffuseColor = Vector3(1.0f, 1.0f, 1.0f);
	Vector3 SpecularColor = Vector3(1.0f, 1.0f, 1.0f);
	Vector3 EmissiveColor = Vector3(1.0f, 1.0f, 1.0f);
	Vector3 ColorFilter = Vector3(1.0f, 1.0f, 1.0f);

	float Reflection = 1.0f;
	float Refraction = 1.0f;
	float Alpha = 1.0f;

	std::wstring PixelShaderName;
	std::wstring DiffuseTextureName;
	std::wstring AmbientTextureName;
	std::wstring NormalTextureName;
	std::wstring SpecularTextureName;
	std::wstring EmissiveTextureName;
	std::wstring AlphaTextureName;
	std::wstring LayerTextureName[MATERIAL_MAX_LAYER_CNT];

public:
	ID3D11PixelShader* PixelShader;

	DXTexture* DiffuseTexture = nullptr;
	DXTexture* AmbientTexture = nullptr;
	DXTexture* NormalTexture = nullptr;
	DXTexture* SpecularTexture = nullptr;
	DXTexture* EmissiveTexture = nullptr;
	DXTexture* AlphaTexture = nullptr;
	DXTexture* LayerTexture[MATERIAL_MAX_LAYER_CNT] = { nullptr, };

public:
	Material();
	~Material();

public:
	bool Apply();
	bool Save(std::wstring path);
	bool Load(std::wstring filename);
	bool Create();

	bool SplitString(std::string line, char delimiter, std::vector<std::string>& dst);
	
public:
	bool SetPixelShader(ID3D11PixelShader* pixelShader);

	void SetDiffuseTexture(DXTexture* texture);
	void SetAmbientTexture(DXTexture* texture);
	void SetNormalTexture(DXTexture* texture);
	void SetSpecularTexture(DXTexture* texture);
	void SetEmissiveTexture(DXTexture* texture);
};