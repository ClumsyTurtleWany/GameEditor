#pragma once
#include "Define.h"
#include "Material.h"

class MaterialManager : public Singleton<MaterialManager>
{
private:
	friend class Singleton<MaterialManager>;

	std::map<std::wstring, Material*> MaterialList;

public:
	virtual bool Initialize();
	virtual bool Release();

public:
	bool Load(std::wstring filename);

	Material* GetMaterial(std::wstring name);
	bool AddMaterial(std::wstring name, Material* material);
};