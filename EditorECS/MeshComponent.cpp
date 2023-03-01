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
		MaterialSlot->SetContext(Context);
		DXTextureManager::GetInstance()->Load(L"../resource/Default.bmp");
		DXTexture* DefaultTexture = DXTextureManager::GetInstance()->getTexture(L"../resource/Default.bmp");
		MaterialSlot->AddTexture(DefaultTexture);
		MaterialSlot->SetPixelShader(DXShaderManager::GetInstance()->GetPixelShader(L"../include/Core/HLSL/PS_Light.hlsl"));
	}

	isCreated = true;

	return true;
}
