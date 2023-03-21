#include "LandscapeComponent.h"

bool LandscapeComponent::Initialize()
{
	if (isCreated)
	{
		return true;
	}

	ComputeShader = DXShaderManager::GetInstance()->GetComputeShader(L"Splatting");
	VertexBuffer = DXShaderManager::GetInstance()->CreateVertexBuffer(Vertices);
	IndexBuffer = DXShaderManager::GetInstance()->CreateIndexBuffer(Indices);
	StructuredBuffer = DXShaderManager::GetInstance()->CreateStructuredBuffer<SplattingData>(SplattingInfo);
	StructuredBufferSRV = DXTextureManager::GetInstance()->CreateShaderResourceViewBuffer(StructuredBuffer);
	AlphaTexture = DXTextureManager::GetInstance()->CreateAlphaTexture(1024, 1024);
	CopyTexture = DXTextureManager::GetInstance()->CreateCopyTexture(AlphaTexture);

	DXDevice::g_pImmediateContext->CopyResource(CopyTexture->GetTexture2D(), AlphaTexture->GetTexture2D());

	isCreated = true;

	return true;
}

bool LandscapeComponent::Render()
{
	if (!isCreated)
	{
		Initialize();
	}

	UINT Strides = sizeof(Vertex); // ���� 1���� ����Ʈ �뷮
	UINT Offsets = 0; // ���� ���ۿ��� ��� ����(����Ʈ)
	DXDevice::g_pImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Strides, &Offsets);
	DXDevice::g_pImmediateContext->UpdateSubresource(VertexBuffer, 0, NULL, &Vertices.at(0), 0, 0);

	ID3D11ShaderResourceView* pSRV = CopyTexture->GetResourceView();
	DXDevice::g_pImmediateContext->PSSetShaderResources(5, 1, &pSRV);

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

void LandscapeComponent::Splatting(Vector3 pos, float radius, int idx, float strength)
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

	DXDevice::g_pImmediateContext->UpdateSubresource(StructuredBuffer, 0, NULL, &SplattingInfo, 0, 0);

	DXDevice::g_pImmediateContext->CSSetShader(ComputeShader, NULL, 0);
	ID3D11ShaderResourceView* pTextureSRV = BaseTexture->GetResourceView();
	DXDevice::g_pImmediateContext->CSSetShaderResources(0, 1, &pTextureSRV);
	ID3D11ShaderResourceView* pCopySRV = CopyTexture->GetResourceView();
	DXDevice::g_pImmediateContext->CSSetShaderResources(1, 1, &pCopySRV);
	DXDevice::g_pImmediateContext->CSSetShaderResources(2, 1, &StructuredBufferSRV);

	ID3D11UnorderedAccessView* pUAV = AlphaTexture->GetUAV();
	DXDevice::g_pImmediateContext->CSSetUnorderedAccessViews(0, 1, &pUAV, NULL);

	// Dispatch(x, y, z)
	// x: column
	// y: row
	// z: depth
	// example) Dispatch(5, 3, 1) == SV_GroupID[1][3][5]
	// SV_GroupID�� 10 * 8 * 3 = 240���� ������(SV_GroupThreadID)�� ����.
	// ����
	// SV_GroupThreadID = (7, 5, 0)
	// SV_GroupID = (2, 1, 0) �� ��,
	// SV_DispatchThreadID = (2, 1, 0) * (10, 8, 3) + (7, 5, 0) = (27, 13, 0)
	// SV_GroupIndex = 0 * 10 * 8 + 5 * 10 + 7 = 57
	// x * y * z�� �ִ� 1024�� ������ �ȵ�. ���� 2���� �迭 ���� 32 * 32 * 1 ���
	DXDevice::g_pImmediateContext->Dispatch(SplattingInfo.DispatchX, SplattingInfo.DispatchY, 1);

	// clear
	DXDevice::g_pImmediateContext->CSSetShader(nullptr, nullptr, 0);
	ID3D11UnorderedAccessView* pNullUAV = nullptr;
	DXDevice::g_pImmediateContext->CSSetUnorderedAccessViews(0, 1, &pNullUAV, NULL);
	ID3D11ShaderResourceView* pNullSRV[2] = { nullptr, nullptr };
	DXDevice::g_pImmediateContext->CSSetShaderResources(0, 2, pNullSRV);

	DXDevice::g_pImmediateContext->CopyResource(CopyTexture->GetTexture2D(), AlphaTexture->GetTexture2D());
}