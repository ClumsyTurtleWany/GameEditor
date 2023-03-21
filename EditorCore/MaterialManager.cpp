#include "MaterialManager.h"
#include <fstream>
#include <sstream>

bool MaterialManager::Initialize()
{
	Material* defaultMaterial = new Material;
	if (defaultMaterial->Load(L"../resource/Material/Default.material"))
	{
		MaterialMap.insert(std::make_pair(L"Default", defaultMaterial));
	}
	else
	{
		defaultMaterial->Name = L"Default";
		defaultMaterial->Type = MaterialType::Default;
		defaultMaterial->DiffuseTextureName = L"../resource/Texture/Default.bmp";
		if (defaultMaterial->Create())
		{
			defaultMaterial->Save(L"../resource/Material/Default.material");
			MaterialMap.insert(std::make_pair(L"Default", defaultMaterial));
		}
		else
		{
			delete defaultMaterial;
			OutputDebugString(L"EditorCore::MaterialManager::Initialize::Failed Create Default Material.");
		}
	}
	
	return true;
}

bool MaterialManager::Release()
{
	for (auto& it : MaterialMap)
	{
		Material* material = it.second;
		if (material != nullptr)
		{
			delete material;
			it.second = nullptr;
		}
	}
	MaterialMap.clear();
	return true;
}

bool MaterialManager::Load(std::wstring filename, std::wstring key)
{
	size_t idx = filename.find_last_of(L".");
	std::wstring extension(filename.begin() + idx, filename.end());
	if (extension != MATERIAL_EXTENSION)
	{
		OutputDebugString(L"EditorCore::MaterialManager::Load::File Extension is Not Material.");
		return false;
	}

	Material* material = new Material;
	if (!material->Load(filename))
	{
		delete material;
		OutputDebugString(L"EditorCore::MaterialManager::Load::Failed Load Material File.");
		return false;
	}

	MaterialMap.insert(std::make_pair(key, material));

	return true;
}

bool MaterialManager::LoadMtlFile(std::wstring filename)
{
	std::fstream file(filename);
	if (!file.is_open())
	{
		OutputDebugString(L"EditorCore::MaterialManager::LoadMtlFile::Failed Open File.");
		return false;
	}
	else
	{
		std::vector<Material*> materialList;
		unsigned int materialCnt = 0;
		while (!file.eof())
		{
			std::string lineData;
			std::getline(file, lineData, '\n');

			std::vector<std::string> wordList;
			if (!SplitString(lineData, ' ', wordList))
			{
				continue;
			}

			if (wordList[0] == "newmtl")
			{
				Material* material = new Material;
				material->Name.assign(wordList[1].begin(), wordList[1].end());
				materialList.push_back(material);
				materialCnt++;
			}
			else if (wordList[0] == "Ns")
			{
				materialList[materialCnt - 1]->Reflection = std::stof(wordList[1]);
			}
			else if (wordList[0] == "Ka")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				materialList[materialCnt - 1]->AmbientColor = vec;
			}
			else if (wordList[0] == "Kd")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				materialList[materialCnt - 1]->DiffuseColor = vec;
			}
			else if (wordList[0] == "Ks")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				materialList[materialCnt - 1]->SpecularColor = vec;
			}
			else if (wordList[0] == "Ke")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				materialList[materialCnt - 1]->EmissiveColor = vec;
			}
			else if (wordList[0] == "Ni")
			{
				materialList[materialCnt - 1]->Refraction = std::stof(wordList[1]);
			}
			else if (wordList[0] == "d")
			{
				materialList[materialCnt - 1]->Alpha = std::stof(wordList[1]);
			}
			else if (wordList[0] == "Tr")
			{
				//_materialList[_materialCnt - 1].Tr = std::stof(wordList[1]);
			}
			else if (wordList[0] == "Tf")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				materialList[materialCnt - 1]->ColorFilter = vec;
			}
			else if (wordList[0] == "illum")
			{
				//_materialList[_materialCnt - 1].illum = std::stoi(wordList[1]);
			}
			else if (wordList[0] == "map_Ka")
			{
				materialList[materialCnt - 1]->AmbientTextureName.assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "map_Kd")
			{
				materialList[materialCnt - 1]->DiffuseTextureName.assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "map_Ks")
			{
				materialList[materialCnt - 1]->SpecularTextureName.assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "map_Ns")
			{
				//materialList[materialCnt - 1]->.assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "map_bump")
			{
				materialList[materialCnt - 1]->NormalTextureName.assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "map_d")
			{
				materialList[materialCnt - 1]->AlphaTextureName.assign(wordList[1].begin(), wordList[1].end());
			}
			else
			{
				continue;
			}

		}
		file.close();

		for (auto& it : materialList)
		{
			MaterialMap.insert(std::make_pair(it->Name, it));
		}
		return true;
	}
}

bool MaterialManager::SplitString(std::string line, char delimiter, std::vector<std::string>& dst)
{
	std::istringstream ss(line);
	std::string word;
	bool isValid = false;
	while (std::getline(ss, word, delimiter))
	{
		isValid = true;
		dst.push_back(word);
	}

	return isValid;
}

Material* MaterialManager::GetMaterial(std::wstring name)
{
	auto it = MaterialMap.find(name);
	if (it != MaterialMap.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

Material* MaterialManager::CreateMaterial(std::wstring name)
{
	Material* material = new Material;
	material->Name = name;
	MaterialMap.insert(std::make_pair(name, material));
	return material;
}