#include "Material.h"
#include <fstream>
#include <sstream>

Material::Material()
{
}

Material::~Material()
{
}

bool Material::Initialize()
{
	PixelShader = DXShaderManager::GetInstance()->GetPixelShader(PixelShaderCodeName);
	if (PixelShader == nullptr)
	{
		PixelShader = DXShaderManager::GetInstance()->GetPixelShader(L"Light");
	}

	if (DXTextureManager::GetInstance()->Load(DiffuseTextureName))
	{
		DiffuseTexture = DXTextureManager::GetInstance()->GetTexture(DiffuseTextureName);
	}
	return true;
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
	std::wstring filename = path + Name + MATERIAL_EXTENSION;
	std::fstream file(filename, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
	if (!file.is_open())
	{
		OutputDebugString(L"EditorCore::Material::Save::Failed Open File.");
		return false;
	}
	else
	{
		file << std::endl;
		file << "newmtl " << Name.c_str() << std::endl;
		file << "Ns " << Reflection << std::endl;
		file << "Ka " << AmbientColor.x << " " << AmbientColor.y << " " << AmbientColor.z << std::endl;
		file << "Kd " << DiffuseColor.x << " " << DiffuseColor.y << " " << DiffuseColor.z << std::endl;
		file << "Ks " << SpecularColor.x << " " << SpecularColor.y << " " << SpecularColor.z << std::endl;
		file << "Ke " << EmissiveColor.x << " " << EmissiveColor.y << " " << EmissiveColor.z << std::endl;
		file << "Ni " << Refraction << std::endl;
		file << "d " << Alpha << std::endl;
		file << "illum " << "2" << std::endl;

		if (!AmbientTextureName.empty())
		{
			file << "map_Ka " << AmbientTextureName.c_str() << std::endl;
		}

		if (!DiffuseTextureName.empty())
		{
			file << "map_Kd " << DiffuseTextureName.c_str() << std::endl;
		}

		if (!SpecularTextureName.empty())
		{
			file << "map_Ks " << SpecularTextureName.c_str() << std::endl;
		}

		if (!NormalTextureName.empty())
		{
			file << "map_bump " << NormalTextureName.c_str() << std::endl;
		}

		if (!AlphaTextureName.empty())
		{
			file << "map_d " << AlphaTextureName.c_str() << std::endl;
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

			if (wordList[0] == "newmtl")
			{
				Name.assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "Ns")
			{
				Reflection = std::stof(wordList[1]);
			}
			else if (wordList[0] == "Ka")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				AmbientColor = vec;
			}
			else if (wordList[0] == "Kd")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				DiffuseColor = vec;
			}
			else if (wordList[0] == "Ks")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				SpecularColor = vec;
			}
			else if (wordList[0] == "Ke")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				EmissiveColor = vec;
			}
			else if (wordList[0] == "Ni")
			{
				Refraction = std::stof(wordList[1]);
			}
			else if (wordList[0] == "d")
			{
				Alpha = std::stof(wordList[1]);
			}
			else if (wordList[0] == "Tr")
			{
				//material. = std::stoi(wordList[1]);
			}
			else if (wordList[0] == "Tf")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				ColorFilter = vec;
			}
			else if (wordList[0] == "illum")
			{
				//material. = std::stoi(wordList[1]);
			}
			else if (wordList[0] == "map_Ka")
			{
				AmbientTextureName.assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "map_Kd")
			{
				DiffuseTextureName.assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "map_Ks")
			{
				SpecularTextureName.assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "map_Ns")
			{
				//material->SpecularHighlightTextureName.assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "map_bump")
			{
				NormalTextureName.assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "map_d")
			{
				AlphaTextureName.assign(wordList[1].begin(), wordList[1].end());
			}
			else
			{
				continue;
			}

		}
		file.close();
		return true;
	}
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