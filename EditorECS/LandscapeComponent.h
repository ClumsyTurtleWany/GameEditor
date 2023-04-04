#pragma once
#include "Define.h"
#include "Face.h"
#include "SplattingData.h"

struct LandscapeComponent //: public MeshComponent
{
public:
	std::vector<Vertex>		Vertices;
	std::vector<Face>		Faces;
	std::vector<DWORD>		Indices;

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
	DXTexture* SplattingTexture;

	bool isCreated = false;
	bool isEditable = false;

public:
	int Row;
	int Column;
	DirectX::BoundingBox Box;

public:
	bool Initialize(bool editable = false);
	bool Render();
	
	bool CreateSplattingData();
	bool Splatting(Vector3 pos, float radius, int idx, float strength);
};