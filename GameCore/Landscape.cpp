#include "Landscape.h"
#include "TransformComponent.h"
#include "DXSamplerState.hpp"

Landscape::Landscape()
{
	Initialize();
}

Landscape::~Landscape()
{

}

void Landscape::Build(int row, int column, int sectionSize)
{
	Components.clear();
	for (int compRow = 0; compRow < row; compRow++)
	{
		for (int compCol = 0; compCol < column; compCol++)
		{
			LandscapeComponent component;
			component.Row = compRow;
			component.Column = compCol;
			for (int sectRow = 0; sectRow < sectionSize + 1; sectRow++)
			{
				for (int sectCol = 0; sectCol < sectionSize + 1; sectCol++)
				{
					float x = compCol * (sectionSize + 1) + sectCol;
					float z = compRow * (sectionSize + 1) + sectRow;
					Vector3 pos = Vector3(x, 0.0f, z);
					Vector4 color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
					Vector3 normal = Vector3(0.0f, 1.0f, 0.0f);

					float u = sectCol / sectionSize;
					float v = sectRow / sectionSize;
					Vector2 texture = Vector2(u, v);
					component.Vertices.push_back(Vertex(pos, normal, color, texture));
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
					// 2 3
					// 0 1

					// 0 2 1 1 2 3

					component.Indices[idx + 0] = sectRow * sectionSize + sectCol;
					component.Indices[idx + 1] = (sectRow + 1) * sectionSize + sectCol + 1;
					component.Indices[idx + 2] = sectRow * sectionSize + sectCol + 1;

					component.Indices[idx + 3] = sectRow * sectionSize + sectCol + 1;
					component.Indices[idx + 4] = (sectRow + 1) * sectionSize + sectCol + 1;
					component.Indices[idx + 5] = (sectRow + 1) * sectionSize + sectCol + 2;

					idx += 6;
				}
			}
			component.SetContext(Context);

			Components.push_back(component);
		}
	}

}

void Landscape::PreRender()
{
	Context->IASetInputLayout(VertexLayout);
	Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Context->VSSetShader(VertexShader, NULL, 0);
	Context->HSSetShader(HullShader, NULL, 0);
	Context->DSSetShader(DomainShader, NULL, 0);
	Context->GSSetShader(GeometryShader, NULL, 0);
	Context->UpdateSubresource(TransformBuffer, 0, NULL, &TransformData, 0, 0);
	Context->VSSetConstantBuffers(0, 1, &TransformBuffer);
	Context->RSSetState(DXSamplerState::pDefaultRSWireFrame);

	for (auto& it : Components)
	{
		it.Render();
	}
}

void Landscape::Render()
{
	Context->IASetInputLayout(VertexLayout);
	Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Context->VSSetShader(VertexShader, NULL, 0);
	Context->HSSetShader(HullShader, NULL, 0);
	Context->DSSetShader(DomainShader, NULL, 0);
	Context->GSSetShader(GeometryShader, NULL, 0);
	Context->UpdateSubresource(TransformBuffer, 0, NULL, &TransformData, 0, 0);
	Context->VSSetConstantBuffers(0, 1, &TransformBuffer);
	Context->RSSetState(DXSamplerState::pDefaultRSSolid);

	for (auto& it : Components)
	{
		it.Render();
	}
}

void Landscape::SetContext(ID3D11DeviceContext* context)
{
	Context = context;
}

void Landscape::UpdateTransformMatrix(const TransformComponent& transform)
{
	DirectX::FXMVECTOR q = DirectX::XMQuaternionRotationRollPitchYawFromVector(transform.Rotation);
	TransformData.Mat = DirectX::XMMatrixAffineTransformation(transform.Scale, transform.Translation, q, transform.Translation);
	TransformData.Mat = TransformData.Mat.Transpose();
}

bool Landscape::Initialize()
{
	VertexLayout = DXShaderManager::getInstance()->GetInputLayout(InputLayoutType::StaticMesh);
	VertexShader = DXShaderManager::getInstance()->GetVertexShader(L"../include/Core/HLSL/VS_StaticMesh.hlsl");
	TransformBuffer = DXShaderManager::getInstance()->CreateConstantBuffer<TransformMatrix>(TransformData);

	return true;
}
