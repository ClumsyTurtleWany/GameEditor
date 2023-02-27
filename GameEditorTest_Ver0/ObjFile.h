#pragma once
#include "MtlFile.h"
#include "Vertex.h"

struct FACEINDEX
{
	int pos = 0;
	int uv = 0;
	int normal = 0;

	FACEINDEX()
	{
		pos = 0;
		uv = 0;
		normal = 0;
	}
};

struct FACE
{
	union
	{
		struct
		{
			FACEINDEX v1;
			FACEINDEX v2;
			FACEINDEX v3;
		};
		FACEINDEX v[3];
	};

	FACE()
	{
		for (int idx = 0; idx < 3; idx++)
		{
			v[idx].pos = 0;
			v[idx].uv = 0;
			v[idx].normal = 0;
		}
	}
};

struct MESH
{
	std::string					name;
	std::string					group;
	std::string					smoothingGroup;

	std::vector<Vector3>		positionList;
	std::vector<Vector2>		uvList;
	std::vector<Vector3>		normalList;
	std::vector<std::string>	materialList;
	std::vector<FACE>			faceList;
};

class ObjFile
{
public:
	std::vector<MESH>	_meshList;
	MtlFile				_mtl;
	int					_meshCnt;

public:
	bool Load(std::wstring filename);

public:
	bool SplitString(std::string line, char delimiter, std::vector<std::string>& dst);
};