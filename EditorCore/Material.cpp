#include "Material.h"
#include <fstream>
#include <sstream>

Material::Material()
{
}

Material::~Material()
{
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

bool Material::Save(std::wstring path)
{
	//std::wstring filename = path + Name + MATERIAL_EXTENSION;
	std::fstream file(path, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
	if (!file.is_open())
	{
		OutputDebugString(L"EditorCore::Material::Save::Failed Open File.");
		return false;
	}
	else
	{
		std::string name;
		name.assign(Name.begin(), Name.end());
		file << std::endl;
		file << "Name " << name << std::endl;
		file << "Type " << Type << std::endl;
		file << "Reflection " << std::to_string(Reflection) << std::endl;
		file << "ReFraction " << std::to_string(Refraction) << std::endl;
		file << "DiffuseColor " << std::to_string(DiffuseColor.x) << " " << std::to_string(DiffuseColor.y) << " " << std::to_string(DiffuseColor.z) << std::endl;
		file << "AmbientColor "  << std::to_string(AmbientColor.x) << " " << std::to_string(AmbientColor.y) << " " << std::to_string(AmbientColor.z) << std::endl;
		file << "SpecularColor " << std::to_string(SpecularColor.x) << " " << std::to_string(SpecularColor.y) << " " << std::to_string(SpecularColor.z) << std::endl;
		file << "EmissiveColor"  << std::to_string(EmissiveColor.x) << " " << std::to_string(EmissiveColor.y) << " " << std::to_string(EmissiveColor.z) << std::endl;
		file << "ColorFilter"    << std::to_string(ColorFilter.x) << " " << std::to_string(ColorFilter.y) << " " << std::to_string(ColorFilter.z) << std::endl;
		file << "Alpha " << std::to_string(Alpha) << std::endl;
		
		if (!DiffuseTextureName.empty())
		{
			name.assign(DiffuseTextureName.begin(), DiffuseTextureName.end());
			file << "DiffuseTextureName " << name << std::endl;
		}

		if (!AmbientTextureName.empty())
		{
			name.assign(AmbientTextureName.begin(), AmbientTextureName.end());
			file << "AmbientTextureName " << name << std::endl;
		}

		if (!SpecularTextureName.empty())
		{
			name.assign(SpecularTextureName.begin(), SpecularTextureName.end());
			file << "SpecularTextureName " << name << std::endl;
		}

		if (!NormalTextureName.empty())
		{
			name.assign(NormalTextureName.begin(), NormalTextureName.end());
			file << "NormalTextureName " << name << std::endl;
		}

		if (!AlphaTextureName.empty())
		{
			name.assign(AlphaTextureName.begin(), AlphaTextureName.end());
			file << "AlphaTextureName " << name << std::endl;
		}

		for (int idx = 0; idx < MATERIAL_MAX_LAYER_CNT; idx++)
		{
			if (!LayerTextureName[idx].empty())
			{
				name.assign(LayerTextureName[idx].begin(), LayerTextureName[idx].end());
				file << "LayerTexture" << idx << " " << name << std::endl;
			}
		}

		file.close();
		return true;
	}
}

bool Material::Load(std::wstring filename)
{
	std::fstream file(filename);
	if (!file.is_open())
	{
		return false;
	}
	else
	{
		while (!file.eof())
		{
			std::string lineData;
			std::getline(file, lineData, '\n');

			std::vector<std::string> wordList;
			if (!SplitString(lineData, ' ', wordList))
			{
				continue;
			}

			if (wordList[0] == "Name")
			{
				Name.assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "Type")
			{
				Type = (MaterialType)std::stoi(wordList[1]);
			}
			else if (wordList[0] == "Reflection")
			{
				Reflection = std::stof(wordList[1]);
			}
			else if (wordList[1] == "Refraction")
			{
				Refraction = std::stof(wordList[1]);
			}
			else if (wordList[0] == "DiffuseColor")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				DiffuseColor = vec;
			}
			else if (wordList[0] == "AmbientColor")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				AmbientColor = vec;
			}
			else if (wordList[0] == "SpecularColor")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				SpecularColor = vec;
			}
			else if (wordList[0] == "EmissiveColor")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				EmissiveColor = vec;
			}
			else if (wordList[0] == "ColorFilter")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				ColorFilter = vec;
			}
			else if (wordList[0] == "Alpha")
			{
				Alpha = std::stof(wordList[1]);
			}
			else if (wordList[0] == "DiffuseTextureName")
			{
				DiffuseTextureName.assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "AmbientTextureName")
			{
				AmbientTextureName.assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "SpecularTextureName")
			{
				SpecularTextureName.assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "NormalTextureName")
			{
				NormalTextureName.assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "AlphaTextureName")
			{
				AlphaTextureName.assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "LayerTexture0")
			{
				LayerTextureName[0].assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "LayerTexture1")
			{
				LayerTextureName[1].assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "LayerTexture2")
			{
				LayerTextureName[2].assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "LayerTexture3")
			{
				LayerTextureName[3].assign(wordList[1].begin(), wordList[1].end());
			}
			else
			{
				continue;
			}

		}
		file.close();

		if (!Create())
		{
			OutputDebugString(L"EditorCore::Material::Load::Failed Create Material.");
			return false;
		}

		return true;
	}
}

bool Material::Create()
{
	switch (Type)
	{
	case MaterialType::Default:
		PixelShaderName = L"Default";
		break;
	case MaterialType::Light:
		PixelShaderName = L"Light";
		break;
	case MaterialType::Layer:
		PixelShaderName = L"Landscape";
		break;
	case MaterialType::Normal:
		PixelShaderName = L"NormalMap";
		break;
	}

	if (PixelShaderName.empty())
	{
		OutputDebugString(L"EditorCore::Material::Create::Pixel Shader Code Name is NULL.");
		return false;
	}

	PixelShader = DXShaderManager::GetInstance()->GetPixelShader(PixelShaderName);
	
	if (!DiffuseTextureName.empty())
	{
		if (DXTextureManager::GetInstance()->Load(DiffuseTextureName))
		{
			DiffuseTexture = DXTextureManager::GetInstance()->GetTexture(DiffuseTextureName);
		}
	}

	if (!AmbientTextureName.empty())
	{
		if (DXTextureManager::GetInstance()->Load(AmbientTextureName))
		{
			AmbientTexture = DXTextureManager::GetInstance()->GetTexture(AmbientTextureName);
		}
	}

	if (!NormalTextureName.empty())
	{
		if (DXTextureManager::GetInstance()->Load(NormalTextureName))
		{
			NormalTexture = DXTextureManager::GetInstance()->GetTexture(NormalTextureName);
		}
	}
	;
	if (!SpecularTextureName.empty())
	{
		if (DXTextureManager::GetInstance()->Load(SpecularTextureName))
		{
			SpecularTexture = DXTextureManager::GetInstance()->GetTexture(SpecularTextureName);
		}
	}

	if (!EmissiveTextureName.empty())
	{
		if (DXTextureManager::GetInstance()->Load(EmissiveTextureName))
		{
			EmissiveTexture = DXTextureManager::GetInstance()->GetTexture(EmissiveTextureName);
		}
	}

	if (!AlphaTextureName.empty())
	{
		if (DXTextureManager::GetInstance()->Load(AlphaTextureName))
		{
			AlphaTexture = DXTextureManager::GetInstance()->GetTexture(AlphaTextureName);
		}
	}
	
	for (int idx = 0; idx < MATERIAL_MAX_LAYER_CNT; idx++)
	{
		if (!LayerTextureName[idx].empty())
		{
			if (DXTextureManager::GetInstance()->Load(LayerTextureName[idx]))
			{
				LayerTexture[idx] = DXTextureManager::GetInstance()->GetTexture(LayerTextureName[idx]);
			}
		}
	}

	bool isValid = false;
	if (DiffuseTexture != nullptr)
	{
		isValid = true;
	}

	for (int idx = 0; idx < MATERIAL_MAX_LAYER_CNT; idx++)
	{
		if (LayerTexture[idx] != nullptr)
		{
			isValid = true;
		}
	}

	if (!isValid)
	{
		return false;
	}

	return true;
}

bool Material::SplitString(std::string line, char delimiter, std::vector<std::string>& dst)
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