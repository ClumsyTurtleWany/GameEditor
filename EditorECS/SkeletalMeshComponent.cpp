#include "SkeletalMeshComponent.h"
#include <fstream>

SkeletalMeshComponent::SkeletalMeshComponent()
{
	Initialize();
}

SkeletalMeshComponent::~SkeletalMeshComponent()
{

}

bool SkeletalMeshComponent::Render()
{
	DXDevice::g_pImmediateContext->IASetInputLayout(VertexLayout);
	DXDevice::g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DXDevice::g_pImmediateContext->VSSetShader(VertexShader, NULL, 0);
	DXDevice::g_pImmediateContext->HSSetShader(HullShader, NULL, 0);
	DXDevice::g_pImmediateContext->DSSetShader(DomainShader, NULL, 0);
	DXDevice::g_pImmediateContext->GSSetShader(GeometryShader, NULL, 0);
	DXDevice::g_pImmediateContext->UpdateSubresource(TransformBuffer, 0, NULL, &TransformData, 0, 0);
	DXDevice::g_pImmediateContext->UpdateSubresource(BPABuffer, 0, NULL, &BPAData, 0, 0);
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(0, 1, &TransformBuffer);
	DXDevice::g_pImmediateContext->PSSetConstantBuffers(4, 1, &TransformBuffer);
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(2, 1, &BPABuffer);

	DXDevice::g_pImmediateContext->PSSetConstantBuffers(4, 1, &TransformBuffer);

	for (auto& it : Meshes)
	{
		it.Render();
	}

	return true;
}

bool SkeletalMeshComponent::RenderTrail()
{
	DXDevice::g_pImmediateContext->IASetInputLayout(VertexLayout);
	DXDevice::g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DXDevice::g_pImmediateContext->VSSetShader(VertexShader, NULL, 0);
	DXDevice::g_pImmediateContext->HSSetShader(HullShader, NULL, 0);
	DXDevice::g_pImmediateContext->DSSetShader(DomainShader, NULL, 0);
	DXDevice::g_pImmediateContext->GSSetShader(GeometryShader, NULL, 0);
	DXDevice::g_pImmediateContext->UpdateSubresource(TransformBuffer, 0, NULL, &TransformData, 0, 0);
	DXDevice::g_pImmediateContext->UpdateSubresource(BPABuffer, 0, NULL, &BPAData, 0, 0);
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(0, 1, &TransformBuffer);
	DXDevice::g_pImmediateContext->PSSetConstantBuffers(4, 1, &TransformBuffer);
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(2, 1, &BPABuffer);

	DXDevice::g_pImmediateContext->PSSetConstantBuffers(4, 1, &TransformBuffer);

	for (auto& it : Meshes)
	{
		it.RenderTrail();
	}

	return true;
}

bool SkeletalMeshComponent::RenderShadow()
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
	DXDevice::g_pImmediateContext->UpdateSubresource(BPABuffer, 0, NULL, &BPAData, 0, 0);
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(0, 1, &TransformBuffer);
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(2, 1, &BPABuffer);

	DXDevice::g_pImmediateContext->PSSetConstantBuffers(4, 1, &TransformBuffer);

	for (auto& it : Meshes)
	{
		it.RenderShadow();
	}

	return true;
}

bool SkeletalMeshComponent::Initialize()
{
	if (isCreated)
	{
		return true;
	}

	VertexLayout = DXShaderManager::GetInstance()->GetInputLayout(L"SkeletalMesh");
	VertexShader = DXShaderManager::GetInstance()->GetVertexShader(L"SkeletalMesh");
	TransformBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<TransformMatrix>(TransformData);
	BPABuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<BindPoseAnimationData>(BPAData);
	isCreated = true;

	ShadowVertexShader = DXShaderManager::GetInstance()->GetVertexShader(L"SkeletalShadow");
	if (ShadowVertexShader == nullptr)
	{
		OutputDebugString(L"EditorECS::SkeletalMeshComponent::Initialize::Failed Get Shadow Vertex Shader.");
		return false;
	}

	ShadowPixelShader = DXShaderManager::GetInstance()->GetPixelShader(L"Shadow");
	if (ShadowPixelShader == nullptr)
	{
		OutputDebugString(L"EditorECS::SkeletalMeshComponent::Initialize::Failed Get Shadow Pixel Shader.");
		return false;
	}

	return true;
}

void SkeletalMeshComponent::UpdateTransformMatrix(const TransformComponent& transform)
{
	Vector3 rotation = transform.Rotation / 180.0f * PI;
	DirectX::FXMVECTOR q = DirectX::XMQuaternionRotationRollPitchYawFromVector(rotation);
	//TransformData.Mat = DirectX::XMMatrixAffineTransformation(transform.Scale, transform.Translation, q, transform.Translation);
	TransformData.Mat = DirectX::XMMatrixAffineTransformation(transform.Scale, Vector3(0.0f, 0.0f, 0.0f), q, transform.Translation);
	TransformData.InversedMat = DirectX::XMMatrixInverse(0, TransformData.Mat);
	TransformData.Mat = TransformData.Mat.Transpose();
}


bool SkeletalMeshComponent::WriteXML(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	TiXmlElement* pSkeletalMeshComponent = new TiXmlElement("SkeletalMeshComponent");
	parent->LinkEndChild(pSkeletalMeshComponent);

	if (!WriteString(pSkeletalMeshComponent, FilePath, "Path"))
	{

	}

	return true;
}

bool SkeletalMeshComponent::ReadXML(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	TiXmlElement* root = parent->FirstChildElement("SkeletalMeshComponent");

	if (!ReadString(root, "Path", FilePath))
	{
		if (!Load(FilePath))
		{
			return false;
		}
	}

	return true;
}

bool SkeletalMeshComponent::Load(std::string filename)
{
	std::ifstream skm_in(filename, std::ios::binary);
	if (skm_in.is_open())
	{
		FilePath = filename;
		int meshCnt = 0;
		skm_in.read(reinterpret_cast<char*>(&meshCnt), sizeof(int));
		Meshes.resize(meshCnt);
		for (int i = 0; i < meshCnt; i++)
		{
			int name_size;
			skm_in.read(reinterpret_cast<char*>(&name_size), sizeof(int));
			char temp[256];
			skm_in.read(reinterpret_cast<char*>(&temp[0]), name_size);
			Meshes[i].Name = to_mw(temp);

			int matname_size;
			skm_in.read(reinterpret_cast<char*>(&matname_size), sizeof(int));
			skm_in.read(reinterpret_cast<char*>(&temp[0]), matname_size);
			Meshes[i].MaterialName = to_mw(temp);

			int vertices_size;
			skm_in.read(reinterpret_cast<char*>(&vertices_size), sizeof(int));
			Meshes[i].Vertices.resize(vertices_size);
			skm_in.read(reinterpret_cast<char*>(&Meshes[i].Vertices[0]), sizeof(Vertex) * vertices_size);
			int faces_size;
			skm_in.read(reinterpret_cast<char*>(&faces_size), sizeof(int));
			if (faces_size > 0)
			{
				Meshes[i].Faces.resize(faces_size);
				skm_in.read(reinterpret_cast<char*>(&Meshes[i].Faces[0]), sizeof(Face) * faces_size);
			}

			int indices_size;
			skm_in.read(reinterpret_cast<char*>(&indices_size), sizeof(int));
			if (indices_size > 0)
			{
				Meshes[i].Indices.resize(indices_size);
				skm_in.read(reinterpret_cast<char*>(&Meshes[i].Indices[0]), sizeof(DWORD) * indices_size);
			}

			int iw_size;
			skm_in.read(reinterpret_cast<char*>(&iw_size), sizeof(int));
			if (iw_size > 0)
			{
				Meshes[i].SkinningIWList.resize(iw_size);
				skm_in.read(reinterpret_cast<char*>(&Meshes[i].SkinningIWList[0]), sizeof(IWData) * iw_size);
			}

		}
		int fbxname_size;
		skm_in.read(reinterpret_cast<char*>(&fbxname_size), sizeof(int));
		std::string temp;
		temp.resize(fbxname_size);
		skm_in.read(reinterpret_cast<char*>(&temp[0]), fbxname_size);
		FBXName = to_mw(temp);

		int meshname_size;
		skm_in.read(reinterpret_cast<char*>(&meshname_size), sizeof(int));
		Name.resize(meshname_size);
		skm_in.read(reinterpret_cast<char*>(&Name[0]), meshname_size);
		int bpmap_size;
		skm_in.read(reinterpret_cast<char*>(&bpmap_size), sizeof(int));
		for (int i = 0; i < bpmap_size; i++)
		{
			int bonename_size;
			std::string bone;
			Matrix mat;

			skm_in.read(reinterpret_cast<char*>(&bonename_size), sizeof(int));
			bone.resize(bonename_size);
			skm_in.read(reinterpret_cast<char*>(&bone[0]), bonename_size);
			skm_in.read(reinterpret_cast<char*>(&mat), sizeof(Matrix));

			///add-
			int pointer = bone.size() - 1;
			if (bone[pointer] == NULL)
			{
				bone.pop_back();
			}
			///-add

			BindPoseMap.insert(std::make_pair(bone, mat));
			BindPoseKeyToIndexMap.insert(std::make_pair(bone, i));
		}

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
			//SKM->Meshes[i].MaterialSlot = MaterialManager::GetInstance()->GetMaterial(L"Default");
			Meshes[i].Initialize();
		}
		isCreated = false;
		Initialize();

		skm_in.close();

	}

	return true;
}
