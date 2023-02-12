#pragma once
#include "Define.h"
#include "DXTexture.hpp"
#include "Material.h"

struct Face
{
	union
	{
		struct
		{
			Vertex _v0;
			Vertex _v1;
			Vertex _v2;
		}
		v[3];
	};

	Vector3 Normal;
};

struct MeshComponent
{
public:
	std::vector<Vertex> Vertices;
	std::vector<Face>	Faces;
	std::vector<DWORD>	Indeices;
	Material			Material;

	int VertexCount;
	int FaceCount;
	int IndexCount;

public:
	MeshComponent() {};
};