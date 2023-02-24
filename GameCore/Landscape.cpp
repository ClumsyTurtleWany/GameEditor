#include "Landscape.h"
#include "TransformComponent.h"
#include "DXSamplerState.hpp"

Landscape::Landscape()
{
	Initialize();
	//this->AddComponent<TransformComponent>();
	//this->Name = L"Landscape" + std::to_wstring(this->ID);
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
			size_t vertexCnt = (sectionSize + 1) * (sectionSize + 1);
			component.Vertices.resize(vertexCnt);
			float cellDistance = 1.0f;
			float cx = static_cast<float>(compCol) * static_cast<float>(sectionSize) + static_cast<float>(sectionSize) / 2.0f;
			float cz = static_cast<float>(compRow) * static_cast<float>(sectionSize) + static_cast<float>(sectionSize) / 2.0f;
			component.Box = DirectX::BoundingOrientedBox(Vector3(cx, 0.0f, cz), Vector3(sectionSize, 1.0f, sectionSize), Vector4(0.0f, 0.0f, 0.0f, 1.0f));
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
						Face(component.Vertices[component.Indices[idx + 0]],
							component.Vertices[component.Indices[idx + 1]],
							component.Vertices[component.Indices[idx + 2]]));

					component.Faces.push_back(
						Face(component.Vertices[component.Indices[idx + 3]],
							component.Vertices[component.Indices[idx + 4]],
							component.Vertices[component.Indices[idx + 5]]));

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
	
	//UINT offset[1] = {0};
	//Context->SOSetTargets(1, &StreamOutputBuffer, offset);


	int intersectCompCnt = 0;
	int nonRenderCompCnt = 0;
	for (auto& it : Components)
	{
		if (MainCamera != nullptr)
		{
			bool isIntersect = MainCamera->Frustum.Intersects(it.Box);
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

void Landscape::SetContext(ID3D11DeviceContext* context)
{
	Context = context;
}

void Landscape::SetCamera(Camera* camera)
{
	MainCamera = camera;
}

void Landscape::UpdateTransformMatrix(const TransformComponent& transform)
{
	DirectX::FXMVECTOR q = DirectX::XMQuaternionRotationRollPitchYawFromVector(transform.Rotation);
	TransformData.Mat = DirectX::XMMatrixAffineTransformation(transform.Scale, Vector3(0.0f, 0.0f, 0.0f), q, transform.Translation);
	TransformData.Mat = TransformData.Mat.Transpose();
}

bool Landscape::Initialize()
{
	VertexLayout = DXShaderManager::getInstance()->GetInputLayout(InputLayoutType::StaticMesh);
	VertexShader = DXShaderManager::getInstance()->GetVertexShader(L"../include/Core/HLSL/VS_StaticMesh.hlsl");
	TransformBuffer = DXShaderManager::getInstance()->CreateConstantBuffer<TransformMatrix>(TransformData);
	//StreamOutputBuffer = DXShaderManager::getInstance()->CreateStreamOutputBuffer();


	return true;
}
