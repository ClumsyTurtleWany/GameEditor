#include "SkyBoxComponent.h"


SkyBoxComponent::SkyBoxComponent()
{
	Vertices.resize(24);
	//// Front
	//Vertices[0] = Vertex(Vector3(-1.0f, +1.0f, -1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f));
	//Vertices[1] = Vertex(Vector3(+1.0f, +1.0f, -1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f));
	//Vertices[2] = Vertex(Vector3(-1.0f, -1.0f, -1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f));
	//Vertices[3] = Vertex(Vector3(+1.0f, -1.0f, -1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f));

	//// Left
	//Vertices[4] = Vertex(Vector3(-1.0f, +1.0f, +1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f));
	//Vertices[5] = Vertex(Vector3(-1.0f, +1.0f, -1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f));
	//Vertices[6] = Vertex(Vector3(-1.0f, -1.0f, +1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f));
	//Vertices[7] = Vertex(Vector3(-1.0f, -1.0f, -1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f));

	//// Right
	//Vertices[8] = Vertex(Vector3(+1.0f, +1.0f, -1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f));
	//Vertices[9] = Vertex(Vector3(+1.0f, +1.0f, +1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f));
	//Vertices[10] = Vertex(Vector3(+1.0f, -1.0f, -1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f));
	//Vertices[11] = Vertex(Vector3(+1.0f, -1.0f, +1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f));

	//// Back
	//Vertices[12] = Vertex(Vector3(+1.0f, +1.0f, +1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f));
	//Vertices[13] = Vertex(Vector3(-1.0f, +1.0f, +1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f));
	//Vertices[14] = Vertex(Vector3(+1.0f, -1.0f, +1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f));
	//Vertices[15] = Vertex(Vector3(-1.0f, -1.0f, +1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f));

	//// Upper
	//Vertices[16] = Vertex(Vector3(-1.0f, +1.0f, +1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f));
	//Vertices[17] = Vertex(Vector3(+1.0f, +1.0f, +1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f));
	//Vertices[18] = Vertex(Vector3(-1.0f, +1.0f, -1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f));
	//Vertices[19] = Vertex(Vector3(+1.0f, +1.0f, -1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f));

	//// Lower
	//Vertices[20] = Vertex(Vector3(-1.0f, -1.0f, -1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f));
	//Vertices[21] = Vertex(Vector3(+1.0f, -1.0f, -1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f));
	//Vertices[22] = Vertex(Vector3(-1.0f, -1.0f, +1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f));
	//Vertices[23] = Vertex(Vector3(+1.0f, -1.0f, +1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f));

	// Front
	Vertices[0] = Vertex(Vector3(-0.5f, +0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f));
	Vertices[1] = Vertex(Vector3(+0.5f, +0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f));
	Vertices[2] = Vertex(Vector3(-0.5f, -0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f));
	Vertices[3] = Vertex(Vector3(+0.5f, -0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f));

	// Left
	Vertices[4] = Vertex(Vector3(-0.5f, +0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f));
	Vertices[5] = Vertex(Vector3(-0.5f, +0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f));
	Vertices[6] = Vertex(Vector3(-0.5f, -0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f));
	Vertices[7] = Vertex(Vector3(-0.5f, -0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f));

	// Right
	Vertices[8] = Vertex(Vector3(+0.5f, +0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f));
	Vertices[9] = Vertex(Vector3(+0.5f, +0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f));
	Vertices[10] = Vertex(Vector3(+0.5f, -0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f));
	Vertices[11] = Vertex(Vector3(+0.5f, -0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f));

	// Back
	Vertices[12] = Vertex(Vector3(+0.5f, +0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f));
	Vertices[13] = Vertex(Vector3(-0.5f, +0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f));
	Vertices[14] = Vertex(Vector3(+0.5f, -0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f));
	Vertices[15] = Vertex(Vector3(-0.5f, -0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f));

	// Upper
	Vertices[16] = Vertex(Vector3(-0.5f, +0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f));
	Vertices[17] = Vertex(Vector3(+0.5f, +0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f));
	Vertices[18] = Vertex(Vector3(-0.5f, +0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f));
	Vertices[19] = Vertex(Vector3(+0.5f, +0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f));

	// Lower
	Vertices[20] = Vertex(Vector3(-0.5f, -0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f));
	Vertices[21] = Vertex(Vector3(+0.5f, -0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f));
	Vertices[22] = Vertex(Vector3(-0.5f, -0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f));
	Vertices[23] = Vertex(Vector3(+0.5f, -0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f));


	Indices.resize(36);
	// Front
	Indices[0] = 0;
	Indices[1] = 1;
	Indices[2] = 2;
	Indices[3] = 2;
	Indices[4] = 1;
	Indices[5] = 3;

	// Left
	Indices[6] = 4;
	Indices[7] = 5;
	Indices[8] = 6;
	Indices[9] = 6;
	Indices[10] = 5;
	Indices[11] = 7;

	// Right
	Indices[12] = 8;
	Indices[13] = 9;
	Indices[14] = 10;
	Indices[15] = 10;
	Indices[16] = 9;
	Indices[17] = 11;

	// Back
	Indices[18] = 12;
	Indices[19] = 13;
	Indices[20] = 14;
	Indices[21] = 14;
	Indices[22] = 13;
	Indices[23] = 15;

	// Upper
	Indices[24] = 16;
	Indices[25] = 17;
	Indices[26] = 18;
	Indices[27] = 18;
	Indices[28] = 17;
	Indices[29] = 19;

	// Lower
	Indices[30] = 20;
	Indices[31] = 21;
	Indices[32] = 22;
	Indices[33] = 22;
	Indices[34] = 21;
	Indices[35] = 23;

	Initialize();
}

SkyBoxComponent::~SkyBoxComponent()
{
}

bool SkyBoxComponent::Initialize()
{
	VertexShader = DXShaderManager::GetInstance()->GetVertexShader(L"Sky");
	PixelShader = DXShaderManager::GetInstance()->GetPixelShader(L"Default");
	TransformBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<TransformMatrix>(TransformData);
	VertexBuffer = DXShaderManager::GetInstance()->CreateVertexBuffer(Vertices);
	IndexBuffer = DXShaderManager::GetInstance()->CreateIndexBuffer(Indices);
	CameraMatrixBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<CameraMatrix>(CameraMatrixData);

	return true;
}

bool SkyBoxComponent::Render()
{
	DXDevice::g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DXDevice::g_pImmediateContext->VSSetShader(VertexShader, NULL, 0);
	DXDevice::g_pImmediateContext->HSSetShader(HullShader, NULL, 0);
	DXDevice::g_pImmediateContext->DSSetShader(DomainShader, NULL, 0);
	DXDevice::g_pImmediateContext->GSSetShader(GeometryShader, NULL, 0);
	DXDevice::g_pImmediateContext->PSSetShader(PixelShader, NULL, 0);
	DXDevice::g_pImmediateContext->UpdateSubresource(TransformBuffer, 0, NULL, &TransformData, 0, 0);
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(0, 1, &TransformBuffer);

	UINT Strides = sizeof(Vertex); // 정점 1개의 바이트 용량
	UINT Offsets = 0; // 정점 버퍼에서 출발 지점(바이트)
	DXDevice::g_pImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Strides, &Offsets);
	DXDevice::g_pImmediateContext->UpdateSubresource(VertexBuffer, 0, NULL, &Vertices.at(0), 0, 0);

	DXDevice::g_pImmediateContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DXDevice::g_pImmediateContext->UpdateSubresource(IndexBuffer, 0, NULL, &Indices.at(0), 0, 0);

	DXDevice::g_pImmediateContext->RSSetState(DXSamplerState::pRSSolid_CullFront);

	if (FrontTexture != nullptr)
	{
		ID3D11ShaderResourceView* front = FrontTexture->GetResourceView();
		DXDevice::g_pImmediateContext->PSSetShaderResources(0, 1, &front);
		DXDevice::g_pImmediateContext->DrawIndexed(6, 6 * 0, 0);
	}

	if (LeftTexture != nullptr)
	{
		ID3D11ShaderResourceView* left = LeftTexture->GetResourceView();
		DXDevice::g_pImmediateContext->PSSetShaderResources(0, 1, &left);
		DXDevice::g_pImmediateContext->DrawIndexed(6, 6 * 1, 0);
	}

	if (RightTexture != nullptr)
	{
		ID3D11ShaderResourceView* right = RightTexture->GetResourceView();
		DXDevice::g_pImmediateContext->PSSetShaderResources(0, 1, &right);
		DXDevice::g_pImmediateContext->DrawIndexed(6, 6 * 2, 0);
	}

	if (BackTexture != nullptr)
	{
		ID3D11ShaderResourceView* back = BackTexture->GetResourceView();
		DXDevice::g_pImmediateContext->PSSetShaderResources(0, 1, &back);
		DXDevice::g_pImmediateContext->DrawIndexed(6, 6 * 3, 0);
	}

	if (UpperTexture != nullptr)
	{
		ID3D11ShaderResourceView* upper = UpperTexture->GetResourceView();
		DXDevice::g_pImmediateContext->PSSetShaderResources(0, 1, &upper);
		DXDevice::g_pImmediateContext->DrawIndexed(6, 6 * 4, 0);
	}

	if (LowerTexture != nullptr)
	{
		ID3D11ShaderResourceView* lower = LowerTexture->GetResourceView();
		DXDevice::g_pImmediateContext->PSSetShaderResources(0, 1, &lower);
		DXDevice::g_pImmediateContext->DrawIndexed(6, 6 * 5, 0);
	}
	
	return true;
}

void SkyBoxComponent::SetFrontTexture(DXTexture* texture)
{
	FrontTexture = texture;
}

void SkyBoxComponent::SetBackTexture(DXTexture* texture)
{
	BackTexture = texture;
}

void SkyBoxComponent::SetLeftTexture(DXTexture* texture)
{
	LeftTexture = texture;
}

void SkyBoxComponent::SetRightTexture(DXTexture* texture)
{
	RightTexture = texture;
}

void SkyBoxComponent::SetUpperTexture(DXTexture* texture)
{
	UpperTexture = texture;
}

void SkyBoxComponent::SetLowerTexture(DXTexture* texture)
{
	LowerTexture = texture;
}

void SkyBoxComponent::SetScale(Vector3 scale)
{
	Scale = scale;
}

void SkyBoxComponent::UpdateTransformMatrix(const TransformComponent& transform)
{
	DirectX::FXMVECTOR q = DirectX::XMQuaternionRotationRollPitchYawFromVector(transform.Rotation);
	TransformData.Mat = DirectX::XMMatrixAffineTransformation(transform.Scale, transform.Translation, q, transform.Translation);
	TransformData.InversedMat = DirectX::XMMatrixInverse(0, TransformData.Mat);
	TransformData.Mat = TransformData.Mat.Transpose();
}

void SkyBoxComponent::UpdateCamera(Camera* camera, const TransformComponent& transform)
{
	if (camera == nullptr)
	{
		return;
	}

	DirectX::FXMVECTOR q = DirectX::XMQuaternionRotationRollPitchYawFromVector(Vector3(0.0f, 0.0f, 0.0f));
	TransformData.Mat = DirectX::XMMatrixAffineTransformation(transform.Scale, Vector3(0.0f, 0.0f, 0.0f), q, Vector3(0.0f, 0.0f, 0.0f));
	//TransformData.Mat = DirectX::XMMatrixAffineTransformation(transform.Scale, camera->Pos, q, camera->Pos);
	TransformData.InversedMat = DirectX::XMMatrixInverse(0, TransformData.Mat);
	TransformData.Mat = TransformData.Mat.Transpose();


	CameraMatrixData.View = camera->View;
	CameraMatrixData.View._41 = 0.0f;
	CameraMatrixData.View._42 = 0.0f;
	CameraMatrixData.View._43 = 0.0f;
	CameraMatrixData.View = CameraMatrixData.View.Transpose();


	//CameraMatrixData.Projection = DirectX::XMMatrixPerspectiveFovLH(PI * 0.25, 1.0f, 1.0f, 10000.0f);
	CameraMatrixData.Projection = camera->Projection;
	CameraMatrixData.Projection = CameraMatrixData.Projection.Transpose();

	DXDevice::g_pImmediateContext->UpdateSubresource(CameraMatrixBuffer, 0, NULL, &CameraMatrixData, 0, 0);
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(1, 1, &CameraMatrixBuffer);
}
