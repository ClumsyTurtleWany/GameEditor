#include "MeshComponent.hpp"

MeshComponent::MeshComponent()
{

}

MeshComponent::~MeshComponent()
{
	if (MaterialSlot != nullptr)
	{
		delete MaterialSlot;
		MaterialSlot = nullptr;
	}
}

bool MeshComponent::Render()
{
	if (!isCreated)
	{
		Initialize();
	}

	UINT Strides = sizeof(Vertex); // 정점 1개의 바이트 용량
	UINT Offsets = 0; // 정점 버퍼에서 출발 지점(바이트)
	DXDevice::g_pImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Strides, &Offsets);
	DXDevice::g_pImmediateContext->UpdateSubresource(VertexBuffer, 0, NULL, &Vertices.at(0), 0, 0);

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

	//ID3D11ShaderResourceView* resourceView = NULL;
	//CONTEXT->PSSetShaderResources(0, 1, &resourceView);

	return true;
}

bool MeshComponent::Initialize()
{
	if (isCreated)
	{
		return true;
	}

	VertexBuffer = DXShaderManager::GetInstance()->CreateVertexBuffer(Vertices);
	IndexBuffer = DXShaderManager::GetInstance()->CreateIndexBuffer(Indices);

	if (MaterialSlot == nullptr)
	{
		MaterialSlot = new Material;
		DXTextureManager::GetInstance()->Load(L"../resource/Default.bmp");
		DXTexture* DefaultTexture = DXTextureManager::GetInstance()->GetTexture(L"../resource/Default.bmp");
		MaterialSlot->SetDiffuseTexture(DefaultTexture);
	}
	MaterialSlot->SetPixelShader(DXShaderManager::GetInstance()->GetPixelShader(L"Light"));

	isCreated = true;

	return true;
}

void MeshComponent::SetMaterial(Material* material)
{
	MaterialSlot = material;
}