#include "MtlFile.h"

bool MtlFile::Load(std::wstring filename)
{
	std::fstream file(filename);
	if (!file.is_open())
	{
		return false;
	}
	else
	{
		_materialCnt = 0;
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
				MATERIAL material;
				material.name.assign(wordList[1].begin(), wordList[1].end());
				_materialList.push_back(material);
				_materialCnt++;
			}
			else if (wordList[0] == "Ns")
			{
				_materialList[_materialCnt - 1].Ns = std::stof(wordList[1]);
			}
			else if (wordList[0] == "Ka")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				_materialList[_materialCnt - 1].Ka = vec;
			}
			else if (wordList[0] == "Kd")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				_materialList[_materialCnt - 1].Kd = vec;
			}
			else if (wordList[0] == "Ks")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				_materialList[_materialCnt - 1].Ks = vec;
			}
			else if (wordList[0] == "Ke")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				_materialList[_materialCnt - 1].Ke = vec;
			}
			else if (wordList[0] == "Ni")
			{
				_materialList[_materialCnt - 1].Ni = std::stof(wordList[1]);
			}
			else if (wordList[0] == "d")
			{
				_materialList[_materialCnt - 1].d = std::stof(wordList[1]);
			}
			else if (wordList[0] == "Tr")
			{
				_materialList[_materialCnt - 1].Tr = std::stof(wordList[1]);
			}
			else if (wordList[0] == "Tf")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				_materialList[_materialCnt - 1].Tf = vec;
			}
			else if (wordList[0] == "illum")
			{
				_materialList[_materialCnt - 1].illum = std::stoi(wordList[1]);
			}
			else if (wordList[0] == "map_Ka")
			{
				_materialList[_materialCnt - 1].map_Ka.assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "map_Kd")
			{
				_materialList[_materialCnt - 1].map_Kd.assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "map_Ks")
			{
				_materialList[_materialCnt - 1].map_Ks.assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "map_Ns")
			{
				_materialList[_materialCnt - 1].map_Ns.assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "map_bump")
			{
				_materialList[_materialCnt - 1].map_bump.assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "map_d")
			{
				_materialList[_materialCnt - 1].map_d.assign(wordList[1].begin(), wordList[1].end());
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

bool MtlFile::SplitString(std::string line, char delimiter, std::vector<std::string>& dst)
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
