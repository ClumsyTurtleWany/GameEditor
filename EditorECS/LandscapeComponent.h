#pragma once
#include "Define.h"
#include "Face.h"
#include "SplattingData.h"

struct FacePointer
{
	Vertex* V0 = nullptr;
	Vertex* V1 = nullptr;
	Vertex* V2 = nullptr;

	FacePointer(Vertex* v0, Vertex* v1, Vertex* v2)
		: V0(v0), V1(v1), V2(v2)
	{
	}
};

struct LandscapeComponent //: public MeshComponent
{
public:
	std::vector<Vertex>		Vertices;
	std::vector<FacePointer>	Faces;
	std::vector<DWORD>			Indices;

	Vector3 MinVertex;
	Vector3 MaxVertex;

	int VertexCount = 0;
	int FaceCount = 0;
	int IndexCount = 0;

	ID3D11ComputeShader* ComputeShader = nullptr;
	ID3D11Buffer* VertexBuffer = nullptr;
	ID3D11Buffer* IndexBuffer = nullptr;

	SplattingData SplattingInfo;
	ID3D11Buffer* StructuredBuffer;
	ID3D11ShaderResourceView* StructuredBufferSRV;
	DXTexture* BaseTexture;
	DXTexture* AlphaTexture;
	DXTexture* CopyTexture;

	bool isCreated = false;

public:
	int Row;
	int Column;
	DirectX::BoundingBox Box;

public:
	bool Initialize();
	bool Render();
	
	void Splatting(Vector3 pos, float radius, int idx, float strength);
};