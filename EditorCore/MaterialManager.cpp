#include "MaterialManager.h"

bool MaterialManager::Initialize()
{
	Material* defaultMaterial = new Material;
	defaultMaterial->DiffuseTextureName = L"../resource/Default.bmp";
	defaultMaterial->Initialize();
	MaterialList.insert(std::make_pair(L"Default", defaultMaterial));
	return true;
}

bool MaterialManager::Release()
{
	for (auto& it : MaterialList)
	{
		Material* material = it.second;
		if (material != nullptr)
		{
			delete material;
			it.second = nullptr;
		}
	}
	MaterialList.clear();
	return true;
}

bool MaterialManager::Load(std::wstring filename)
{
	return true;
}

Material* MaterialManager::GetMaterial(std::wstring name)
{
	auto it = MaterialList.find(name);
	if (it != MaterialList.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

bool MaterialManager::AddMaterial(std::wstring name, Material* material)
{
	if (GetMaterial(name))
	{
		return true;
	}
	material->Initialize();
	MaterialList.insert(std::make_pair(name, material));

	return true;
}
