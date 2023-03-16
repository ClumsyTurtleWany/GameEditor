#include "StaticMeshComponent.h"
#include <fstream>
#include <sstream>

StaticMeshComponent::StaticMeshComponent()
{
	Initialize();
}
StaticMeshComponent::~StaticMeshComponent()
{
}

bool StaticMeshComponent::Initialize()
{
	if (isCreated)
	{
		return true;
	}

	VertexLayout = DXShaderManager::GetInstance()->GetInputLayout(L"StaticMesh");
	if (VertexLayout == nullptr)
	{
		OutputDebugString(L"EditorECS::StaticMeshComponent::Initialize::Failed Get Input Layout.");
		return false;
	}

	VertexShader = DXShaderManager::GetInstance()->GetVertexShader(L"StaticMesh");
	if (VertexShader == nullptr)
	{
		OutputDebugString(L"EditorECS::StaticMeshComponent::Initialize::Failed Get Vertex Shader.");
		return false;
	}

	TransformBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<TransformMatrix>(TransformData);
	if (TransformBuffer == nullptr)
	{
		OutputDebugString(L"EditorECS::StaticMeshComponent::Initialize::Failed Create Transform Buffer.");
		return false;
	}

	isCreated = true;

	return true;
}

bool StaticMeshComponent::Render()
{
	DXDevice::g_pImmediateContext->IASetInputLayout(VertexLayout);
	DXDevice::g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DXDevice::g_pImmediateContext->VSSetShader(VertexShader, NULL, 0);
	DXDevice::g_pImmediateContext->HSSetShader(HullShader, NULL, 0);
	DXDevice::g_pImmediateContext->DSSetShader(DomainShader, NULL, 0);
	DXDevice::g_pImmediateContext->GSSetShader(GeometryShader, NULL, 0);
	DXDevice::g_pImmediateContext->UpdateSubresource(TransformBuffer, 0, NULL, &TransformData, 0, 0);
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(0, 1, &TransformBuffer);

	DXDevice::g_pImmediateContext->PSSetConstantBuffers(4, 1, &TransformBuffer);

	for (auto& it : Meshes)
	{
		it.Render();
	}

	return true;
}

bool StaticMeshComponent::Save(std::wstring filename)
{
	return false;
}

bool StaticMeshComponent::Load(std::wstring filename)
{
	size_t filepathIdx = filename.find_last_of('/');
	std::wstring filepath;
	for (int idx = 0; idx <= filepathIdx; idx++)
	{
		filepath.push_back(filename.at(idx));
	}

	std::wstring fileName;
	for (int idx = filepathIdx + 1; idx < filename.size(); idx++)
	{
		fileName.push_back(filename.at(idx));
	}

	std::fstream file(filename);
	if (!file.is_open())
	{
		return false;
	}
	else
	{
		_meshCnt = 0;
		while (!file.eof())
		{
			std::string lineData;
			std::getline(file, lineData, '\n');

			std::vector<std::string> wordList;
			if (!SplitString(lineData, ' ', wordList))
			{
				continue;
			}

			if (wordList[0] == "mtllib")
			{
				std::wstring mtlFilePath = filepath;
				std::wstring mtlFileName;
				mtlFileName.assign(wordList[1].begin(), wordList[1].end());
				mtlFilePath += mtlFileName;
				_mtl.Load(mtlFilePath);
			}
			else if (wordList[0] == "o")
			{
				MESH mesh;
				mesh.name = wordList[1];
				_meshList.push_back(mesh);
				_meshCnt++;
			}
			else if (wordList[0] == "v")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				_meshList[_meshCnt - 1].positionList.push_back(vec);
			}
			else if (wordList[0] == "vt")
			{
				Vector2 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				_meshList[_meshCnt - 1].uvList.push_back(vec);
			}
			else if (wordList[0] == "vn")
			{
				Vector3 vec;
				vec.x = std::stof(wordList[1]);
				vec.y = std::stof(wordList[2]);
				vec.z = std::stof(wordList[3]);
				_meshList[_meshCnt - 1].normalList.push_back(vec);
			}
			else if (wordList[0] == "usemtl")
			{
				for (int listIdx = 1; listIdx < wordList.size(); listIdx++)
				{
					_meshList[_meshCnt - 1].materialList.push_back(wordList[listIdx]);
				}
			}
			else if (wordList[0] == "s")
			{
				_meshList[_meshCnt - 1].smoothingGroup = wordList[1];
			}
			else if (wordList[0] == "f")
			{
				FACE face;
				for (int listIdx = 1; listIdx < wordList.size(); listIdx++)
				{
					std::vector<std::string> wordSplitList;
					if (!SplitString(wordList[listIdx], '/', wordSplitList))
					{
						continue;
					}

					face.v[listIdx - 1].pos = std::stoi(wordSplitList[0]);
					face.v[listIdx - 1].uv = std::stoi(wordSplitList[1]);
					face.v[listIdx - 1].normal = std::stoi(wordSplitList[2]);
				}

				_meshList[_meshCnt - 1].faceList.push_back(face);
			}
			else if (wordList[0] == "g")
			{
				_meshList[_meshCnt - 1].group = wordList[1];
			}
			else
			{
				continue;
			}

		}
		file.close();
		return true;
	}
}

void StaticMeshComponent::UpdateTransformMatrix(const TransformComponent& transform)
{
	DirectX::FXMVECTOR q = DirectX::XMQuaternionRotationRollPitchYawFromVector(transform.Rotation);
	TransformData.Mat = DirectX::XMMatrixAffineTransformation(transform.Scale, transform.Translation, q, transform.Translation);
	TransformData.InversedMat = DirectX::XMMatrixInverse(0, TransformData.Mat);
	TransformData.Mat = TransformData.Mat.Transpose();
}