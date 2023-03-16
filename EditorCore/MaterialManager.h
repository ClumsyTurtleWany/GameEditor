#pragma once
#include "Define.h"
#include "Material.h"

class MaterialManager : public Singleton<MaterialManager>
{
private:
	friend class Singleton<MaterialManager>;

	std::map<std::wstring, Material*> MaterialMap;

public:
	virtual bool Initialize();
	virtual bool Release();

public:
	bool Load(std::wstring filename, std::wstring key);
	bool LoadMtlFile(std::wstring filename);

	bool SplitString(std::string line, char delimiter, std::vector<std::string>& dst);

	Material* GetMaterial(std::wstring name);
	Material* CreateMaterial(std::wstring name);
};