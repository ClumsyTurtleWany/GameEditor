#include "SkeletalMesh.h"
#include "MaterialManager.h"

SkeletalMesh::SkeletalMesh()
{
}

SkeletalMesh::~SkeletalMesh()
{
}

bool SkeletalMesh::Render()
{
	//DXDevice::g_pImmediateContext->UpdateSubresource(VertexBuffer, 0, NULL, &Vertices.at(0), 0, 0);
	//DXDevice::g_pImmediateContext->UpdateSubresource(SkinningBuffer, 0, NULL, &SkinningIWList.at(0), 0, 0);
	ID3D11Buffer* pBufferStream[2] = { VertexBuffer, SkinningBuffer }; // 멀티 스트림
	UINT Strides[2] = { sizeof(Vertex), sizeof(IWData) }; // 각 버퍼 데이터의 바이트 용량
	UINT Offsets[2] = { 0, 0 }; // 정점 버퍼에서 출발 지점(바이트)
	DXDevice::g_pImmediateContext->IASetVertexBuffers(0, 2, pBufferStream, Strides, Offsets);
		
	MaterialSlot->Apply();

	if (IndexBuffer == nullptr)
	{
		DXDevice::g_pImmediateContext->Draw(static_cast<UINT>(Vertices.size()), 0);
	}
	else
	{
		DXDevice::g_pImmediateContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		DXDevice::g_pImmediateContext->UpdateSubresource(IndexBuffer, 0, NULL, &Indices.at(0), 0, 0);
		DXDevice::g_pImmediateContext->DrawIndexed(static_cast<UINT>(Indices.size()), 0, 0);
	}

	return true;
}

bool SkeletalMesh::Initialize()
{
	if (isCreated)
	{
		return true;
	}

	VertexBuffer = DXShaderManager::GetInstance()->CreateVertexBuffer(Vertices);
	if (VertexBuffer == nullptr)
	{
		OutputDebugString(L"EditorCore::SkeletalMesh::Initialize::Failed Create Vertex Buffer.");
		return false;
	}
	else
	{
		DXDevice::g_pImmediateContext->UpdateSubresource(VertexBuffer, 0, NULL, &Vertices.at(0), 0, 0);
	}

	if (!Indices.empty())
	{
		IndexBuffer = DXShaderManager::GetInstance()->CreateIndexBuffer(Indices);
		if (IndexBuffer == nullptr)
		{
			OutputDebugString(L"EditorCore::SkeletalMesh::Initialize::Failed Create Index Buffer.");
			return false;
		}
	}

	if (!SkinningIWList.empty())
	{
		SkinningBuffer = DXShaderManager::GetInstance()->CreateVertexBuffer<IWData>(SkinningIWList);
		if (SkinningBuffer == nullptr)
		{
			OutputDebugString(L"EditorCore::SkeletalMesh::Initialize::Failed Create Skinning Buffer.");
			return false;
		}
		else
		{
			DXDevice::g_pImmediateContext->UpdateSubresource(SkinningBuffer, 0, NULL, &SkinningIWList.at(0), 0, 0);
		}
	}

	if (MaterialSlot == nullptr)
	{
		MaterialSlot = MaterialManager::GetInstance()->GetMaterial(L"Default");
		if (MaterialSlot == nullptr)
		{
			OutputDebugString(L"EditorCore::StaticMesh::Initialize::Failed Get Default Material.");
			return false;
		}
	}
	isCreated = true;

	return true;
}
