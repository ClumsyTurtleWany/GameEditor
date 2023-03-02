#pragma once
#include "Define.h"
#include "MeshComponent.hpp"
#include "SplattingData.h"


struct LandscapeComponent //: public MeshComponent
{
public:
	ID3D11DeviceContext* Context;

	std::vector<Vertex> Vertices;
	std::vector<Face>	Faces;
	std::vector<DWORD>	Indices;

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
	DirectX::BoundingOrientedBox Box;


public:
	bool Render()
	{
		if (!isCreated)
		{
			Initialize();
		}

		UINT Strides = sizeof(Vertex); // 정점 1개의 바이트 용량
		UINT Offsets = 0; // 정점 버퍼에서 출발 지점(바이트)
		Context->IASetVertexBuffers(0, 1, &VertexBuffer, &Strides, &Offsets);
		Context->UpdateSubresource(VertexBuffer, 0, NULL, &Vertices.at(0), 0, 0);

		ID3D11ShaderResourceView* pSRV = CopyTexture->getResourceView();
		Context->PSSetShaderResources(5, 1, &pSRV);

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

		return true;
	}

	bool Initialize()
	{
		if (isCreated)
		{
			return true;
		}

		ComputeShader = DXShaderManager::getInstance()->GetComputeShader(L"../include/Core/HLSL/CS_LandscapeSplatting.hlsl");
		VertexBuffer = DXShaderManager::getInstance()->CreateVertexBuffer(Vertices);
		IndexBuffer = DXShaderManager::getInstance()->CreateIndexBuffer(Indices);
		StructuredBuffer = DXShaderManager::getInstance()->CreateStructuredBuffer<SplattingData>(SplattingInfo);
		StructuredBufferSRV = DXShaderManager::getInstance()->CreateShaderResourceViewBuffer(StructuredBuffer);
		AlphaTexture = DXTextureManager::getInstance()->CreateAlphaTexture(1024, 1024);
		CopyTexture = DXTextureManager::getInstance()->CreateTexture(AlphaTexture);

		Context->CopyResource(CopyTexture->GetTexture2D(), AlphaTexture->GetTexture2D());

		isCreated = true;

		return true;
	}

	void SetContext(ID3D11DeviceContext* context) 
	{
		Context = context; 
	}

	void Splatting(Vector3 pos, float radius, int idx, float strength)
	{
		SplattingInfo.Position = pos;
		SplattingInfo.Radius = radius;
		SplattingInfo.Index = idx;
		SplattingInfo.Strength = strength;
		//SplattingInfo.TextureWidth = 32;
		//SplattingInfo.TextureHeight = 32;
		SplattingInfo.DispatchX = 32;
		SplattingInfo.DispatchY = 32;
		SplattingInfo.MinVertex = MinVertex;
		SplattingInfo.MaxVertex = MaxVertex;

		Context->UpdateSubresource(StructuredBuffer, 0, NULL, &SplattingInfo, 0, 0);

		Context->CSSetShader(ComputeShader, NULL, 0);
		ID3D11ShaderResourceView* pTextureSRV = BaseTexture->getResourceView();
		Context->CSSetShaderResources(0, 1, &pTextureSRV);
		ID3D11ShaderResourceView* pCopySRV = CopyTexture->getResourceView();
		Context->CSSetShaderResources(1, 1, &pCopySRV);
		Context->CSSetShaderResources(2, 1, &StructuredBufferSRV);

		ID3D11UnorderedAccessView* pUAV = AlphaTexture->GetUAV();
		Context->CSSetUnorderedAccessViews(0, 1, &pUAV, NULL);

		// Dispatch(x, y, z)
		// x: column
		// y: row
		// z: depth
		// example) Dispatch(5, 3, 1) == SV_GroupID[1][3][5]
		// SV_GroupID는 10 * 8 * 3 = 240개의 쓰레드(SV_GroupThreadID)를 가짐.
		// 예시
		// SV_GroupThreadID = (7, 5, 0)
		// SV_GroupID = (2, 1, 0) 일 때,
		// SV_DispatchThreadID = (2, 1, 0) * (10, 8, 3) + (7, 5, 0) = (27, 13, 0)
		// SV_GroupIndex = 0 * 10 * 8 + 5 * 10 + 7 = 57
		// x * y * z는 최대 1024를 넘으면 안됨. 보통 2차원 배열 기준 32 * 32 * 1 사용
		Context->Dispatch(SplattingInfo.DispatchX, SplattingInfo.DispatchY, 1);

		// clear
		Context->CSSetShader(nullptr, nullptr, 0);
		ID3D11UnorderedAccessView* pNullUAV = nullptr;
		Context->CSSetUnorderedAccessViews(0, 1, &pNullUAV, NULL);
		ID3D11ShaderResourceView* pNullSRV[2] = { nullptr, nullptr };
		Context->CSSetShaderResources(0, 2, pNullSRV);

		Context->CopyResource(CopyTexture->GetTexture2D(), AlphaTexture->GetTexture2D());
	}

};