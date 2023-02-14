#pragma once
#include "Define.h"
#include "DXTextureManager.hpp"
#include "Material.h"
#include "DXDevice.hpp"
#include "DXShaderManager.h"

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
	Material*		MaterialSlot = nullptr;

	bool isCreated = false;

public:
	MeshComponent() {};
	~MeshComponent()
	{
		delete MaterialSlot;
		MaterialSlot = nullptr;
	}

public:
	bool Render();
	bool Initialize();
};

inline bool MeshComponent::Render()
{
	if (!isCreated)
	{
		Initialize();
	}

	UINT Strides = sizeof(Vertex); // 정점 1개의 바이트 용량
	UINT Offsets = 0; // 정점 버퍼에서 출발 지점(바이트)
	DEVICE->GetContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &Strides, &Offsets);
	DEVICE->GetContext()->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DEVICE->GetContext()->UpdateSubresource(VertexBuffer, 0, NULL, &Vertices.at(0), 0, 0);
	DEVICE->GetContext()->UpdateSubresource(IndexBuffer, 0, NULL, &Indices.at(0), 0, 0);

	MaterialSlot->Apply();

	DEVICE->GetContext()->DrawIndexed(static_cast<UINT>(Indices.size()), 0, 0);
	//ID3D11ShaderResourceView* resourceView = NULL;
	//CONTEXT->PSSetShaderResources(0, 1, &resourceView);

	return true;
}

inline bool MeshComponent::Initialize()
{
	if (isCreated)
	{
		return true;
	}

	VertexBuffer = DXShaderManager::getInstance()->CreateVertexBuffer(Vertices);
	IndexBuffer = DXShaderManager::getInstance()->CreateIndexBuffer(Indices);

	if (MaterialSlot == nullptr)
	{
		MaterialSlot = new Material;
		DXTextureManager::getInstance()->Load(L"../resource/Default.bmp");
		DXTexture* DefaultTexture = DXTextureManager::getInstance()->getTexture(L"../resource/Default.bmp");
		MaterialSlot->AddTexture(DefaultTexture);
		MaterialSlot->SetPixelShader(DXShaderManager::getInstance()->GetPixelShader(L"../include/Core/HLSL/PS_Default.hlsl"));
	}

	return true;
}
