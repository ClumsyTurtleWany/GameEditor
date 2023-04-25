#include "StaticMeshComponent.h"
#include <fstream>
#include <sstream>
#include "MaterialManager.h"

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

	ShadowVertexShader = DXShaderManager::GetInstance()->GetVertexShader(L"Shadow");
	if (ShadowVertexShader == nullptr)
	{
		OutputDebugString(L"EditorECS::StaticMeshComponent::Initialize::Failed Get Shadow Vertex Shader.");
		return false;
	}

	ShadowPixelShader = DXShaderManager::GetInstance()->GetPixelShader(L"Shadow");
	if (ShadowPixelShader == nullptr)
	{
		OutputDebugString(L"EditorECS::StaticMeshComponent::Initialize::Failed Get Shadow Pixel Shader.");
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

bool StaticMeshComponent::RenderShadow()
{
	DXDevice::g_pImmediateContext->IASetInputLayout(VertexLayout);
	DXDevice::g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DXDevice::g_pImmediateContext->VSSetShader(ShadowVertexShader, NULL, 0);
	DXDevice::g_pImmediateContext->HSSetShader(HullShader, NULL, 0);
	DXDevice::g_pImmediateContext->DSSetShader(DomainShader, NULL, 0);
	DXDevice::g_pImmediateContext->GSSetShader(GeometryShader, NULL, 0);
	//DXDevice::g_pImmediateContext->PSSetShader(ShadowPixelShader, NULL, 0);
	DXDevice::g_pImmediateContext->PSSetShader(nullptr, NULL, 0);
	DXDevice::g_pImmediateContext->UpdateSubresource(TransformBuffer, 0, NULL, &TransformData, 0, 0);
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(0, 1, &TransformBuffer);

	DXDevice::g_pImmediateContext->PSSetConstantBuffers(4, 1, &TransformBuffer);

	for (auto& it : Meshes)
	{
		it.RenderShadow();
	}

	return true;
}

bool StaticMeshComponent::Save(std::wstring filename)
{
	std::fstream file(filename, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
	if (!file.is_open())
	{
		OutputDebugString(L"EditorCore::Material::Save::Failed Open File.");
		return false;
	}
	else
	{
		std::string name;
		name.assign(Name.begin(), Name.end());
		file << std::endl;
		file << "ComponentName " << name << std::endl;
		size_t meshCnt = Meshes.size();
		file << "MeshCount " << std::to_string(meshCnt) << std::endl;
		for (size_t idx = 0; idx < meshCnt; idx++)
		{
			name.assign(Meshes[idx].Name.begin(), Meshes[idx].Name.end());
			file << "MeshName " << name << std::endl;
			Meshes[idx].VertexCount = Meshes[idx].Vertices.size();
			file << "VertexCount " << std::to_string(Meshes[idx].VertexCount) << std::endl;
			for (size_t vertexIdx = 0; vertexIdx < Meshes[idx].VertexCount; vertexIdx++)
			{
				file << "Position " << std::to_string(Meshes[idx].Vertices[vertexIdx].Pos.x) << " "
					<< std::to_string(Meshes[idx].Vertices[vertexIdx].Pos.y) << " "
					<< std::to_string(Meshes[idx].Vertices[vertexIdx].Pos.z) << std::endl;

				file << "Normal " << std::to_string(Meshes[idx].Vertices[vertexIdx].Normal.x) << " "
					<< std::to_string(Meshes[idx].Vertices[vertexIdx].Normal.y) << " "
					<< std::to_string(Meshes[idx].Vertices[vertexIdx].Normal.z) << std::endl;

				file << "Color " << std::to_string(Meshes[idx].Vertices[vertexIdx].Color.x) << " "
					<< std::to_string(Meshes[idx].Vertices[vertexIdx].Color.y) << " "
					<< std::to_string(Meshes[idx].Vertices[vertexIdx].Color.z) << " "
					<< std::to_string(Meshes[idx].Vertices[vertexIdx].Color.w)	<< std::endl;

				file << "Texture " << std::to_string(Meshes[idx].Vertices[vertexIdx].Texture.x) << " "
					<< std::to_string(Meshes[idx].Vertices[vertexIdx].Texture.y)  << std::endl;

				file << "Tangent " << std::to_string(Meshes[idx].Vertices[vertexIdx].Tangent.x) << " "
					<< std::to_string(Meshes[idx].Vertices[vertexIdx].Tangent.y) << " "
					<< std::to_string(Meshes[idx].Vertices[vertexIdx].Tangent.z) << std::endl;
			}

			Meshes[idx].IndexCount = Meshes[idx].Indices.size();
			file << "IndexCount " << std::to_string(Meshes[idx].IndexCount) << std::endl;
			for (size_t indexIdx = 0; indexIdx < Meshes[idx].IndexCount; indexIdx += 3)
			{
				file << "Face " << std::to_string(Meshes[idx].Indices[indexIdx + 0]) << " "
					<< std::to_string(Meshes[idx].Indices[indexIdx + 1]) << " "
					<< std::to_string(Meshes[idx].Indices[indexIdx + 2]) << std::endl;
			}
		}
		
		file.close();
		return true;
	}
}

bool StaticMeshComponent::Load(std::wstring filename)
{
	std::fstream file(filename);
	if (!file.is_open())
	{
		return false;
	}
	else
	{
		while (!file.eof())
		{
			std::string lineData;
			std::getline(file, lineData, '\n');

			std::vector<std::string> wordList;
			if (!SplitString(lineData, ' ', wordList))
			{
				continue;
			}

			if (wordList[0] == "ComponentName")
			{
				Name.assign(wordList[1].begin(), wordList[1].end());
			}
			else if (wordList[0] == "MeshCount")
			{
				size_t meshCnt = std::stoi(wordList[1]);
				Meshes.resize(meshCnt);
				for (size_t meshIdx = 0; meshIdx < meshCnt; meshIdx++)
				{
					std::getline(file, lineData, '\n');
					wordList.clear();
					if (!SplitString(lineData, ' ', wordList))
					{
						return false;
					}
					Meshes[meshIdx].Name.assign(wordList[1].begin(), wordList[1].end());
					
					std::getline(file, lineData, '\n');
					wordList.clear();
					if (!SplitString(lineData, ' ', wordList))
					{
						return false;
					}
					Meshes[meshIdx].VertexCount = std::stoi(wordList[1]);
					Meshes[meshIdx].Vertices.resize(Meshes[meshIdx].VertexCount);
					for (size_t vertexIdx = 0; vertexIdx < Meshes[meshIdx].VertexCount; vertexIdx++)
					{
						std::getline(file, lineData, '\n');
						wordList.clear();
						if (!SplitString(lineData, ' ', wordList))
						{
							return false;
						}
						Vector3 pos;
						pos.x = std::stof(wordList[1]);
						pos.y = std::stof(wordList[2]);
						pos.z = std::stof(wordList[3]);
						Meshes[meshIdx].Vertices[vertexIdx].Pos = pos;

						std::getline(file, lineData, '\n');
						wordList.clear();
						if (!SplitString(lineData, ' ', wordList))
						{
							return false;
						}
						Vector3 normal;
						normal.x = std::stof(wordList[1]);
						normal.y = std::stof(wordList[2]);
						normal.z = std::stof(wordList[3]);
						Meshes[meshIdx].Vertices[vertexIdx].Normal = normal;

						std::getline(file, lineData, '\n');
						wordList.clear();
						if (!SplitString(lineData, ' ', wordList))
						{
							return false;
						}
						Vector4 color;
						color.x = std::stof(wordList[1]);
						color.y = std::stof(wordList[2]);
						color.z = std::stof(wordList[3]);
						color.w = std::stof(wordList[4]);
						Meshes[meshIdx].Vertices[vertexIdx].Color = color;

						std::getline(file, lineData, '\n');
						wordList.clear();
						if (!SplitString(lineData, ' ', wordList))
						{
							return false;
						}
						Vector2 texture;
						texture.x = std::stof(wordList[1]);
						texture.y = std::stof(wordList[2]);
						Meshes[meshIdx].Vertices[vertexIdx].Texture = texture;

						std::getline(file, lineData, '\n');
						wordList.clear();
						if (!SplitString(lineData, ' ', wordList))
						{
							return false;
						}
						Vector3 tangent;
						tangent.x = std::stof(wordList[1]);
						tangent.y = std::stof(wordList[2]);
						tangent.z = std::stof(wordList[3]);
						Meshes[meshIdx].Vertices[vertexIdx].Tangent = tangent;
					}


					// Indices
					std::getline(file, lineData, '\n');
					wordList.clear();
					if (!SplitString(lineData, ' ', wordList))
					{
						return false;
					}
					Meshes[meshIdx].IndexCount = std::stoi(wordList[1]);
					if (Meshes[meshIdx].IndexCount > 0)
					{
						Meshes[meshIdx].Indices.resize(Meshes[meshIdx].IndexCount);
						Meshes[meshIdx].FaceCount = Meshes[meshIdx].IndexCount / 3;
						Meshes[meshIdx].Faces.resize(Meshes[meshIdx].FaceCount);
						for (size_t indexIdx = 0; indexIdx < Meshes[meshIdx].IndexCount; indexIdx += 3)
						{
							std::getline(file, lineData, '\n');
							wordList.clear();
							if (!SplitString(lineData, ' ', wordList))
							{
								return false;
							}
							Meshes[meshIdx].Indices[indexIdx + 0] = std::stoi(wordList[1]);
							Meshes[meshIdx].Indices[indexIdx + 1] = std::stoi(wordList[2]);
							Meshes[meshIdx].Indices[indexIdx + 2] = std::stoi(wordList[3]);

							Meshes[meshIdx].Faces[indexIdx].V0 = Meshes[meshIdx].Indices[indexIdx + 0];
							Meshes[meshIdx].Faces[indexIdx].V1 = Meshes[meshIdx].Indices[indexIdx + 1];
							Meshes[meshIdx].Faces[indexIdx].V2 = Meshes[meshIdx].Indices[indexIdx + 2];
						}
					}

				}
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
	Vector3 rotation = transform.Rotation / 180.0f * PI;
	DirectX::FXMVECTOR q = DirectX::XMQuaternionRotationRollPitchYawFromVector(rotation);
	//TransformData.Mat = DirectX::XMMatrixAffineTransformation(transform.Scale, transform.Translation, q, transform.Translation);
	TransformData.Mat = DirectX::XMMatrixAffineTransformation(transform.Scale, Vector3(0.0f, 0.0f, 0.0f), q, transform.Translation);
	TransformData.InversedMat = DirectX::XMMatrixInverse(0, TransformData.Mat);
	TransformData.Mat = TransformData.Mat.Transpose();
}

bool StaticMeshComponent::SplitString(std::string line, char delimiter, std::vector<std::string>& dst)
{
	std::istringstream ss(line);
	std::string word;
	bool isValid = false;
	while (std::getline(ss, word, delimiter))
	{
		isValid = true;
		dst.push_back(word);
	}

	return isValid;
}

bool StaticMeshComponent::WriteXML(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	TiXmlElement* pStaticMeshComponent = new TiXmlElement("StaticMeshComponent");
	parent->LinkEndChild(pStaticMeshComponent);
	
	if (!WriteString(pStaticMeshComponent, FilePath, "Path"))
	{

	}

	return true;
}

bool StaticMeshComponent::ReadXML(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	TiXmlElement* root = parent->FirstChildElement("StaticMeshComponent");

	if (ReadString(root, "Path", FilePath))
	{
		if (!Load(FilePath))
		{
			return false;
		}
	}

	return true;
}

void StaticMeshComponent::UpdateTransformData(const TransformMatrix& transform)
{
	TransformData = transform;
	return;
}

bool StaticMeshComponent::Load(std::string filename)
{
	std::ifstream STM_in(filename, std::ios::binary);
	if (STM_in.is_open())
	{
		int meshCnt = 0;
		STM_in.read(reinterpret_cast<char*>(&meshCnt), sizeof(int));
		FilePath = filename;
		Meshes.resize(meshCnt);
		for (int i = 0; i < meshCnt; i++)
		{
			int name_size;
			STM_in.read(reinterpret_cast<char*>(&name_size), sizeof(int));
			char temp[256];
			STM_in.read(reinterpret_cast<char*>(&temp[0]), name_size);
			Meshes[i].Name = to_mw(temp);

			int matname_size;
			STM_in.read(reinterpret_cast<char*>(&matname_size), sizeof(int));
			STM_in.read(reinterpret_cast<char*>(&temp[0]), matname_size);
			Meshes[i].MaterialName = to_mw(temp);

			int vertices_size;
			STM_in.read(reinterpret_cast<char*>(&vertices_size), sizeof(int));
			Meshes[i].Vertices.resize(vertices_size);
			STM_in.read(reinterpret_cast<char*>(&Meshes[i].Vertices[0]), sizeof(Vertex) * vertices_size);
			int faces_size;
			STM_in.read(reinterpret_cast<char*>(&faces_size), sizeof(int));
			if (faces_size > 0)
			{
				Meshes[i].Faces.resize(faces_size);
				STM_in.read(reinterpret_cast<char*>(&Meshes[i].Faces[0]), sizeof(Face) * faces_size);
			}

			int indices_size;
			STM_in.read(reinterpret_cast<char*>(&indices_size), sizeof(int));
			if (indices_size > 0)
			{
				Meshes[i].Indices.resize(indices_size);
				STM_in.read(reinterpret_cast<char*>(&Meshes[i].Indices[0]), sizeof(DWORD) * indices_size);
			}

		}
		int fbxname_size;
		STM_in.read(reinterpret_cast<char*>(&fbxname_size), sizeof(int));
		std::string temp;
		temp.resize(fbxname_size);
		STM_in.read(reinterpret_cast<char*>(&temp[0]), fbxname_size);
		FBXName = to_mw(temp);

		int meshname_size;
		STM_in.read(reinterpret_cast<char*>(&meshname_size), sizeof(int));
		temp.resize(meshname_size);
		STM_in.read(reinterpret_cast<char*>(&temp[0]), meshname_size);
		Name = to_mw(temp);

		for (int i = 0; i < meshCnt; i++)
		{
			std::string mtlname = "../resource/FBX/MTL/";
			mtlname += to_wm(Meshes[i].MaterialName);
			mtlname += ".mtl";

			std::ifstream mtl_in(mtlname, std::ios::binary);
			if (mtl_in.is_open())
			{
				Material* material = MaterialManager::GetInstance()->CreateMaterial(Meshes[i].MaterialName);
				material->Type = MaterialType::Light;

				int size;
				std::string temp;
				std::wstring _mtlPath = L"../resource/FBX/MTL/";
				std::wstring _mtlName;

				mtl_in.read(reinterpret_cast<char*>(&size), sizeof(int));
				if (size > 1)
				{
					temp.resize(size);
					mtl_in.read(reinterpret_cast<char*>(&temp[0]), size);
					_mtlName = to_mw(temp);
					_mtlName = _mtlName.substr(_mtlName.find_last_of(L"/") + 1);
					material->DiffuseTextureName = _mtlPath + _mtlName;
				}

				mtl_in.read(reinterpret_cast<char*>(&size), sizeof(int));
				if (size > 1)
				{
					temp.resize(size);
					mtl_in.read(reinterpret_cast<char*>(&temp[0]), size);
					_mtlName = to_mw(temp);
					_mtlName = _mtlName.substr(_mtlName.find_last_of(L"/") + 1);
					material->NormalTextureName = _mtlPath + _mtlName;
				}

				mtl_in.read(reinterpret_cast<char*>(&size), sizeof(int));
				if (size > 1)
				{
					temp.resize(size);
					mtl_in.read(reinterpret_cast<char*>(&temp[0]), size);
					_mtlName = to_mw(temp);
					_mtlName = _mtlName.substr(_mtlName.find_last_of(L"/") + 1);
					material->AmbientTextureName = _mtlPath + _mtlName;
				}

				mtl_in.read(reinterpret_cast<char*>(&size), sizeof(int));
				if (size > 1)
				{
					temp.resize(size);
					mtl_in.read(reinterpret_cast<char*>(&temp[0]), size);
					_mtlName = to_mw(temp);
					_mtlName = _mtlName.substr(_mtlName.find_last_of(L"/") + 1);
					material->SpecularTextureName = _mtlPath + _mtlName;
				}

				mtl_in.read(reinterpret_cast<char*>(&size), sizeof(int));
				if (size > 1)
				{
					temp.resize(size);
					mtl_in.read(reinterpret_cast<char*>(&temp[0]), size);
					_mtlName = to_mw(temp);
					_mtlName = _mtlName.substr(_mtlName.find_last_of(L"/") + 1);
					material->EmissiveTextureName = _mtlPath + _mtlName;
				}

				if (!material->Create())
				{
					material = MaterialManager::GetInstance()->GetMaterial(L"Default");
				}

				mtl_in.close();
			}

			Meshes[i].MaterialSlot = MaterialManager::GetInstance()->GetMaterial(Meshes[i].MaterialName);
			Meshes[i].Initialize();
		}
		isCreated = false;
		Initialize();

		STM_in.close();

	}

	return true;
}