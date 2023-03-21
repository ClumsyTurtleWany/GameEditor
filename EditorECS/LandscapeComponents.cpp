#include "LandscapeComponents.h"

bool LandscapeComponents::Initialize()
{
	VertexLayout = DXShaderManager::GetInstance()->GetInputLayout(L"StaticMesh");
	VertexShader = DXShaderManager::GetInstance()->GetVertexShader(L"StaticMesh");
	PixelShader = DXShaderManager::GetInstance()->GetPixelShader(L"Landscape");
	TransformBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<TransformMatrix>(TransformData);
	if (DXTextureManager::GetInstance()->Load(L"../resource/Default.bmp"))
	{
		BaseTexture = DXTextureManager::GetInstance()->GetTexture(L"../resource/Default.bmp");
	}

	LayerTextureList.resize(4);
	LayerTextureList[0] = BaseTexture;
	LayerTextureList[1] = BaseTexture;
	LayerTextureList[2] = BaseTexture;
	LayerTextureList[3] = BaseTexture;

	return true;
}

void LandscapeComponents::Build(int column, int row, int sectionSize, int cellDistance)
{
	Components.clear();
	Transform->Translation.x = -static_cast<float>(column) * static_cast<float>(sectionSize + 1) * static_cast<float>(cellDistance) / 2.0f;
	Transform->Translation.z = -static_cast<float>(row) * static_cast<float>(sectionSize + 1) * static_cast<float>(cellDistance) / 2.0f;
	for (int compRow = 0; compRow < row; compRow++)
	{
		for (int compCol = 0; compCol < column; compCol++)
		{
			LandscapeComponent component;
			component.Row = compRow;
			component.Column = compCol;
			size_t vertexCnt = (sectionSize + 1) * (sectionSize + 1);
			component.Vertices.resize(vertexCnt);
			float cx = static_cast<float>(compCol) * static_cast<float>(sectionSize) + static_cast<float>(sectionSize) / 2.0f;
			float cz = static_cast<float>(compRow) * static_cast<float>(sectionSize) + static_cast<float>(sectionSize) / 2.0f;

			for (int sectRow = 0; sectRow < sectionSize + 1; sectRow++)
			{
				for (int sectCol = 0; sectCol < sectionSize + 1; sectCol++)
				{
					size_t idx = sectRow * (sectionSize + 1) + sectCol;
					// compRow == 0, compCol == 0, x(0 ~ sectionSize + 1), z(0 ~ sectionSize + 1) 
					// compRow == 0, compCol == 1, x(sectionSize ~ sectionSize * compCol + sectionSize + 1), z(0 ~ sectionSize + 1)
					// compRow == 1, compCol == 0, x(0 ~ sectionSize + 1), z(sectionSize ~ sectionSize * compRow + sectionSize + 1) 
					// compRow == 1, compCol == 1, x(sectionSize ~ sectionSize * compCol + sectionSize + 1), z(0 ~ sectionSize + 1)
					float x = (sectionSize * compCol + sectCol) * cellDistance;
					float z = (sectionSize * compRow + sectRow) * cellDistance;
					Vector3 pos = Vector3(x, 0.0f, z);
					Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
					Vector3 normal = Vector3(0.0f, 1.0f, 0.0f);

					float u = static_cast<float>(sectCol) / static_cast<float>(sectionSize);
					float v = static_cast<float>(sectRow) / static_cast<float>(sectionSize);
					Vector2 texture = Vector2(u, v);
					component.Vertices[idx] = Vertex(pos, normal, color, texture);
					//component.Vertices.push_back(Vertex(pos, normal, color, texture));
					if (sectRow == 0 && sectCol == 0)
					{
						component.MinVertex = pos;
					}
					else if (sectRow == sectionSize && sectCol == sectionSize)
					{
						component.MaxVertex = pos;
					}
				}
			}

			size_t cellCnt = sectionSize * sectionSize;
			size_t faceCnt = sectionSize * sectionSize * 2;
			size_t IndexCount = faceCnt * 3;
			component.Indices.resize(IndexCount);
			size_t idx = 0;
			for (int sectRow = 0; sectRow < sectionSize; sectRow++)
			{
				for (int sectCol = 0; sectCol < sectionSize; sectCol++)
				{
					// 12 13 14 15 
					//  8  9 10 11
					//  4  5  6  7
					//  0  1  2  3
					// 
					// 0 4 1 1 4 5
					// 1 5 2 2 5 6
					// 2 6 3 3 6 7
					// 
					// 4 8 5 5 8 9
					// 5 9 6 6 9 10
					// 6 10 7 7 10 11
					// 
					// 8 12 9 9 12 13
					// 9 13 10 10 13 14
					// 10 14 11 11 14 15

					// sectRow == 0, sectCol == 0 // 0 4 1 1 4 5
					// sectRow == 0, sectCol == 1 // 1 5 2 2 5 6
					// sectRow == 0, sectCol == 2 // 2 6 3 3 6 7

					component.Indices[idx + 0] = sectRow * (sectionSize + 1) + sectCol;
					component.Indices[idx + 1] = (sectRow + 1) * (sectionSize + 1) + sectCol;
					component.Indices[idx + 2] = sectRow * (sectionSize + 1) + sectCol + 1;

					component.Indices[idx + 3] = sectRow * (sectionSize + 1) + sectCol + 1;
					component.Indices[idx + 4] = (sectRow + 1) * (sectionSize + 1) + sectCol;
					component.Indices[idx + 5] = (sectRow + 1) * (sectionSize + 1) + sectCol + 1;

					component.Faces.push_back(
						FacePointer(&component.Vertices[component.Indices[idx + 0]],
							&component.Vertices[component.Indices[idx + 1]],
							&component.Vertices[component.Indices[idx + 2]]));

					component.Faces.push_back(
						FacePointer(&component.Vertices[component.Indices[idx + 3]],
							&component.Vertices[component.Indices[idx + 4]],
							&component.Vertices[component.Indices[idx + 5]]));

					idx += 6;
				}
			}
			Vector3 center = (component.MinVertex + component.MaxVertex) / 2.0f;
			Vector3 extend = (component.MaxVertex - component.MinVertex) / 2.0f;
			extend.y += 0.01f;
			component.Box = DirectX::BoundingBox(center, extend);
			component.BaseTexture = BaseTexture;
			component.Initialize();
			Components.push_back(component);
		}
	}

}

void LandscapeComponents::Render()
{
	DXDevice::g_pImmediateContext->IASetInputLayout(VertexLayout);
	DXDevice::g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DXDevice::g_pImmediateContext->VSSetShader(VertexShader, NULL, 0);
	DXDevice::g_pImmediateContext->PSSetShader(PixelShader, NULL, 0);
	DXDevice::g_pImmediateContext->HSSetShader(HullShader, NULL, 0);
	DXDevice::g_pImmediateContext->DSSetShader(DomainShader, NULL, 0);
	DXDevice::g_pImmediateContext->GSSetShader(GeometryShader, NULL, 0);
	DXDevice::g_pImmediateContext->UpdateSubresource(TransformBuffer, 0, NULL, &TransformData, 0, 0);
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(0, 1, &TransformBuffer);
	DXDevice::g_pImmediateContext->RSSetState(DXSamplerState::pDefaultRSSolid);

	ID3D11ShaderResourceView* pSRV = BaseTexture->GetResourceView();
	DXDevice::g_pImmediateContext->PSSetShaderResources(0, 1, &pSRV);
	int layerTextureSize = min(LayerTextureList.size(), 4);
	for (int idx = 0; idx < layerTextureSize; idx++)
	{
		ID3D11ShaderResourceView* pLayerSRV = LayerTextureList[idx]->GetResourceView();
		DXDevice::g_pImmediateContext->PSSetShaderResources(idx + 1, 1, &pLayerSRV);
	}

	int intersectCompCnt = 0;
	int nonRenderCompCnt = 0;

	Vector3 rotation = Transform->Rotation / 180.0f * PI;
	DirectX::FXMVECTOR q = DirectX::XMQuaternionRotationRollPitchYawFromVector(rotation);
	Matrix transformMatrix = DirectX::XMMatrixAffineTransformation(Transform->Scale, Vector3(0.0f, 0.0f, 0.0f), q, Transform->Translation);
	for (auto& it : Components)
	{
		if (MainCamera != nullptr)
		{
			Vector3 center = Vector3(it.Box.Center.x, it.Box.Center.y, it.Box.Center.z);
			Vector3 newCenter = DirectX::XMVector3Transform(center, transformMatrix);
			DirectX::BoundingBox bBox = it.Box;
			bBox.Center.x = newCenter.x;
			bBox.Center.y = newCenter.y;
			bBox.Center.z = newCenter.z;
			bool isIntersect = MainCamera->Frustum.Intersects(bBox);
			if (isIntersect)
			{
				intersectCompCnt++;
				it.Render();
			}
			else
			{
				nonRenderCompCnt++;
				continue;
			}
		}
		else
		{
			it.Render();
		}
	}

	std::string debugStr = "Render Comp: " + std::to_string(intersectCompCnt) + ", Non Render Comp: " + std::to_string(nonRenderCompCnt) + "\n";
	OutputDebugStringA(debugStr.c_str());
}

void LandscapeComponents::SetCamera(Camera* camera)
{
	MainCamera = camera;
}

void LandscapeComponents::SetBaseTexture(DXTexture* texture)
{
	BaseTexture = texture;
}

void LandscapeComponents::AddLayerTexture(DXTexture* texture)
{
	LayerTextureList.push_back(texture);
}

void LandscapeComponents::UpdateTransformMatrix(const TransformComponent& transform)
{
	Vector3 rotation = transform.Rotation / 180.0f * PI;
	DirectX::FXMVECTOR q = DirectX::XMQuaternionRotationRollPitchYawFromVector(rotation);
	TransformData.Mat = DirectX::XMMatrixAffineTransformation(transform.Scale, Vector3(0.0f, 0.0f, 0.0f), q, transform.Translation);
	TransformData.InversedMat = DirectX::XMMatrixInverse(0, TransformData.Mat);
	TransformData.Mat = TransformData.Mat.Transpose();
}

void LandscapeComponents::SetSculptingData(Vector3 pos, float radius, float attenuation, float strength, float weight)
{
	SculptData.Position = Vector4(pos.x, pos.y, pos.z, 0.0f);
	SculptData.Radius = radius;
	SculptData.Attenuation = attenuation;
	SculptData.Strength = strength;
	SculptData.Weight = weight;
}

