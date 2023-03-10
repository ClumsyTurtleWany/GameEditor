#include "ObjFile.h"

bool ObjFile::Load(std::wstring filename)
{
	size_t filepathIdx = filename.find_last_of('/');
	std::wstring filepath;
	for (int idx = 0; idx <= filepathIdx; idx++)
	{
		filepath.push_back(filename.at(idx));
	}
	std::wstring fileName;
	for (int idx = filepathIdx + 1; idx < filename.size(); idx++)
	{
		fileName.push_back(filename.at(idx));
	}

	std::fstream file(filename);
	if (!file.is_open())
	{
		return false;
	}
	else
	{
		_meshCnt = 0;
		while (!file.eof())
		{
			std::string lineData;
			std::getline(file, lineData, '\n');

			std::vector<std::string> wordList;
			if (!SplitString(lineData, ' ', wordList))
			{
				continue;
			}

			if (wordList[0] == "mtllib")
			{
				std::wstring mtlFilePath = filepath;
				std::wstring mtlFileName;
				mtlFileName.assign(wordList[1].begin(), wordList[1].end());
				mtlFilePath += mtlFileName;
				_mtl.Load(mtlFilePath);
			}
			else if (wordList[0] == "o")
			{
				MESH mesh;
				mesh.name = wordList[1];
				_meshList.push_back(mesh);
				_meshCnt++;
			}
			else if (wordList[0] == "v")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				_meshList[_meshCnt - 1].positionList.push_back(vec);
			}
			else if (wordList[0] == "vt")
			{
				Vector2 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				_meshList[_meshCnt - 1].uvList.push_back(vec);
			}
			else if (wordList[0] == "vn")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				_meshList[_meshCnt - 1].normalList.push_back(vec);
			}
			else if (wordList[0] == "usemtl")
			{
				for (int listIdx = 1; listIdx < wordList.size(); listIdx++)
				{
					_meshList[_meshCnt - 1].materialList.push_back(wordList[listIdx]);
				}
			}
			else if (wordList[0] == "s")
			{
				_meshList[_meshCnt - 1].smoothingGroup = wordList[1];
			}
			else if (wordList[0] == "f")
			{
				FACE face;
				for (int listIdx = 1; listIdx < wordList.size(); listIdx++)
				{
					std::vector<std::string> wordSplitList;
					if (!SplitString(wordList[listIdx], '/', wordSplitList))
					{
						continue;
					}

					face.v[listIdx - 1].pos = std::stoi(wordSplitList[0]);
					face.v[listIdx - 1].uv = std::stoi(wordSplitList[1]);
					face.v[listIdx - 1].normal = std::stoi(wordSplitList[2]);
				}

				_meshList[_meshCnt - 1].faceList.push_back(face);
			}
			else if (wordList[0] == "g")
			{
				_meshList[_meshCnt - 1].group = wordList[1];
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

bool ObjFile::SplitString(std::string line, char delimiter, std::vector<std::string>& dst)
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
