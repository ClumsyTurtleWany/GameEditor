#pragma once
#include "Define.h"
#include "Material.h"
#include "Face.h"

class Mesh
{
public:
	std::wstring Name;
	std::wstring MaterialName;

	std::vector<Vertex> Vertices;
	std::vector<Face>	Faces;
	std::vector<DWORD>	Indices;

	int VertexCount = 0;
	int FaceCount = 0;
	int IndexCount = 0;

	bool isCreated = false;

	Material* MaterialSlot = nullptr;

public:
	Mesh();
	virtual ~Mesh();

public:
	virtual bool Initialize() = 0;
	virtual bool Render() = 0;

public:
	void SetMaterial(Material* material);
};