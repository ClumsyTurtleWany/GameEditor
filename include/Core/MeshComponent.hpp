#pragma once
#include "Define.h"
#include "DXTexture.hpp"
#include "Material.h"
#include "DXDevice.hpp"

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

class MeshComponent
{
public:
	std::vector<Vertex> Vertices;
	std::vector<Face>	Faces;
	std::vector<DWORD>	Indices;

	int VertexCount = 0;
	int FaceCount = 0;
	int IndexCount = 0;

	ID3D11Buffer*	VertexBuffer = nullptr;
	ID3D11Buffer*	IndexBuffer = nullptr;
	Material*		Material = nullptr;

public:
	MeshComponent() {};

public:
	bool Render();
};

bool MeshComponent::Render()
{
	UINT Strides = sizeof(Vertex); // 정점 1개의 바이트 용량
	UINT Offsets = 0; // 정점 버퍼에서 출발 지점(바이트)
	DEVICE->GetContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &Strides, &Offsets);
	DEVICE->GetContext()->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DEVICE->GetContext()->UpdateSubresource(VertexBuffer, 0, NULL, &Vertices.at(0), 0, 0);
	DEVICE->GetContext()->UpdateSubresource(IndexBuffer, 0, NULL, &Indices.at(0), 0, 0);

	Material->Apply();

	DEVICE->GetContext()->DrawIndexed(static_cast<UINT>(Indices.size()), 0, 0);
}