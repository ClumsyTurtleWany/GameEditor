#pragma once
#include "Define.h"
#include "DXTextureManager.hpp"
#include "Material.h"
#include "DXDevice.hpp"
#include "DXShaderManager.h"

struct Face
{
	Vertex V0;
	Vertex V1;
	Vertex V2;

	Face() : V0(Vertex()), V1(Vertex()), V2(Vertex()) {};
	Face(const Vertex& v0, const Vertex& v1, const Vertex& v2)
		: V0(v0), V1(v1), V2(v2)
	{

	}

	Vector3 GetNormal()
	{

	}
};

class MeshComponent
{
public:
	std::vector<Vertex> Vertices;
	std::vector<Face>	Faces;
	std::vector<DWORD>	Indices;
	std::vector<IWData> IWList;

	int VertexCount = 0;
	int FaceCount = 0;
	int IndexCount = 0;

	ID3D11Buffer*	VertexBuffer = nullptr;
	ID3D11Buffer*	IndexBuffer = nullptr;
	ID3D11Buffer*	IWBuffer = nullptr;
	Material*		MaterialSlot = nullptr;

	bool isCreated = false;

public:
	MeshComponent();
	~MeshComponent();

public:
	bool Render();
	bool Initialize();

public:
	void SetMaterial(Material* material);
};