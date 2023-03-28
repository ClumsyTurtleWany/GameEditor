#include "FBXLoader.hpp"
#include "MaterialManager.h"

bool FBXLoader::Initialize()
{
	m_pManager = FbxManager::Create(); // static으로 리턴 됨.
	if (m_pManager != nullptr)
	{
		m_pImporter = FbxImporter::Create(m_pManager, ""); // 유니코드 사용 안함. static으로 리턴 됨.
		if (m_pImporter == nullptr)
		{
			OutputDebugString(L"FBXLoader::initialize::Failed Create Fbx Importer.\n");
			return false;
		}

		return true;
	}
	else
	{
		OutputDebugString(L"FBXLoader::initialize::Failed Create Fbx Manager.\n");
		return false;
	}
}

bool FBXLoader::Release()
{
	if (m_pImporter != nullptr)
	{
		m_pImporter->Destroy();
		m_pImporter = nullptr;
	}
	
	if (m_pManager != nullptr)
	{
		m_pManager->Destroy();
		m_pManager = nullptr;
	}

	for (auto& it : FbxFileList)
	{
		FBXFileData* data = it.second;
		delete data;
		it.second = nullptr;
	}
	FbxFileList.clear();

	return true;
}

bool FBXLoader::Load(std::wstring filename)
{
	auto it = FbxFileList.find(filename);
	if (it != FbxFileList.end())
	{
		return true;
	}

	std::string strPath;
	strPath.assign(filename.begin(), filename.end());
	const char* path = strPath.c_str();

	if (!m_pImporter->Initialize(path))
	{
		OutputDebugString(L"FBXLoader::Load::Failed Initialize Importer.\n");
		return false;
	}

	// 기본적으로 디자인 파일들은 Scene 단위로 저장됨.
	// Scene은 트리 구조로 이루어져 있어 Root부터 시작되며 보통 Root는 NULL로 이루어져있음.
	// 따라서 파일 마다 새로 생성해서 로드 해줄 필요가 있음.

	FbxScene* pScene = FbxScene::Create(m_pManager, "");
	if (pScene == nullptr)
	{
		OutputDebugString(L"FBXLoader::Load::Failed Create Scene.\n");
		return false;
	}

	if (!m_pImporter->Import(pScene))
	{
		OutputDebugString(L"FBXLoader::Load::Failed Import Scene.\n");
		return false;
	}

	FBXFileData* fbxFile = new FBXFileData;
	std::filesystem::path filepath(filename);
	fbxFile->FilePath = filepath.parent_path();
	fbxFile->FilePath += L"/";
	if (!ParseScene(pScene, fbxFile))
	{
		OutputDebugString(L"FBXLoader::Load::Failed Parse Scene.\n");
		return false;
	}

	FbxNode* pRoot = pScene->GetRootNode();
	if (!ParseNode(pRoot, fbxFile))
	{
		OutputDebugString(L"FBXLoader::Load::Failed Parse Root Node.\n");
		return false;
	}

	if (!PreProcess(fbxFile))
	{
		OutputDebugString(L"FBXLoader::Load::Failed Pre Process.\n");
		return false;
	}

	FbxFileList.insert(std::make_pair(filename, fbxFile));

	pRoot->Destroy();
	pScene->Destroy();

	return true;
}

bool FBXLoader::GetFBXFileList(std::wstring _path, std::vector<std::wstring>& _dst)
{
	std::filesystem::path path(_path);
	for (auto& file : std::filesystem::directory_iterator(path))
	{
		std::wstring filename = file.path().filename();
		std::wstring filepath = file.path();
		std::wstring fileExtension = file.path().extension();

		if ((fileExtension == L".FBX") || (fileExtension == L".fbx"))
		{
			_dst.push_back(filename);
		}
		else
		{
			continue;
		}
	}

	return true;
}

bool FBXLoader::ParseScene(FbxScene* scene, FBXFileData* dst)
{
	if ((scene == nullptr) || (dst == nullptr))
	{
		return false;
	}

	// 각 FBX를 만드는 툴(3D Max, 마야 등 디자인 툴)마다 기저 축과 단위가 다를 수 있기 때문에 설정 후 로딩 할 수 있다.
	// 보통의 FBX는 기본적으로 오른손 좌표계, Y-Up
	//FbxSystemUnit::cm.ConvertScene(_scene); // 단위 변경. cm 단위로 변경
	FbxSystemUnit::m.ConvertScene(scene); // 단위 변경. m 단위로 변경
	//FbxAxisSystem::MayaZUp.ConvertScene(pScene); // 기저 축 변경. 마야 기준으로 변경. 정점 변환이 아닌 행렬에 적용 되는 것임.
	FbxAxisSystem sceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();

	//FbxAxisSystem::DirectX.ConvertScene(pScene); // 기저 축 변경. DirectX 기준으로 바꿔도 실제 우리가 사용하는 축과 다름. (Right Vector가 마이너스임.).
	//FbxAxisSystem sceneAxisSystemAfter = pScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem::MayaZUp.ConvertScene(scene); // 기저 축 변경. 마야 기준으로 변경. 정점 변환이 아닌 행렬에 적용 되는 것임.


	// Animation Scene Information
	FbxAnimStack* pStack = scene->GetSrcObject<FbxAnimStack>(0);
	FbxLongLong s = 0;
	FbxLongLong n = 0;
	FbxTime::EMode timeMode = FbxTime::GetGlobalTimeMode();
	if (pStack != nullptr)
	{
		FbxTime::SetGlobalTimeMode(FbxTime::eFrames30);
		//FbxTime::EMode TimeMode = FbxTime::GetGlobalTimeMode();
		FbxTimeSpan localTimeSpan = pStack->GetLocalTimeSpan();// 시간 간격. 프레임 사이
		FbxTime start = localTimeSpan.GetStart();
		FbxTime end = localTimeSpan.GetStop();
		FbxTime duration = localTimeSpan.GetDirection();
		s = start.GetFrameCount(timeMode);
		n = end.GetFrameCount(timeMode);
	}

	dst->AnimationSceneInfo.TimeMode = timeMode;
	dst->AnimationSceneInfo.StartFrame = s;
	dst->AnimationSceneInfo.EndFrame = n;

	return true;
}

bool FBXLoader::ParseNode(FbxNode* node, FBXFileData* dst)
{
	if ((node == nullptr) || (dst == nullptr))
	{
		OutputDebugString(L"FBXLoader::ParseNode::Node or Dest is nullptr.\n");
		return false;
	}

	bool isValid = false;

	FBXNodeData NodeData;
	NodeData.Name = node->GetName();
	FbxNodeAttribute* pAttribute = node->GetNodeAttribute();
	if (pAttribute != nullptr)
	{
		// Attribute Type
		// eUnknown,
		// eNull, // 더미 오브젝트로 이용 될 수도 있음.
		// eMarker,
		// eSkeleton, // 애니메이션용 데이터.
		// eMesh, // 랜더링용 데이터.
		// eNurbs,
		// ePatch,
		// eCamera,
		// eCameraStereo,
		// eCameraSwitcher,
		// eLight,
		// eOpticalReference,
		// eOpticalMarker,
		// eNurbsCurve,
		// eTrimNurbsSurface,
		// eBoundary,
		// eNurbsSurface,
		// eShape,
		// eLODGroup,
		// eSubDiv,
		// eCachedEffect,
		// eLine

		//FbxNodeAttribute::EType attributeType = pAttribute->GetAttributeType();
		NodeData.AttributeType = pAttribute->GetAttributeType();
		switch (NodeData.AttributeType)
		{
			case FbxNodeAttribute::EType::eNull: // 보통 Root 노드는 NULL을 가짐.
			{
				isValid = true;
				//FbxNull* pDummy = _node->GetNull();
				NodeData.Dummy = node->GetNull();
				if (NodeData.Dummy != nullptr)
				{
					// Dummy: 자식 오브젝트의 원점과 부모 오브젝트의 원점을 맞춰주기 위한 정보.
					//ParseDummy(pDummy, _dst);
					dst->DummyList.push_back(NodeData.Dummy);
				}
				break;
			}

			case FbxNodeAttribute::EType::eSkeleton:
			{
				isValid = true;
				//FbxSkeleton* pSkeleton = _node->GetSkeleton();
				NodeData.Skeleton = node->GetSkeleton();
				if (NodeData.Skeleton != nullptr)
				{
					// Skeleton: 애니메이션을 위한 정보
					//ParseSkeleton(pSkeleton, _dst);
					dst->SkeletonList.push_back(NodeData.Skeleton);
				}
				break;
			}

			case FbxNodeAttribute::EType::eMesh:
			{
				isValid = true;
				//FbxMesh* pMesh = _node->GetMesh();
				NodeData.FMesh = node->GetMesh();
				if (NodeData.FMesh != nullptr)
				{
					// Mesh: 랜더 가능한 데이터
					// Scene graph 형식(트리에 모든 정보를 다 넣어서 저장 후 사용 및 랜더링하는 방식) 이라고 부름.
					//ParseMesh(pMesh, _dst);
					dst->MeshList.push_back(NodeData.FMesh);
				}
				break;
			}
		}
	}

	if (isValid)
	{
		dst->NodeList.push_back(node);
		dst->NodeNameList.push_back(NodeData.Name);

		// 로컬 행렬
		// Affine 행렬은 정점 변환 할 때, 그냥 행렬은 행렬 끼리 연산 할 때 사용
		NodeData.LocalGeometryMatrix = GetGeometryMatrix(node); // 기하 행렬. 초기 정점 위치를 변환 할 때 사용. 로컬 행렬. 이 매트릭스를 적용해 바뀌지 않으면 월드 행렬에서 변환. 정점 변환.
		dst->LocalGeometryMatrixMap.insert(std::make_pair(NodeData.Name, NodeData.LocalGeometryMatrix));

		// 노말 성분은 역행렬의 전치 행렬로 곱해 줘야 함.
		NodeData.LocalNormalMatrix = GetNormalMatrix(NodeData.LocalGeometryMatrix);
		dst->NormalMatrixMap.insert(std::make_pair(NodeData.Name, NodeData.LocalGeometryMatrix));


		// 각 페이스 별로 다른 텍스쳐를 사용 할 수 있다. 이것을 서브 머테리얼 이라고 함. (1개의 오브젝트에 여러개의 텍스쳐 사용)
		// 서브 머테리얼을 렌더링 하기 위해선 같은 텍스쳐를 사용하는 페이스들을 묶어서 출력.
		NodeData.MaterialNum = node->GetMaterialCount(); // 텍스쳐가 붙은 갯수.
		for (int idx = 0; idx < NodeData.MaterialNum; idx++)
		{
			FbxSurfaceMaterial* surface = node->GetMaterial(idx);
			std::wstring materialName;
			if (idx >= 1)
			{
				std::wstring subMaterial = std::to_wstring(idx);
				materialName += subMaterial;
			}
			materialName.assign(NodeData.Name.begin(), NodeData.Name.end());
			Material* material = MaterialManager::GetInstance()->CreateMaterial(materialName);
			bool isValid = false;
			material->Type = MaterialType::Light;
			material->DiffuseTextureName = GetTextureFileName(surface, FbxSurfaceMaterial::sDiffuse);
			material->NormalTextureName = GetTextureFileName(surface, FbxSurfaceMaterial::sNormalMap);
			material->AmbientTextureName = GetTextureFileName(surface, FbxSurfaceMaterial::sAmbient);
			material->SpecularTextureName = GetTextureFileName(surface, FbxSurfaceMaterial::sSpecular);
			material->EmissiveTextureName = GetTextureFileName(surface, FbxSurfaceMaterial::sEmissive);
			if (!material->DiffuseTextureName.empty())
			{
				material->DiffuseTextureName = dst->FilePath + material->DiffuseTextureName;
			}

			if (!material->NormalTextureName.empty())
			{
				material->NormalTextureName = dst->FilePath + material->NormalTextureName;
			}

			if (!material->AmbientTextureName.empty())
			{
				material->AmbientTextureName = dst->FilePath + material->AmbientTextureName;
			}

			if (!material->SpecularTextureName.empty())
			{
				material->SpecularTextureName = dst->FilePath + material->SpecularTextureName;
			}

			if (!material->EmissiveTextureName.empty())
			{
				material->EmissiveTextureName = dst->FilePath + material->EmissiveTextureName;
			}

			if (!material->Create())
			{
				material = MaterialManager::GetInstance()->GetMaterial(L"Default");
			}
			NodeData.MaterialList.push_back(material);
		}

		dst->NodeDataList.push_back(NodeData);
	}

	int childCount = node->GetChildCount(); // Child 갯수가 0이면 정적 매쉬, 0이 아니면 동적 매쉬로 볼 수 있음.
	for (int idx = 0; idx < childCount; idx++)
	{
		FbxNode* pChild = node->GetChild(idx);
		if (isValid |= ParseNode(pChild, dst))
		{
		}
		else
		{
		}
	}
	
	return isValid;
}

bool FBXLoader::PreProcess(FBXFileData* dst)
{
	if (!GenerateAnimationTrack(dst, 100.0f))
	{

	}

	for (auto &it : dst->NodeDataList)
	{
		if (!ParseDummy(it.Dummy, dst))
		{

		}

		if (!ParseSkeleton(it.Skeleton, dst))
		{

		}

		if (!ParseMesh(it.FMesh, dst, &it))
		{

		}
	}

	return true;
}

bool FBXLoader::ParseMesh(FbxMesh* mesh, FBXFileData* dst, FBXNodeData* dstData)
{
	if ((mesh == nullptr) || (dst == nullptr))
	{
		return false;
	}

	size_t materialSize = dstData->MaterialList.size();
	if (materialSize > 0)
	{
		dstData->MeshList.resize(materialSize);
	}
	else
	{
		dstData->MeshList.resize(1);
	}

	if (ParseMeshSkinning(mesh, dst , dstData))
	{
		
	}

	// Layer 개념 필요. 여러번에 걸쳐 동일한 곳에 랜더링 하는것 == 멀티 패스 랜더링. 텍스쳐로 치환하면 멀티 텍스처 랜더링.
	std::vector<FbxLayerElementUV*> uvList;
	std::vector<FbxLayerElementVertexColor*> colorList;
	std::vector<FbxLayerElementMaterial*> materialList;
	std::vector<FbxLayerElementNormal*> normalList;
	std::vector<FbxLayerElementTangent*> tangentList;
	
	int layerCount = mesh->GetLayerCount();
	for (int layerIdx = 0; layerIdx < layerCount; layerIdx++)
	{
		FbxLayer* pLayer = mesh->GetLayer(layerIdx);
		FbxLayerElementUV* pUV = pLayer->GetUVs();
		if (pUV != nullptr)
		{
			uvList.push_back(pUV);
		}

		FbxLayerElementVertexColor* pColor = pLayer->GetVertexColors();
		if (pColor != nullptr)
		{
			colorList.push_back(pColor);
		}

		FbxLayerElementMaterial* pMaterial = pLayer->GetMaterials();
		if (pMaterial != nullptr)
		{
			materialList.push_back(pMaterial);
		}

		FbxLayerElementNormal* pNormal = pLayer->GetNormals();
		if (pNormal != nullptr)
		{
			normalList.push_back(pNormal);
		}

		FbxLayerElementTangent* pTangent = pLayer->GetTangents();
		if (pTangent != nullptr)
		{
			tangentList.push_back(pTangent);
		}
	}

	size_t LayerIdx = 0;
	int polyCount = mesh->GetPolygonCount();
	// 3정점 = 1폴리곤(Triangle) 일 수도 있고
	// 4정점 = 1폴리곤(Quad) 일 수도 있다.
	// 폴리곤 -> 페이스 -> 정점
	int basePolyIdx = 0; // Color
	int MaterialIdx = 0;
	FbxVector4* pVertexPosition = mesh->GetControlPoints(); // 제어점(Control point == Vertices). 정점의 시작 위치. 
	for (int polyIdx = 0; polyIdx < polyCount; polyIdx++)
	{
		int polySize = mesh->GetPolygonSize(polyIdx);
		int faceCount = polySize - 2;
		if (!materialList.empty())
		{
			MaterialIdx = GetSubMaterialIndex(materialList[LayerIdx], polyIdx);
		}

		for (int faceIdx = 0; faceIdx < faceCount; faceIdx++)
		{
			// FBX는 시계 반대 방향임. DirectX와 좌표계 방향이 다르다. 
			// Max 좌표는 Y축이 Z축, Z축이 Y축, (DX.x == Max.x, DX.y = Max.z, DX.z = Max.y)
			// DX 기저 		Max 기저
			// 1 0 0 0		1 0 0 0
			// 0 1 0 0		0 0 1 0
			// 0 0 1 0		0 1 0 0
			// 0 0 0 1		0 0 0 1
			////////////////////////////
			// Texture
			// DX			Max
			// (0,0) (1,0)  (0,1) (1,1)
			// 
			// (0,1) (1,1)  (0,0) (1,0)
			/////////////////////////////
			int cornerIdx[3];
			cornerIdx[0] = mesh->GetPolygonVertex(polyIdx, 0);
			cornerIdx[1] = mesh->GetPolygonVertex(polyIdx, faceIdx + 2);
			cornerIdx[2] = mesh->GetPolygonVertex(polyIdx, faceIdx + 1);

			int UVidx[3] = { 0, };
			UVidx[0] = mesh->GetTextureUVIndex(polyIdx, 0);
			UVidx[1] = mesh->GetTextureUVIndex(polyIdx, faceIdx + 2);
			UVidx[2] = mesh->GetTextureUVIndex(polyIdx, faceIdx + 1);

			int vertexColorIdx[3] = { 0, faceIdx + 2, faceIdx + 1 };
			for (int idx = 0; idx < 3; idx++)
			{
				int vertexIdx = cornerIdx[idx];
				FbxVector4 vertex = pVertexPosition[vertexIdx];
				vertex = dstData->LocalGeometryMatrix.MultT(vertex); // 로컬 행렬 변환, Transform의 T, 열 우선 방식

				Vector3 pos;
				pos.x = vertex.mData[0];
				pos.y = vertex.mData[2];
				pos.z = vertex.mData[1];

				Vector3 normal;
				Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
				Vector2 texture;
				Vector3 tangent;
				FbxVector2 tex; // = ;
				int uvidx = UVidx[idx];
				if (!uvList.empty())
				{
					if (ReadTextureCoord(uvList[LayerIdx], vertexIdx, uvidx, tex))
					{
						texture.x = tex.mData[0];
						texture.y = 1.0f - tex.mData[1];
					}
				}

				if (!colorList.empty())
				{
					int colorIdx = basePolyIdx + vertexColorIdx[idx];
					FbxColor fbxColor;
					if (ReadColorCoord(colorList[LayerIdx], vertexIdx, colorIdx, fbxColor))
					{
						color.x = fbxColor.mRed;
						color.y = fbxColor.mGreen;
						color.z = fbxColor.mBlue;
						//color.w = fbxColor.mAlpha;
						color.w = 1.0f;
					}
				}

				if (!normalList.empty())
				{
					int normalIdx = basePolyIdx + vertexColorIdx[idx];
					FbxVector4 fbxNormal;
					if (ReadNormal(normalList[LayerIdx], vertexIdx, normalIdx, fbxNormal))
					{
						fbxNormal = dstData->LocalNormalMatrix.MultT(fbxNormal);
						//fbxNormal = normalMatrix_World.MultT(fbxNormal); // 월드 변환. 나중에 뺄 것.
						//fbxNormal = matNormalGlobal.MultT(fbxNormal);
						normal.x = fbxNormal.mData[0];
						normal.y = fbxNormal.mData[2];
						normal.z = fbxNormal.mData[1];
					}
				}

				if (!tangentList.empty())
				{
					int tangentIdx = basePolyIdx + vertexColorIdx[idx];
					FbxVector4 fbxTangent;
					if (ReadTangent(tangentList[LayerIdx], vertexIdx, tangentIdx, fbxTangent))
					{
						tangent.x = fbxTangent.mData[0];
						tangent.y = fbxTangent.mData[2];
						tangent.z = fbxTangent.mData[1];
					}
				}

				//VertexList[MaterialIdx].push_back(Vertex(pos, normal, color, texture));
				if (dstData->BindPoseMap.empty())
				{
					//_dstData->Materials[MaterialIdx].push_back(Vertex(pos, normal, color, texture));
					dstData->MeshList[MaterialIdx].Vertices.push_back(Vertex(pos, normal, color, texture, tangent));
				}
				else
				{
					SkinningData skinData = dstData->SkinningList[vertexIdx];
					std::vector<SkinWeight> SkinWeightList;
					for (auto it = skinData.SkinWeightList.begin(); it != skinData.SkinWeightList.end(); it++)
					{
						SkinWeightList.push_back(*it);
					}

					IWData IWDatas;
					for (int SkinWeightIdx = 0; SkinWeightIdx < SkinWeightList.size(); SkinWeightIdx++)
					{
						if (SkinWeightIdx < 4)
						{
							if (SkinWeightIdx == 0)
							{
								IWDatas.Index.x = dstData->BindPoseKeyToIndexMap.find(SkinWeightList[SkinWeightIdx].BoneName)->second;
								IWDatas.Weight.x = SkinWeightList[SkinWeightIdx].weight;
							}
							if (SkinWeightIdx == 1)
							{
								IWDatas.Index.y = dstData->BindPoseKeyToIndexMap.find(SkinWeightList[SkinWeightIdx].BoneName)->second;
								IWDatas.Weight.y = SkinWeightList[SkinWeightIdx].weight;
							}
							if (SkinWeightIdx == 2)
							{
								IWDatas.Index.z = dstData->BindPoseKeyToIndexMap.find(SkinWeightList[SkinWeightIdx].BoneName)->second;
								IWDatas.Weight.z = SkinWeightList[SkinWeightIdx].weight;
							}
							if (SkinWeightIdx == 3)
							{
								IWDatas.Index.w = dstData->BindPoseKeyToIndexMap.find(SkinWeightList[SkinWeightIdx].BoneName)->second;
								IWDatas.Weight.w = SkinWeightList[SkinWeightIdx].weight;
							}
							
						}
						else
						{
							break;
						}
					}

					dstData->MeshList[MaterialIdx].Vertices.push_back(Vertex(pos, normal, color, texture, tangent));
					dstData->MeshList[MaterialIdx].SkinningIWList.push_back(IWDatas);
					//_dstData->Materials[MaterialIdx].push_back(Vertex(pos, normal, color, texture), IWDatas);
				}
			}


		}

		basePolyIdx += polySize;
	}

	return true;
}

//bool FBXLoader::ParseMeshLayer(FbxMesh* mesh, MeshData* dstData)
//{
//	if (mesh == nullptr || dstData == nullptr)
//	{
//		return false;
//	}
//
//	// Layer 개념 필요. 여러번에 걸쳐 동일한 곳에 랜더링 하는것 == 멀티 패스 랜더링. 텍스쳐로 치환하면 멀티 텍스처 랜더링.
//	std::vector<FbxLayerElementUV*> UVList;
//	std::vector<FbxLayerElementVertexColor*> ColorList;
//	std::vector<FbxLayerElementMaterial*> MaterialList;
//	std::vector<FbxLayerElementNormal*> NormalList;
//
//	int layerCount = mesh->GetLayerCount();
//	dstData->LayerList.resize(layerCount);
//	for (int layerIdx = 0; layerIdx < layerCount; layerIdx++)
//	{
//		FbxLayer* pLayer = mesh->GetLayer(layerIdx);
//		FbxLayerElementUV* pUV = pLayer->GetUVs();
//		if (pUV != nullptr)
//		{
//			UVList.push_back(pUV);
//		}
//
//		FbxLayerElementVertexColor* pColor = pLayer->GetVertexColors();
//		if (pColor != nullptr)
//		{
//			ColorList.push_back(pColor);
//		}
//
//		FbxLayerElementMaterial* pMaterial = pLayer->GetMaterials();
//		if (pMaterial != nullptr)
//		{
//			MaterialList.push_back(pMaterial);
//		}
//
//		FbxLayerElementNormal* pNormal = pLayer->GetNormals();
//		if (pNormal != nullptr)
//		{
//			NormalList.push_back(pNormal);
//		}
//
//		dstData->LayerList[layerIdx].ElementUV = pUV;
//		dstData->LayerList[layerIdx].ElementColor = pColor;
//		dstData->LayerList[layerIdx].ElementMaterialList = pMaterial;
//		dstData->LayerList[layerIdx].ElementNormalList = pNormal;
//	}
//
//	return true;
//}

bool FBXLoader::ParseMeshSkinning(FbxMesh* mesh, FBXFileData* dst, FBXNodeData* dstData)
{
	if ((mesh == nullptr) || (dst == nullptr))
	{
		return false;
	}

	// Skinning
	// VertexCnt == 메쉬의 정점 개수와 동일해야 한다.
	int VertexCnt = mesh->GetControlPointsCount();
	dstData->SkinningList.resize(VertexCnt);

	// Skinning: 정점에 영향을 주는 행렬들을 찾는 과정.
	int DeformerCnt = mesh->GetDeformerCount(FbxDeformer::EDeformerType::eSkin);
	for (int idx = 0; idx < DeformerCnt; idx++)
	{
		// Deformer(리깅 도구): 뼈대에 스킨을 붙이는 작업 도구.
		FbxDeformer* pDeformer = mesh->GetDeformer(idx, FbxDeformer::EDeformerType::eSkin);
		FbxSkin* pSkin = reinterpret_cast<FbxSkin*>(pDeformer);

		FbxGeometry* pGeometry = pSkin->GetGeometry();
		int ClusterCnt = pSkin->GetClusterCount(); // ClusterCnt 개의 Bone이 VertexCnt 수만큼의 정점에 영향을 준다.
		for (int clusterIdx = 0; clusterIdx < ClusterCnt; clusterIdx++)
		{
			// Cluster: 뼈대에 붙이는 덩어리(구, Sphere), Mesh의 정점 덩어리. 뼈대가 영향을 미치는 정점 덩어리.
			FbxCluster* pCluster = pSkin->GetCluster(clusterIdx);

			FbxCluster::ELinkMode LinkMode = pCluster->GetLinkMode();
			FbxNode* pLinkNode = pCluster->GetLink(); // Bone으로 보면 됨.
			std::string LinkedNodeName = pLinkNode->GetName();

			FbxNodeAttribute::EType attributeType;
			FbxNodeAttribute* pAttribute = pLinkNode->GetNodeAttribute();
			if (pAttribute != nullptr)
			{
				attributeType = pAttribute->GetAttributeType();
			}

			int BoneIndex = 0;// = pLinkNode 의 Bone 데이터; // 오브젝트에서 찾아서 넣어줘야 함.

			// 어떤 정점이 영향을 받는지 체크
			int* indices = pCluster->GetControlPointIndices();
			double* weights = pCluster->GetControlPointWeights();
			int clusterSize = pCluster->GetControlPointIndicesCount(); // 하나의 뼈대(BoneIndex의 행렬)가 영향을 주는 Vertex 개 수.
			for (int vertexIdx = 0; vertexIdx < clusterSize; vertexIdx++)
			{
				// 각 정점마다 저장해 주어야 함. 지금은 4개를 넘기도록 했기 때문에 Weight가 큰 순으로 사용 해 주어야 함.
				// 현재 뼈대가 영향을 주는 정점과 가중치. VertexIndex의 정점에 Weight만큼 BoneIndex 행렬이 영향을 준다.
				int VertexIndex = indices[vertexIdx];
				float Weight = static_cast<float>(weights[vertexIdx]);

				//_dst->SkinningList[VertexIndex].setKey(LinkedNodeName);
				//_dst->SkinningList[VertexIndex].insert(BoneIndex, Weight);
				dstData->SkinningList[VertexIndex].insert(Weight, LinkedNodeName);
			}

			// 뼈대 공간으로 변환하는 행렬이 필요하다.
			// Mesh의 정점들은 월드 행렬 공간에 있기 때문에(애니메이션) 
			// Skin Mesh가 Bone을 이탈하지 않게 하기 위해서는 Bone공간으로 변환이 필요함.
			FbxAMatrix matXBindPose;
			FbxAMatrix matRefGlobalInitPosition;
			pCluster->GetTransformLinkMatrix(matXBindPose); // Dress Pose == Bind Pose가 만들어 짐.(Animation Matrix)
			pCluster->GetTransformMatrix(matRefGlobalInitPosition); // Offset Matrix, 부모 자식 뼈대가 붙어 다녀야 하기 때문에 Pivot(Dummy, offset)이 정리되어야 한다 == 이동이 꼭 필요하다.  
			FbxAMatrix matInversedBindPose = matRefGlobalInitPosition.Inverse() * matXBindPose; // 전역 행렬이 곱해진 후 바인드 포즈가 곱해진 것이 애니메이션
			matInversedBindPose = matInversedBindPose.Inverse(); // 따라서 역행렬로 만들어 주어서 정점에 곱해 주어야 뼈대의 로컬 행렬로 변환 가능하다.
			Matrix matInvBindPose = ConvertToDxMatrix(matInversedBindPose); // 정점과 곱한 후 VertexBuffer에 넣고 월드 행렬과 곱해줘도 되고, VertexBuffer는 냅두고 월드 행렬 앞에 곱해도 됨.
			//_dst->BindPoseMap.insert(std::make_pair(BoneIndex, matInvBindPose)); // 상수 버퍼 적용 전에 곱해 주고
			dstData->BindPoseMap.insert(std::make_pair(LinkedNodeName, matInvBindPose)); // 상수 버퍼 적용 전에 곱해 주고
		}
		
	}

	int BindPoseIdx = 0;
	for (auto it : dstData->BindPoseMap)
	{
		dstData->BindPoseKeyToIndexMap.insert(std::make_pair(it.first, BindPoseIdx++));
	}

	return true;
}

bool FBXLoader::ParseDummy(FbxNull* dummy, FBXFileData* dst)
{
	if ((dummy == nullptr) || (dst == nullptr))
	{
		return false;
	}

	dst->DummyList.push_back(dummy);

	return true;
}

bool FBXLoader::ParseSkeleton(FbxSkeleton* skeleton, FBXFileData* dst)
{
	if ((skeleton == nullptr) || (dst == nullptr))
	{
		return false;
	}

	dst->SkeletonList.push_back(skeleton);

	return true;
}

bool FBXLoader::ReadTextureCoord(FbxLayerElementUV* uv, int vertexIdx, int uvIdx, FbxVector2& dst)
{
	if (uv == nullptr || dst == nullptr)
	{
		return false;
	}

	// 텍스처 맵핑 방식은 여러가지 있음.
	// EMappingMode
	// eNone: 맵핑이 결정되지 않았다.
	// eByControlPoint: 제어점마다 1개의 맵핑이 되어 있다. (각 정점이 UV좌표를 들고 있음.)
	// eByPolygonVertex: 각 정점이 속한 폴리곤에 맵핑에 좌표가 있다. (폴리곤에서 탐색 해야함)
	// eByPolygon: 전체 폴리곤에 맵핑 좌표가 있다. 
	// eByEdge: 엣지에 1개의 맵핑 좌표가 있다.
	// eAllSame: 전체 폴리곤에 1개의 맵핑 좌표가 있다.
	// 맵핑 방식 -> 저장 방식 확인 필요. == Reference Mode
	// eDirect: 리스트에 저장되어 있음. N번째 맵핑 정보가 DirectArray의 N번째 위치에 있다.
	// eIndex: FBX 5.0 이하 버전에서 사용되었음. 지금은 사용하지 않음. 그 이상의 버전에서는 eIndexToDirect로 대체됨.
	// eIndexToDirect: 인덱스에 저장되어 있음. 
	switch (uv->GetMappingMode())
	{
		case FbxLayerElementUV::eByControlPoint:
		{
			// 정점 마다 UV가 1개씩 있으니 해당 Vertex에서 정보를 가져옴.
			switch (uv->GetReferenceMode())
			{
				case FbxLayerElementUV::eDirect:
				{
					FbxVector2 fbxUv = uv->GetDirectArray().GetAt(vertexIdx); 
					dst.mData[0] = fbxUv.mData[0];
					dst.mData[1] = fbxUv.mData[1];
					break;
				}
				case FbxLayerElementUV::eIndexToDirect:
				{
					int id = uv->GetIndexArray().GetAt(vertexIdx);
					FbxVector2 fbxUv = uv->GetDirectArray().GetAt(id);
					dst.mData[0] = fbxUv.mData[0];
					dst.mData[1] = fbxUv.mData[1];
					break;
				}
			}
			break;
		}
		case FbxLayerElementUV::eByPolygonVertex:
		{
			// 폴리곤에 하나 있으니 다이렉트로 가져와도 된다.
			switch (uv->GetReferenceMode())
			{
				case FbxLayerElementUV::eDirect:
				//{
				//	FbxVector2 fbxUv = _uv->GetDirectArray().GetAt(_uvIdx);
				//	_dst.mData[0] = fbxUv.mData[0];
				//	_dst.mData[1] = fbxUv.mData[1];
				//	break;
				//}
				case FbxLayerElementUV::eIndexToDirect:
				{
					//int id = _uv->GetIndexArray().GetAt(_uvIdx);
					//FbxVector2 fbxUv = _uv->GetDirectArray().GetAt(id);
					//_dst.mData[0] = fbxUv.mData[0];
					//_dst.mData[1] = fbxUv.mData[1];

					FbxVector2 fbxUv = uv->GetDirectArray().GetAt(uvIdx); // 다이렉트로 가져오면 됨.
					dst.mData[0] = fbxUv.mData[0];
					dst.mData[1] = fbxUv.mData[1];
					break;
				}
			}
			break;
		}
		
	}

	return true;
}

bool FBXLoader::ReadColorCoord(FbxLayerElementVertexColor* color, int vertexIdx, int colorIdx, FbxColor& dst)
{
	if (color == nullptr)
	{
		return false;
	}

	// 텍스처 맵핑 방식은 여러가지 있음.
	// EMappingMode
	// eNone: 맵핑이 결정되지 않았다.
	// eByControlPoint: 제어점마다 1개의 맵핑이 되어 있다. (각 정점이 UV좌표를 들고 있음.)
	// eByPolygonVertex: 각 정점이 속한 폴리곤에 맵핑에 좌표가 있다. (폴리곤에서 탐색 해야함)
	// eByPolygon: 전체 폴리곤에 맵핑 좌표가 있다. 
	// eByEdge: 엣지에 1개의 맵핑 좌표가 있다.
	// eAllSame: 전체 폴리곤에 1개의 맵핑 좌표가 있다.
	// 맵핑 방식 -> 저장 방식 확인 필요. == Reference Mode
	// eDirect: 리스트에 저장되어 있음. N번째 맵핑 정보가 DirectArray의 N번째 위치에 있다.
	// eIndex: FBX 5.0 이하 버전에서 사용되었음. 지금은 사용하지 않음. 그 이상의 버전에서는 eIndexToDirect로 대체됨.
	// eIndexToDirect: 인덱스에 저장되어 있음. 
	switch (color->GetMappingMode())
	{
	case FbxLayerElementUV::eByControlPoint:
	{
		// 정점 마다 UV가 1개씩 있으니 해당 Vertex에서 정보를 가져옴.
		switch (color->GetReferenceMode())
		{
		case FbxLayerElementUV::eDirect:
		{
			dst = color->GetDirectArray().GetAt(vertexIdx);
			break;
		}
		case FbxLayerElementUV::eIndexToDirect:
		{
			int id = color->GetIndexArray().GetAt(vertexIdx);
			dst = color->GetDirectArray().GetAt(id);
			break;
		}
		}
		break;
	}
	case FbxLayerElementUV::eByPolygonVertex:
	{
		// 폴리곤에 하나 있으니 다이렉트로 가져와도 된다.
		switch (color->GetReferenceMode())
		{
		case FbxLayerElementUV::eDirect:
			//{
			//	FbxVector2 fbxUv = _uv->GetDirectArray().GetAt(_uvIdx);
			//	_dst.mData[0] = fbxUv.mData[0];
			//	_dst.mData[1] = fbxUv.mData[1];
			//	break;
			//}
		{
			dst = color->GetDirectArray().GetAt(colorIdx); // 다이렉트로 가져오면 됨.
			break;
		}
		case FbxLayerElementUV::eIndexToDirect:
		{
			//int id = _uv->GetIndexArray().GetAt(_uvIdx);
			//FbxVector2 fbxUv = _uv->GetDirectArray().GetAt(id);
			//_dst.mData[0] = fbxUv.mData[0];
			//_dst.mData[1] = fbxUv.mData[1];

			//_dst = _color->GetDirectArray().GetAt(_colorIdx); // 다이렉트로 가져오면 됨.
			int id = color->GetIndexArray().GetAt(colorIdx);
			dst = color->GetDirectArray().GetAt(id);
			break;
		}
		}
		break;
	}

	}

	return true;
}

bool FBXLoader::ReadNormal(FbxLayerElementNormal* normal, int vertexIdx, int normalIdx, FbxVector4& dst)
{
	if (normal == nullptr)
	{
		return false;
	}

	// 텍스처 맵핑 방식은 여러가지 있음.
	// EMappingMode
	// eNone: 맵핑이 결정되지 않았다.
	// eByControlPoint: 제어점마다 1개의 맵핑이 되어 있다. (각 정점이 UV좌표를 들고 있음.)
	// eByPolygonVertex: 각 정점이 속한 폴리곤에 맵핑에 좌표가 있다. (폴리곤에서 탐색 해야함)
	// eByPolygon: 전체 폴리곤에 맵핑 좌표가 있다. 
	// eByEdge: 엣지에 1개의 맵핑 좌표가 있다.
	// eAllSame: 전체 폴리곤에 1개의 맵핑 좌표가 있다.
	// 맵핑 방식 -> 저장 방식 확인 필요. == Reference Mode
	// eDirect: 리스트에 저장되어 있음. N번째 맵핑 정보가 DirectArray의 N번째 위치에 있다.
	// eIndex: FBX 5.0 이하 버전에서 사용되었음. 지금은 사용하지 않음. 그 이상의 버전에서는 eIndexToDirect로 대체됨.
	// eIndexToDirect: 인덱스에 저장되어 있음. 
	switch (normal->GetMappingMode())
	{
	case FbxLayerElementUV::eByControlPoint:
	{
		// 정점 마다 UV가 1개씩 있으니 해당 Vertex에서 정보를 가져옴.
		switch (normal->GetReferenceMode())
		{
		case FbxLayerElementUV::eDirect:
		{
			dst = normal->GetDirectArray().GetAt(vertexIdx);
			break;
		}
		case FbxLayerElementUV::eIndexToDirect:
		{
			int id = normal->GetIndexArray().GetAt(vertexIdx);
			dst = normal->GetDirectArray().GetAt(id);
			break;
		}
		}
		break;
	}
	case FbxLayerElementUV::eByPolygonVertex:
	{
		// 폴리곤에 하나 있으니 다이렉트로 가져와도 된다.
		switch (normal->GetReferenceMode())
		{
		case FbxLayerElementUV::eDirect:
			//{
			//	FbxVector2 fbxUv = _uv->GetDirectArray().GetAt(_uvIdx);
			//	_dst.mData[0] = fbxUv.mData[0];
			//	_dst.mData[1] = fbxUv.mData[1];
			//	break;
			//}
		{
			dst = normal->GetDirectArray().GetAt(normalIdx); // 다이렉트로 가져오면 됨.
			break;
		}
		case FbxLayerElementUV::eIndexToDirect:
		{
			//int id = _uv->GetIndexArray().GetAt(_uvIdx);
			//FbxVector2 fbxUv = _uv->GetDirectArray().GetAt(id);
			//_dst.mData[0] = fbxUv.mData[0];
			//_dst.mData[1] = fbxUv.mData[1];

			//_dst = _color->GetDirectArray().GetAt(_colorIdx); // 다이렉트로 가져오면 됨.
			int id = normal->GetIndexArray().GetAt(normalIdx);
			dst = normal->GetDirectArray().GetAt(id);
			break;
		}
		}
		break;
	}

	}

	return true;
}

bool FBXLoader::ReadTangent(FbxLayerElementTangent* tangent, int vertexIdx, int tangentIdx, FbxVector4& dst)
{
	if (tangent == nullptr)
	{
		return false;
	}

	// 텍스처 맵핑 방식은 여러가지 있음.
	// EMappingMode
	// eNone: 맵핑이 결정되지 않았다.
	// eByControlPoint: 제어점마다 1개의 맵핑이 되어 있다. (각 정점이 UV좌표를 들고 있음.)
	// eByPolygonVertex: 각 정점이 속한 폴리곤에 맵핑에 좌표가 있다. (폴리곤에서 탐색 해야함)
	// eByPolygon: 전체 폴리곤에 맵핑 좌표가 있다. 
	// eByEdge: 엣지에 1개의 맵핑 좌표가 있다.
	// eAllSame: 전체 폴리곤에 1개의 맵핑 좌표가 있다.
	// 맵핑 방식 -> 저장 방식 확인 필요. == Reference Mode
	// eDirect: 리스트에 저장되어 있음. N번째 맵핑 정보가 DirectArray의 N번째 위치에 있다.
	// eIndex: FBX 5.0 이하 버전에서 사용되었음. 지금은 사용하지 않음. 그 이상의 버전에서는 eIndexToDirect로 대체됨.
	// eIndexToDirect: 인덱스에 저장되어 있음. 
	switch (tangent->GetMappingMode())
	{
	case FbxLayerElementUV::eByControlPoint:
	{
		// 정점 마다 UV가 1개씩 있으니 해당 Vertex에서 정보를 가져옴.
		switch (tangent->GetReferenceMode())
		{
		case FbxLayerElementUV::eDirect:
		{
			dst = tangent->GetDirectArray().GetAt(vertexIdx);
			break;
		}
		case FbxLayerElementUV::eIndexToDirect:
		{
			int id = tangent->GetIndexArray().GetAt(vertexIdx);
			dst = tangent->GetDirectArray().GetAt(id);
			break;
		}
		}
		break;
	}
	case FbxLayerElementUV::eByPolygonVertex:
	{
		// 폴리곤에 하나 있으니 다이렉트로 가져와도 된다.
		switch (tangent->GetReferenceMode())
		{
		case FbxLayerElementUV::eDirect:
			//{
			//	FbxVector2 fbxUv = _uv->GetDirectArray().GetAt(_uvIdx);
			//	_dst.mData[0] = fbxUv.mData[0];
			//	_dst.mData[1] = fbxUv.mData[1];
			//	break;
			//}
		{
			dst = tangent->GetDirectArray().GetAt(tangentIdx); // 다이렉트로 가져오면 됨.
			break;
		}
		case FbxLayerElementUV::eIndexToDirect:
		{
			//int id = _uv->GetIndexArray().GetAt(_uvIdx);
			//FbxVector2 fbxUv = _uv->GetDirectArray().GetAt(id);
			//_dst.mData[0] = fbxUv.mData[0];
			//_dst.mData[1] = fbxUv.mData[1];

			//_dst = _color->GetDirectArray().GetAt(_colorIdx); // 다이렉트로 가져오면 됨.
			int id = tangent->GetIndexArray().GetAt(tangentIdx);
			dst = tangent->GetDirectArray().GetAt(id);
			break;
		}
		}
		break;
	}

	}

	return true;
}

int FBXLoader::GetSubMaterialIndex(FbxLayerElementMaterial* material, int polyIdx)
{
	// 매핑방식
	//eNone,
	//eByControlPoint,  // 제어점
	//eByPolygonVertex, //  
	//eByPolygon, // 폴리곤마다 다를수 있다.
	//eAllSame - 전체표면에 1개의 매핑좌표가 있다.
	int subMaterial = 0;
	if (material != nullptr)
	{
		switch (material->GetMappingMode())
		{
		case FbxLayerElement::eByPolygon:
		{
			// 매핑 정보가 배열에 저장되는 방식
			switch (material->GetReferenceMode())
			{
			case FbxLayerElement::eIndex:
			{
				subMaterial = polyIdx;
			}break;
			case FbxLayerElement::eIndexToDirect:
			{
				subMaterial = material->GetIndexArray().GetAt(polyIdx);
			}break;
			}
		}
		default:
		{
			//break;
		}
		}
	}
	return subMaterial;
}

DXTexture* FBXLoader::FindTexture(FbxSurfaceMaterial* surface, const char* name, std::wstring* rst)
{
	if (surface == nullptr)
	{
		return nullptr;
	}

	//static const char* sShadingModel;
	//static const char* sMultiLayer;
	//
	//static const char* sEmissive;
	//static const char* sEmissiveFactor;
	//
	//static const char* sAmbient;
	//static const char* sAmbientFactor;
	//
	//static const char* sDiffuse;
	//static const char* sDiffuseFactor;
	//
	//static const char* sSpecular;
	//static const char* sSpecularFactor;
	//static const char* sShininess;
	//
	//static const char* sBump;
	//static const char* sNormalMap;
	//static const char* sBumpFactor;
	//
	//static const char* sTransparentColor;
	//static const char* sTransparencyFactor;
	//
	//static const char* sReflection;
	//static const char* sReflectionFactor;
	//
	//static const char* sDisplacementColor;
	//static const char* sDisplacementFactor;
	//
	//static const char* sVectorDisplacementColor;
	//static const char* sVectorDisplacementFactor;

	// FbxSurfaceMaterial
	// 텍스처 정보를 가져오기 위한 것. 보통 1개의 Surface에 24개 이상의 텍스쳐가 붙어 있다.(24종 이상의 텍스쳐 방식이 존재한다.)
	// 텍스쳐 맵을 가지고 있다(ex. 마스크 텍스처처럼 알파를 가진 놈들 등 여러가지 종류가 있음.)
	std::string textureName;
	auto prop = surface->FindProperty(name); 
	if (prop.IsValid())
	{
		const FbxFileTexture* fbxFile = prop.GetSrcObject<FbxFileTexture>();
		if (fbxFile != nullptr)
		{
			textureName = fbxFile->GetFileName();
			if (!textureName.empty())
			{
				std::filesystem::path path(textureName);
				std::wstring file = path.filename().c_str();
				std::wstring wstrPath = ResourceDirection;
				wstrPath += file;
				if (rst != nullptr)
				{
					rst->assign(wstrPath.begin(), wstrPath.end());
				}

				if (DXTextureManager::GetInstance()->Load(wstrPath))
				{
					DXTexture* pTexture = DXTextureManager::GetInstance()->GetTexture(wstrPath);
					if (pTexture != nullptr)
					{
						return pTexture;
					}
					else
					{
						return nullptr;
					}
				}
			}
		}
		else
		{
			int LayerdTextureNum = prop.GetSrcObjectCount<FbxLayeredTexture>();
			for (int idx = 0; idx < LayerdTextureNum; idx++)
			{
				FbxFileTexture* pLayerTexture = prop.GetSrcObject<FbxFileTexture>(idx);
				if (pLayerTexture != nullptr)
				{
					std::string filename = pLayerTexture->GetFileName();
				}
			}
		}
	}
	
	return nullptr;
}

std::wstring FBXLoader::GetTextureFileName(FbxSurfaceMaterial* surface, const char* name)
{
	if (surface == nullptr)
	{
		return L"";
	}

	//static const char* sShadingModel;
	//static const char* sMultiLayer;
	//
	//static const char* sEmissive;
	//static const char* sEmissiveFactor;
	//
	//static const char* sAmbient;
	//static const char* sAmbientFactor;
	//
	//static const char* sDiffuse; // 기존에 많이 사용하던 텍스쳐 방식. 보통 Diffuse는 무조건 있음. 기본 방식
	//static const char* sDiffuseFactor;
	//
	//static const char* sSpecular;
	//static const char* sSpecularFactor;
	//static const char* sShininess;
	//
	//static const char* sBump;
	//static const char* sNormalMap;
	//static const char* sBumpFactor;
	//
	//static const char* sTransparentColor;
	//static const char* sTransparencyFactor;
	//
	//static const char* sReflection;
	//static const char* sReflectionFactor;
	//
	//static const char* sDisplacementColor;
	//static const char* sDisplacementFactor;
	//
	//static const char* sVectorDisplacementColor;
	//static const char* sVectorDisplacementFactor;
	std::wstring rst;
	auto prop = surface->FindProperty(name);
	if (prop.IsValid())
	{
		const FbxFileTexture* fbxFile = prop.GetSrcObject<FbxFileTexture>();
		if (fbxFile != nullptr)
		{
			std::string textureName = fbxFile->GetFileName();
			if (!textureName.empty())
			{
				std::filesystem::path path(textureName);
				rst = path.filename().c_str();
			}
		}
	}

	return rst;
}

FbxAMatrix FBXLoader::GetGeometryMatrix(FbxNode* node)
{
	// 로컬 행렬
	// Affine 행렬은 정점 변환 할 때, 그냥 행렬은 행렬 끼리 연산 할 때 사용
	FbxAMatrix geometryMatrix; // 기하 행렬. 초기 정점 위치를 변환 할 때 사용. 로컬 행렬. 이 매트릭스를 적용해 바뀌지 않으면 월드 행렬에서 변환. 정점 변환.
	FbxVector4 translation = node->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 rotation = node->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 scale = node->GetGeometricScaling(FbxNode::eSourcePivot);
	geometryMatrix.SetT(translation);
	geometryMatrix.SetR(rotation);
	geometryMatrix.SetS(scale);

	return geometryMatrix;
}

FbxAMatrix FBXLoader::GetNormalMatrix(const FbxAMatrix& src)
{
	// 노말 성분은 역행렬의 전치 행렬로 곱해 줘야 함.
	FbxAMatrix normalMatrix = src;
	normalMatrix = normalMatrix.Inverse();
	normalMatrix = normalMatrix.Transpose();
	return normalMatrix;
}

FbxAMatrix FBXLoader::GetWorldMatrix(FbxNode* node)
{
	// 월드 행렬, 애니메이션에서 사용 하면 안됨. 원래는 밖에서 사용해야 하나 임시로 사용
	FbxVector4 translation_World;
	if (node->LclTranslation.IsValid())
	{
		translation_World = node->LclTranslation;
	}

	FbxVector4 rotation_World;
	if (node->LclRotation.IsValid())
	{
		rotation_World = node->LclRotation;
	}

	FbxVector4 scale_World;
	if (node->LclScaling.IsValid())
	{
		scale_World = node->LclScaling;
	}

	FbxAMatrix worldMatrix;
	worldMatrix.SetT(translation_World);
	worldMatrix.SetR(rotation_World);
	worldMatrix.SetS(scale_World);

	return worldMatrix;
}

void FBXLoader::SetResourceDirectory(std::wstring dir)
{
	ResourceDirection = dir;
}

Matrix FBXLoader::ToMatrix(const FbxAMatrix& _src)
{
	Matrix rst;
	float* arry = (float*)(&_src);
	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			double val = _src.Get(row, col);
			rst.m[row][col] = static_cast<float>(val);
		}
	}

	return rst;
}

Matrix FBXLoader::ConvertToDxMatrix(const FbxAMatrix& _src)
{
	Matrix rst;
	Matrix src = ToMatrix(_src);
	rst._11 = src._11; rst._12 = src._13; rst._13 = src._12;
	rst._21 = src._31; rst._22 = src._33; rst._23 = src._32;
	rst._31 = src._21; rst._32 = src._23; rst._33 = src._22;
	rst._41 = src._41; rst._42 = src._43; rst._43 = src._42;
	rst._14 = rst._24 = rst._34 = 0.0f;
	rst._44 = 1.0f;

	return rst;
}

bool FBXLoader::GenerateAnimationTrack(FBXFileData* data, float sampling)
{
	FbxTime time;
	UINT StartFrame = data->AnimationSceneInfo.StartFrame;
	UINT EndFrame = data->AnimationSceneInfo.EndFrame;
	FbxTime::EMode TimeMode = data->AnimationSceneInfo.TimeMode;
	std::vector<std::vector<FBXAnimationTrack>> TrackList;
	size_t NodeNum = data->NodeList.size();
	TrackList.resize(NodeNum);
	for (UINT t = StartFrame; t <= EndFrame; t++)
	{
		time.SetFrame(t, TimeMode); // 이게 시간을 많이 잡아먹어서 최대한 적게 호출하는게 좋다.
		for (size_t NodeIdx = 0; NodeIdx < NodeNum; NodeIdx++)
		{
			FbxNode* currentNode = data->NodeList[NodeIdx];
			FBXAnimationTrack Track;
			Track.Frame = t;
			FbxAMatrix fbxMatrix = currentNode->EvaluateGlobalTransform(time);
			Track.AnimationMatrix = ConvertToDxMatrix(fbxMatrix);
			Matrix4x4Decompose(Track.AnimationMatrix, Track.Scale, Track.Rotation, Track.Translation);

			TrackList[NodeIdx].push_back(Track);
		}
	}

	for (size_t NodeIdx = 0; NodeIdx < NodeNum; NodeIdx++)
	{
		// Original Animation Track List Map.
		FbxNode* currentNode = data->NodeList[NodeIdx];
		std::string NodeName = currentNode->GetName();
		data->AnimationTrackMap.insert(std::make_pair(NodeName, TrackList[NodeIdx]));

		// Generate Matrix List Map of Interpolation Animation.
		size_t TrackSize = TrackList[NodeIdx].size();
		std::vector<Matrix> InterpolationMatrixList;
		//InterpolationMatrixList.resize(TrackSize * _data->InterpolationSampling);
		for (size_t FrameIdx = 0; FrameIdx < TrackSize; FrameIdx++)
		{
			FBXAnimationTrack A, B;
			UINT FrameA = max(FrameIdx + 0, data->AnimationSceneInfo.StartFrame);
			UINT FrameB = min(FrameIdx + 1, data->AnimationSceneInfo.EndFrame);
			A = TrackList[NodeIdx][FrameA];
			B = TrackList[NodeIdx][FrameB];
			if (A.Frame == B.Frame) // End Frame
			{
				//InterpolationMatrixList[FrameIdx] = TrackList[NodeIdx][FrameIdx].matAnimation;
				InterpolationMatrixList.push_back(TrackList[NodeIdx][FrameIdx].AnimationMatrix);
				continue;
			}

			//float t = (FrameIdx - A.frame) / (B.frame - A.frame);
			float tick = 1.0f / sampling; // / _data->InterpolationSampling; // Sampling 수 만큼 보간.
			for (float t = 0.0f; t < 1.0f; t += tick)
			{
				Vector3 translation = LinearInterpolation(A.Translation, B.Translation, t);
				Vector3 scale = LinearInterpolation(A.Scale, B.Scale, t);
				Matrix matScale;
				//matScale = matScale.Identity();
				//matScale.Identity();
				matScale._11 = scale.x;
				matScale._22 = scale.y;
				matScale._33 = scale.z;
				Vector4 qRotation = SphereLinearInterpolation(A.Rotation, B.Rotation, t);
				Matrix matRotation = QuaternionToMatrix4x4(qRotation);
				Matrix rst = matScale * matRotation;
				rst._41 = translation.x;
				rst._42 = translation.y;
				rst._43 = translation.z;

				//InterpolationMatrixList[FrameIdx] = rst;
				InterpolationMatrixList.push_back(rst);
			}
		}
		data->InterpolationFrameMatrixList.insert(std::make_pair(NodeName, InterpolationMatrixList));
	}

	return true;
}

bool FBXLoader::GenerateStaticMeshFromFileData(std::wstring filename, StaticMeshComponent* dst)
{
	auto it = FbxFileList.find(filename);
	if (it == FbxFileList.end())
	{
		return false;
	}
		
	FBXFileData* pData = it->second;
	for (auto& node : pData->NodeDataList)
	{
		if (node.AttributeType != FbxNodeAttribute::EType::eMesh)
		{
			continue;
		}
		else
		{
			if (node.MeshList.empty())
			{
				continue;
			}
		
			size_t meshCnt = node.MeshList.size();
			for (size_t idx = 0; idx < meshCnt; idx++)
			{
				if (node.MaterialList.empty())
				{
					node.MeshList[idx].MaterialSlot = MaterialManager::GetInstance()->GetMaterial(L"Default");
				}
				else
				{
					std::wstring materialName;
					materialName.assign(node.Name.begin(), node.Name.end());
					node.MeshList[idx].MaterialSlot = MaterialManager::GetInstance()->GetMaterial(materialName);
				}
				
				dst->Meshes.push_back(node.MeshList[idx]);				
			}
					
		}
	}
		
	for (auto& it : dst->Meshes)
	{
		it.Initialize();
	}

	return true;
}

bool FBXLoader::GenerateSkeletalMeshFromFileData(std::wstring filename, SkeletalMeshComponent* dst)
{
	auto it = FbxFileList.find(filename);
	if (it == FbxFileList.end())
	{
		return false;
	}

	FBXFileData* pData = it->second;
	// Root Node Name 할당(?)
	dst->Name = pData->NodeNameList.front();

	////////
	for (auto& node : pData->NodeDataList)
	{
		if (node.AttributeType != FbxNodeAttribute::EType::eMesh)
		{
			continue;
		}
		else
		{
			if (node.MeshList.empty())
			{
				continue;
			}

			size_t meshCnt = node.MeshList.size();
			// 싱글메시라고 가정하고/////////
			dst->BindPoseMap = node.BindPoseMap;
			dst->BindPoseKeyToIndexMap = node.BindPoseKeyToIndexMap;

			///////////////
			for (size_t idx = 0; idx < meshCnt; idx++)
			{
				if (node.MaterialList.empty())
				{
					node.MeshList[idx].MaterialSlot = MaterialManager::GetInstance()->GetMaterial(L"Default");
				}
				else
				{
					std::wstring materialName;
					if (idx >= 1)
					{
						std::wstring subMaterial = std::to_wstring(idx);
						materialName += subMaterial;
					}
					materialName.assign(node.Name.begin(), node.Name.end());
					node.MeshList[idx].MaterialSlot = MaterialManager::GetInstance()->GetMaterial(materialName);
				}

				dst->Meshes.push_back(node.MeshList[idx]);
			}

		}
	}

	for (auto& it : dst->Meshes)
	{
		it.Initialize();
	}

	return true;
}

bool FBXLoader::GenerateAnimationFromFileData(std::wstring filename, AnimationComponent* dst, bool Loop)
{
	auto it = FbxFileList.find(filename);
	if (it == FbxFileList.end())
	{
		return false;
	}

	FBXFileData* pData = it->second;

	std::filesystem::path path = filename;
	std::wstring name = path.filename();

	auto pos = name.rfind('.'); // 확장자명의 시작 위치 검색
	if (pos != std::wstring::npos) { // 확장자명이 있을 경우
		name = name.substr(0, pos); // 확장자명 제외한 파일명 추출
	}

	AnimationClip* clip = new AnimationClip;
	clip->FileName = name;
	clip->StartFrame = pData->AnimationSceneInfo.StartFrame;
	clip->EndFrame = pData->AnimationSceneInfo.EndFrame;
	clip->TickPerFrame = pData->AnimationSceneInfo.TickPerFrame;
	clip->FrameSpeed = pData->AnimationSceneInfo.FrameSpeed;
	clip->LerpFrameMatrixList = pData->InterpolationFrameMatrixList;
	clip->LoopState = Loop;	// Default = true

	dst->AddClip(name, clip);

	return true;
}

bool FBXLoader::GenerateWeaponMeshFromFileData(std::wstring filename, WeaponMeshComponent* dst)
{
	auto it = FbxFileList.find(filename);
	if (it == FbxFileList.end())
	{
		return false;
	}

	FBXFileData* pData = it->second;
	for (auto& node : pData->NodeDataList)
	{
		if (node.AttributeType != FbxNodeAttribute::EType::eMesh)
		{
			continue;
		}
		else
		{
			if (node.MeshList.empty())
			{
				continue;
			}

			size_t meshCnt = node.MeshList.size();
			for (size_t idx = 0; idx < meshCnt; idx++)
			{
				if (node.MaterialList.empty())
				{
					node.MeshList[idx].MaterialSlot = MaterialManager::GetInstance()->GetMaterial(L"Default");
				}
				else
				{
					std::wstring materialName;
					materialName.assign(node.Name.begin(), node.Name.end());
					node.MeshList[idx].MaterialSlot = MaterialManager::GetInstance()->GetMaterial(materialName);
				}

				dst->Meshes.push_back(node.MeshList[idx]);
			}

		}
	}

	for (auto& it : dst->Meshes)
	{
		it.Initialize();
	}

	return true;
}