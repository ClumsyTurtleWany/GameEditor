#include "StaticMesh.h"
#include "MaterialManager.h"

StaticMesh::StaticMesh()
{

}

StaticMesh::~StaticMesh()
{

}

bool StaticMesh::Render()
{
	if (!isCreated)
	{
		if (!Initialize())
		{
			OutputDebugString(L"EditorCore::StaticMesh::Render::Failed Initialize Static Mesh.");
			return false;
		}
	}

	UINT Strides = sizeof(Vertex); // 정점 1개의 바이트 용량
	UINT Offsets = 0; // 정점 버퍼에서 출발 지점(바이트)
	DXDevice::g_pImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Strides, &Offsets);
	DXDevice::g_pImmediateContext->UpdateSubresource(VertexBuffer, 0, NULL, &Vertices.at(0), 0, 0);

	if (MaterialSlot == nullptr)
	{
		OutputDebugString(L"EditorCore::StaticMesh::Render::Material is NULL.");
		return false;
	}

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

bool StaticMesh::Initialize()
{
	if (isCreated)
	{
		return true;
	}

	VertexBuffer = DXShaderManager::GetInstance()->CreateVertexBuffer(Vertices);
	if (VertexBuffer == nullptr)
	{
		OutputDebugString(L"EditorCore::StaticMesh::Initialize::Failed Create Vertex Buffer.");
		return false;
	}

	IndexBuffer = DXShaderManager::GetInstance()->CreateIndexBuffer(Indices);
	/*if (IndexBuffer == nullptr)
	{
		OutputDebugString(L"EditorCore::StaticMesh::Initialize::Failed Create Index Buffer.");
		return false;
	}*/

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