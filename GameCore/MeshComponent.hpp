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
	ID3D11DeviceContext* Context;
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
		if (MaterialSlot != nullptr)
		{
			delete MaterialSlot;
			MaterialSlot = nullptr;
		}
	}

public:
	bool Render();
	bool Initialize();

public:
	void SetContext(ID3D11DeviceContext* context) { Context = context; };
};

inline bool MeshComponent::Render()
{
	if (!isCreated)
	{
		Initialize();
	}

	UINT Strides = sizeof(Vertex); // 정점 1개의 바이트 용량
	UINT Offsets = 0; // 정점 버퍼에서 출발 지점(바이트)
	Context->IASetVertexBuffers(0, 1, &VertexBuffer, &Strides, &Offsets);
	Context->UpdateSubresource(VertexBuffer, 0, NULL, &Vertices.at(0), 0, 0);

	MaterialSlot->Apply();

	if (IndexBuffer == nullptr)
	{
		Context->Draw(static_cast<UINT>(Vertices.size()), 0);
	}
	else
	{
		Context->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		Context->UpdateSubresource(IndexBuffer, 0, NULL, &Indices.at(0), 0, 0);
		Context->DrawIndexed(static_cast<UINT>(Indices.size()), 0, 0);
	}
	
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
		MaterialSlot->SetContext(Context);
		DXTextureManager::getInstance()->Load(L"../resource/Default.bmp");
		DXTexture* DefaultTexture = DXTextureManager::getInstance()->getTexture(L"../resource/Default.bmp");
		MaterialSlot->AddTexture(DefaultTexture);
		MaterialSlot->SetPixelShader(DXShaderManager::getInstance()->GetPixelShader(L"../include/Core/HLSL/PS_Light.hlsl"));
	}

	isCreated = true;

	return true;
}
