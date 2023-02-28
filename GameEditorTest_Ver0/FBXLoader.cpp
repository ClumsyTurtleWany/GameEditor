#include "pch.h"
#include "FBXLoader.hpp"

bool FBXLoader::initialize()
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

bool FBXLoader::frame()
{
	return true;
}

bool FBXLoader::render()
{
	return true;
}

bool FBXLoader::release()
{
	m_ObjectMap.clear();

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

	return true;
}

bool FBXLoader::LoadDir(std::wstring _path)
{
	setResourceDirectory(_path);
	std::filesystem::path path(_path);
	for (auto& file : std::filesystem::directory_iterator(path))
	{
		std::wstring filename = file.path().filename();
		std::wstring filepath = file.path();
		std::wstring fileExtension = file.path().extension();

		if (fileExtension == L"")
		{
			std::wstring dir = filepath + L"/";
			LoadDir(dir);
		}
		else
		{
			if ((fileExtension == L".FBX") || (fileExtension == L".fbx"))
			{
				if (filename == L"Turret_Deploy1.FBX")
				{
					int a = 0;
				}

				/*auto it = m_ObjectMap.find(filename);
				if (it != m_ObjectMap.end())
				{
					continue;
				}

				std::unique_ptr<FBXObject> Object = std::make_unique<FBXObject>();
				if (Load(filepath, Object.get()))
				{
					m_ObjectMap.insert(std::make_pair(filename, std::move(Object)));
				}*/

				auto it = m_StaticMeshMap.find(filename);
				if (it != m_StaticMeshMap.end())
				{
					continue;
				}

				std::unique_ptr<StaticMeshComponent> staticMesh = std::make_unique<StaticMeshComponent>();
				if (Load(filepath, staticMesh.get()))
				{
					KeyStringList.push_back(filename);
					m_StaticMeshMap.insert(std::make_pair(filename, std::move(staticMesh)));
				}

			}
			else
			{
				continue;
			}
		}
	}

	return true;
}

bool FBXLoader::Load(std::wstring _path, FBXObject* _dst)
{
	std::string strPath;
	strPath.assign(_path.begin(), _path.end());
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

	FBXFileData fbxFile;
	if (!ParseScene(pScene, &fbxFile))
	{
		OutputDebugString(L"FBXLoader::Load::Failed Parse Scene.\n");
		return false;
	}

	FbxNode* pRoot = pScene->GetRootNode();
	if (!ParseNode(pRoot, &fbxFile))
	{
		OutputDebugString(L"FBXLoader::Load::Failed Parse Root Node.\n");
		return false;
	}

	if (!PreProcess(&fbxFile))
	{
		OutputDebugString(L"FBXLoader::Load::Failed Pre Process.\n");
		return false;
	}

	if (!GenerateObjectFromFileData(&fbxFile, _dst))
	{
		OutputDebugString(L"FBXLoader::Load::Failed Generate FBX Object.\n");
		return false;
	}

	pRoot->Destroy();
	pScene->Destroy();

	return true;
}

bool FBXLoader::Load(std::wstring _path, StaticMeshComponent* _dst)
{
	std::string strPath;
	strPath.assign(_path.begin(), _path.end());
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

	FBXFileData fbxFile;
	if (!ParseScene(pScene, &fbxFile))
	{
		OutputDebugString(L"FBXLoader::Load::Failed Parse Scene.\n");
		return false;
	}

	FbxNode* pRoot = pScene->GetRootNode();
	if (!ParseNode(pRoot, &fbxFile))
	{
		OutputDebugString(L"FBXLoader::Load::Failed Parse Root Node.\n");
		return false;
	}

	if (!PreProcess(&fbxFile))
	{
		OutputDebugString(L"FBXLoader::Load::Failed Pre Process.\n");
		return false;
	}

	if (!GenerateStaticMeshFromFileData(&fbxFile, _dst))
	{
		OutputDebugString(L"FBXLoader::Load::Failed Generate FBX Object.\n");
		return false;
	}

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

bool FBXLoader::ParseScene(FbxScene* _scene, FBXFileData* _dst)
{
	if ((_scene == nullptr) || (_dst == nullptr))
	{
		return false;
	}

	// 각 FBX를 만드는 툴(3D Max, 마야 등 디자인 툴)마다 기저 축과 단위가 다를 수 있기 때문에 설정 후 로딩 할 수 있다.
	// 보통의 FBX는 기본적으로 오른손 좌표계, Y-Up
	//FbxSystemUnit::cm.ConvertScene(_scene); // 단위 변경. cm 단위로 변경
	FbxSystemUnit::m.ConvertScene(_scene); // 단위 변경. m 단위로 변경
	//FbxAxisSystem::MayaZUp.ConvertScene(pScene); // 기저 축 변경. 마야 기준으로 변경. 정점 변환이 아닌 행렬에 적용 되는 것임.
	FbxAxisSystem sceneAxisSystem = _scene->GetGlobalSettings().GetAxisSystem();

	// Test Code
	int upSign = 0;
	FbxAxisSystem::EUpVector up = sceneAxisSystem.GetUpVector(upSign);

	int frontSign = 0;
	FbxAxisSystem::EFrontVector front = sceneAxisSystem.GetFrontVector(frontSign);

	FbxAxisSystem::ECoordSystem coord = sceneAxisSystem.GetCoorSystem();
	bool bFlag = true;
	if (((up == FbxAxisSystem::EUpVector::eYAxis) && (upSign == 1)) &&
		((front == FbxAxisSystem::EFrontVector::eParityEven) && (frontSign == 1)) &&
		((up == FbxAxisSystem::ECoordSystem::eLeftHanded)))
	{
		bFlag = false;
	}

	//FbxAxisSystem::DirectX.ConvertScene(pScene); // 기저 축 변경. DirectX 기준으로 바꿔도 실제 우리가 사용하는 축과 다름. (Right Vector가 마이너스임.).
	//FbxAxisSystem sceneAxisSystemAfter = pScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem::MayaZUp.ConvertScene(_scene); // 기저 축 변경. 마야 기준으로 변경. 정점 변환이 아닌 행렬에 적용 되는 것임.


	// Animation Scene Information
	FbxAnimStack* pStack = _scene->GetSrcObject<FbxAnimStack>(0);
	FbxLongLong s = 0;
	FbxLongLong n = 0;
	FbxTime::EMode TimeMode = FbxTime::GetGlobalTimeMode();
	if (pStack != nullptr)
	{
		FbxTime::SetGlobalTimeMode(FbxTime::eFrames30);
		//FbxTime::EMode TimeMode = FbxTime::GetGlobalTimeMode();
		FbxTimeSpan localTimeSpan = pStack->GetLocalTimeSpan();// 시간 간격. 프레임 사이
		FbxTime start = localTimeSpan.GetStart();
		FbxTime end = localTimeSpan.GetStop();
		FbxTime Duration = localTimeSpan.GetDirection();
		s = start.GetFrameCount(TimeMode);
		n = end.GetFrameCount(TimeMode);
	}

	_dst->AnimationSceneInfo.TimeMode = TimeMode;
	_dst->AnimationSceneInfo.StartFrame = s;
	_dst->AnimationSceneInfo.EndFrame = n;

	return true;
}

bool FBXLoader::ParseNode(FbxNode* _node, FBXFileData* _dst)
{
	if ((_node == nullptr) || (_dst == nullptr))
	{
		OutputDebugString(L"FBXLoader::ParseNode::Node or Dest is nullptr.\n");
		return false;
	}

	bool isValid = false;

	FBXNodeData NodeData;
	NodeData.Name = _node->GetName();
	FbxNodeAttribute* pAttribute = _node->GetNodeAttribute();
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
				NodeData.Dummy = _node->GetNull();
				if (NodeData.Dummy != nullptr)
				{
					// Dummy: 자식 오브젝트의 원점과 부모 오브젝트의 원점을 맞춰주기 위한 정보.
					//ParseDummy(pDummy, _dst);
					_dst->DummyList.push_back(NodeData.Dummy);
				}
				break;
			}

			case FbxNodeAttribute::EType::eSkeleton:
			{
				isValid = true;
				//FbxSkeleton* pSkeleton = _node->GetSkeleton();
				NodeData.Skeleton = _node->GetSkeleton();
				if (NodeData.Skeleton != nullptr)
				{
					// Skeleton: 애니메이션을 위한 정보
					//ParseSkeleton(pSkeleton, _dst);
					_dst->SkeletonList.push_back(NodeData.Skeleton);
				}
				break;
			}

			case FbxNodeAttribute::EType::eMesh:
			{
				isValid = true;
				//FbxMesh* pMesh = _node->GetMesh();
				NodeData.Mesh = _node->GetMesh();
				if (NodeData.Mesh != nullptr)
				{
					// Mesh: 랜더 가능한 데이터
					// Scene graph 형식(트리에 모든 정보를 다 넣어서 저장 후 사용 및 랜더링하는 방식) 이라고 부름.
					//ParseMesh(pMesh, _dst);
					_dst->MeshList.push_back(NodeData.Mesh);
				}
				break;
			}
		}
	}

	if (isValid)
	{
		_dst->NodeList.push_back(_node);
		_dst->NodeNameList.push_back(NodeData.Name);

		// 로컬 행렬
		// Affine 행렬은 정점 변환 할 때, 그냥 행렬은 행렬 끼리 연산 할 때 사용
		NodeData.LocalGeometryMatrix = getGeometryMatrix(_node); // 기하 행렬. 초기 정점 위치를 변환 할 때 사용. 로컬 행렬. 이 매트릭스를 적용해 바뀌지 않으면 월드 행렬에서 변환. 정점 변환.
		_dst->LocalGeometryMatrixMap.insert(std::make_pair(NodeData.Name, NodeData.LocalGeometryMatrix));

		// 노말 성분은 역행렬의 전치 행렬로 곱해 줘야 함.
		NodeData.LocalNormalMatrix = getNormalMatrix(NodeData.LocalGeometryMatrix);
		_dst->NormalMatrixMap.insert(std::make_pair(NodeData.Name, NodeData.LocalGeometryMatrix));


		// 각 페이스 별로 다른 텍스쳐를 사용 할 수 있다. 이것을 서브 머테리얼 이라고 함. (1개의 오브젝트에 여러개의 텍스쳐 사용)
		// 서브 머테리얼을 렌더링 하기 위해선 같은 텍스쳐를 사용하는 페이스들을 묶어서 출력.
		NodeData.MaterialNum = _node->GetMaterialCount(); // 텍스쳐가 붙은 갯수.
		for (int idx = 0; idx < NodeData.MaterialNum; idx++)
		{
			FbxMaterial material;
			// 텍스처 정보를 가져오기 위한 것. 보통 1개의 Surface에 24개 이상의 텍스쳐가 붙어 있다.(24종 이상의 텍스쳐 방식이 존재한다.)
			// 텍스쳐 맵을 가지고 있다(ex. 마스크 텍스처처럼 알파를 가진 놈들 등 여러가지 종류가 있음.)
			material.Surface = _node->GetMaterial(idx);
			if (material.Surface != nullptr)
			{
				material.DiffuseTexture = getTextureFileName(material.Surface, FbxSurfaceMaterial::sDiffuse);
			}

			NodeData.Materials.push_back(material);
		}

		if (NodeData.MaterialNum == 0)
		{
			FbxMaterial material;
			NodeData.Materials.push_back(material);
		}

		_dst->NodeDataList.push_back(NodeData);
	}

	int childCount = _node->GetChildCount(); // Child 갯수가 0이면 정적 매쉬, 0이 아니면 동적 매쉬로 볼 수 있음.
	for (int idx = 0; idx < childCount; idx++)
	{
		FbxNode* pChild = _node->GetChild(idx);
		if (isValid |= ParseNode(pChild, _dst))
		{
		}
		else
		{
		}
	}
	
	return isValid;
}

bool FBXLoader::PreProcess(FBXFileData* _dst)
{
	if (!GenerateAnimationTrack(_dst))
	{

	}

	/*for (auto it : _dst->DummyList)
	{
		if (!ParseDummy(it, _dst))
		{

		}
	}

	for (auto it : _dst->SkeletonList)
	{
		if (!ParseSkeleton(it, _dst))
		{

		}
	}

	for (auto it : _dst->MeshList)
	{
		if (!ParseMesh(it, _dst))
		{

		}
	}*/

	for (auto &it : _dst->NodeDataList)
	{
		if (!ParseDummy(it.Dummy, _dst))
		{

		}

		if (!ParseSkeleton(it.Skeleton, _dst))
		{

		}

		if (!ParseMesh(it.Mesh, _dst, &it))
		{

		}
	}

	return true;
}

bool FBXLoader::ParseMesh(FbxMesh* _mesh, FBXFileData* _dst, FBXNodeData* _dstData)
{
	if ((_mesh == nullptr) || (_dst == nullptr))
	{
		return false;
	}

	//MeshData meshData;

	if (!ParseMeshSkinning(_mesh, _dst , _dstData))
	{

	}

	// Layer 개념 필요. 여러번에 걸쳐 동일한 곳에 랜더링 하는것 == 멀티 패스 랜더링. 텍스쳐로 치환하면 멀티 텍스처 랜더링.
	std::vector<FbxLayerElementUV*> uvList;
	std::vector<FbxLayerElementVertexColor*> colorList;
	std::vector<FbxLayerElementMaterial*> materialList;
	std::vector<FbxLayerElementNormal*> normalList;
	std::vector<FbxLayerElementTangent*> tangentList;
	
	int layerCount = _mesh->GetLayerCount();
	//meshData.LayerList.resize(layerCount);
	for (int layerIdx = 0; layerIdx < layerCount; layerIdx++)
	{
		FbxLayer* pLayer = _mesh->GetLayer(layerIdx);
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

		//meshData.LayerList[layerIdx].ElementUV = pUV;
		//meshData.LayerList[layerIdx].ElementColor = pColor;
		//meshData.LayerList[layerIdx].ElementMaterialList = pMaterial;
		//meshData.LayerList[layerIdx].ElementNormalList = pNormal;
	}

	//FbxNode* pNode = _mesh->GetNode(); // 원래는 Mesh가 아닌 Node로 돌리는게 맞다.  
	//// 각 페이스 별로 다른 텍스쳐를 사용 할 수 있다. 이것을 서브 머테리얼 이라고 함. (1개의 오브젝트에 여러개의 텍스쳐 사용)
	//// 서브 머테리얼을 렌더링 하기 위해선 같은 텍스쳐를 사용하는 페이스들을 묶어서 출력.
	//int MaterialCnt = pNode->GetMaterialCount(); // 텍스쳐가 붙은 갯수.
	//meshData.MaterialNameList.resize(MaterialCnt);
	////std::vector<std::vector<Vertex>> VertexList;
	//for (size_t LayerIdx = 0; LayerIdx < layerCount; LayerIdx++)
	//{
	//	if (MaterialCnt < 1)
	//	{
	//		meshData.LayerList[LayerIdx].MaterialList.resize(1);
	//	}
	//	else
	//	{
	//		meshData.LayerList[LayerIdx].MaterialList.resize(MaterialCnt);
	//	}
	//}

	//for (int idx = 0; idx < MaterialCnt; idx++)
	//{
	//	// 텍스처 정보를 가져오기 위한 것. 보통 1개의 Surface에 24개 이상의 텍스쳐가 붙어 있다.(24종 이상의 텍스쳐 방식이 존재한다.)
	//	// 텍스쳐 맵을 가지고 있다(ex. 마스크 텍스처처럼 알파를 가진 놈들 등 여러가지 종류가 있음.)
	//	FbxSurfaceMaterial* pSurface = pNode->GetMaterial(idx);
	//	if (pSurface != nullptr)
	//	{
	//		_dst->Materials[idx].DiffuseTexture = getTextureFileName(pSurface, FbxSurfaceMaterial::sDiffuse);
	//	}
	//}


	//// 로컬 행렬
	//// Affine 행렬은 정점 변환 할 때, 그냥 행렬은 행렬 끼리 연산 할 때 사용
	//FbxAMatrix geometryMatrix = getGeometryMatrix(pNode); // 기하 행렬. 초기 정점 위치를 변환 할 때 사용. 로컬 행렬. 이 매트릭스를 적용해 바뀌지 않으면 월드 행렬에서 변환. 정점 변환.

	//// 노말 성분은 역행렬의 전치 행렬로 곱해 줘야 함.
	//FbxAMatrix normalMatrix = getNormalMatrix(geometryMatrix);


	size_t LayerIdx = 0;
	int polyCount = _mesh->GetPolygonCount();
	// 3정점 = 1폴리곤(Triangle) 일 수도 있고
	// 4정점 = 1폴리곤(Quad) 일 수도 있다.
	// 폴리곤 -> 페이스 -> 정점
	int basePolyIdx = 0; // Color
	int MaterialIdx = 0;
	FbxVector4* pVertexPosition = _mesh->GetControlPoints(); // 제어점(Control point == Vertices). 정점의 시작 위치. 
	for (int polyIdx = 0; polyIdx < polyCount; polyIdx++)
	{
		int polySize = _mesh->GetPolygonSize(polyIdx);
		int faceCount = polySize - 2;
		if (!materialList.empty())
		{
			MaterialIdx = getSubMaterialIndex(materialList[LayerIdx], polyIdx);
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
			cornerIdx[0] = _mesh->GetPolygonVertex(polyIdx, 0);
			cornerIdx[1] = _mesh->GetPolygonVertex(polyIdx, faceIdx + 2);
			cornerIdx[2] = _mesh->GetPolygonVertex(polyIdx, faceIdx + 1);

			int UVidx[3] = { 0, };
			UVidx[0] = _mesh->GetTextureUVIndex(polyIdx, 0);
			UVidx[1] = _mesh->GetTextureUVIndex(polyIdx, faceIdx + 2);
			UVidx[2] = _mesh->GetTextureUVIndex(polyIdx, faceIdx + 1);

			int vertexColorIdx[3] = { 0, faceIdx + 2, faceIdx + 1 };
			for (int idx = 0; idx < 3; idx++)
			{
				int vertexIdx = cornerIdx[idx];
				FbxVector4 vertex = pVertexPosition[vertexIdx];
				vertex = _dstData->LocalGeometryMatrix.MultT(vertex); // 로컬 행렬 변환, Transform의 T, 열 우선 방식
				//vertex = worldMatrix.MultT(vertex); // 월드 변환 행렬. 나중에 뺄 것.
				//vertex = matGlobalTransform.MultT(vertex);

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
						fbxNormal = _dstData->LocalNormalMatrix.MultT(fbxNormal);
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
				if (_dstData->BindPoseMap.empty())
				{
					_dstData->Materials[MaterialIdx].push_back(Vertex(pos, normal, color, texture));
				}
				else
				{
					SkinningData skinData = _dstData->SkinningList[vertexIdx];
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
								IWDatas.Index.x = _dstData->BindPoseKeyToIndexMap.find(SkinWeightList[SkinWeightIdx].BoneName)->second;
								IWDatas.Weight.x = SkinWeightList[SkinWeightIdx].weight;
							}
							if (SkinWeightIdx == 1)
							{
								IWDatas.Index.y = _dstData->BindPoseKeyToIndexMap.find(SkinWeightList[SkinWeightIdx].BoneName)->second;
								IWDatas.Weight.y = SkinWeightList[SkinWeightIdx].weight;
							}
							if (SkinWeightIdx == 2)
							{
								IWDatas.Index.z = _dstData->BindPoseKeyToIndexMap.find(SkinWeightList[SkinWeightIdx].BoneName)->second;
								IWDatas.Weight.z = SkinWeightList[SkinWeightIdx].weight;
							}
							if (SkinWeightIdx == 3)
							{
								IWDatas.Index.w = _dstData->BindPoseKeyToIndexMap.find(SkinWeightList[SkinWeightIdx].BoneName)->second;
								IWDatas.Weight.w = SkinWeightList[SkinWeightIdx].weight;
							}
							
						}
						else
						{
							break;
						}
					}

					_dstData->Materials[MaterialIdx].push_back(Vertex(pos, normal, color, texture), IWDatas);
				}
			}


		}

		basePolyIdx += polySize;
	}

	/*for (size_t idx = 0; idx < _dst->Materials.size(); idx++)
	{
		if (_dst->Materials[idx].isValid())
		{
			_dst->Materials[idx].create();
			if (DXTextureManager::getInstance()->Load(m_wstrResourceDir + _dst->Materials[idx].DiffuseTexture))
			{
				DXTexture* pTexture = DXTextureManager::getInstance()->getTexture(m_wstrResourceDir + _dst->Materials[idx].DiffuseTexture);
				if (pTexture != nullptr)
				{
					_dst->Materials[idx].setTexture(pTexture);
				}
			}
		}
	}*/

	return true;
}

bool FBXLoader::ParseMeshLayer(FbxMesh* _mesh, MeshData* _dstData)
{
	if (_mesh == nullptr || _dstData == nullptr)
	{
		return false;
	}

	// Layer 개념 필요. 여러번에 걸쳐 동일한 곳에 랜더링 하는것 == 멀티 패스 랜더링. 텍스쳐로 치환하면 멀티 텍스처 랜더링.
	std::vector<FbxLayerElementUV*> UVList;
	std::vector<FbxLayerElementVertexColor*> ColorList;
	std::vector<FbxLayerElementMaterial*> MaterialList;
	std::vector<FbxLayerElementNormal*> NormalList;

	int layerCount = _mesh->GetLayerCount();
	_dstData->LayerList.resize(layerCount);
	for (int layerIdx = 0; layerIdx < layerCount; layerIdx++)
	{
		FbxLayer* pLayer = _mesh->GetLayer(layerIdx);
		FbxLayerElementUV* pUV = pLayer->GetUVs();
		if (pUV != nullptr)
		{
			UVList.push_back(pUV);
		}

		FbxLayerElementVertexColor* pColor = pLayer->GetVertexColors();
		if (pColor != nullptr)
		{
			ColorList.push_back(pColor);
		}

		FbxLayerElementMaterial* pMaterial = pLayer->GetMaterials();
		if (pMaterial != nullptr)
		{
			MaterialList.push_back(pMaterial);
		}

		FbxLayerElementNormal* pNormal = pLayer->GetNormals();
		if (pNormal != nullptr)
		{
			NormalList.push_back(pNormal);
		}

		_dstData->LayerList[layerIdx].ElementUV = pUV;
		_dstData->LayerList[layerIdx].ElementColor = pColor;
		_dstData->LayerList[layerIdx].ElementMaterialList = pMaterial;
		_dstData->LayerList[layerIdx].ElementNormalList = pNormal;
	}

	return true;
}

bool FBXLoader::ParseMeshSkinning(FbxMesh* _mesh, FBXFileData* _dst, FBXNodeData* _dstData)
{
	if ((_mesh == nullptr) || (_dst == nullptr))
	{
		return false;
	}

	// Skinning
	// VertexCnt == 메쉬으 정점 개수와 동일해야 한다.
	int VertexCnt = _mesh->GetControlPointsCount();
	_dstData->SkinningList.resize(VertexCnt);

	// Skinning: 정점에 영향을 주는 행렬들을 찾는 과정.
	int DeformerCnt = _mesh->GetDeformerCount(FbxDeformer::EDeformerType::eSkin);
	for (int idx = 0; idx < DeformerCnt; idx++)
	{
		// Deformer(리깅 도구): 뼈대에 스킨을 붙이는 작업 도구.
		FbxDeformer* pDeformer = _mesh->GetDeformer(idx, FbxDeformer::EDeformerType::eSkin);
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
				_dstData->SkinningList[VertexIndex].insert(Weight, LinkedNodeName);
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
			_dstData->BindPoseMap.insert(std::make_pair(LinkedNodeName, matInvBindPose)); // 상수 버퍼 적용 전에 곱해 주고
		}
		
	}

	int BindPoseIdx = 0;
	for (auto it : _dstData->BindPoseMap)
	{
		_dstData->BindPoseKeyToIndexMap.insert(std::make_pair(it.first, BindPoseIdx++));
	}

	return true;
}

bool FBXLoader::ParseDummy(FbxNull* _dummy, FBXFileData* _dst)
{
	if ((_dummy == nullptr) || (_dst == nullptr))
	{
		return false;
	}

	_dst->DummyList.push_back(_dummy);

	return true;
}

bool FBXLoader::ParseSkeleton(FbxSkeleton* _skeleton, FBXFileData* _dst)
{
	if ((_skeleton == nullptr) || (_dst == nullptr))
	{
		return false;
	}

	_dst->SkeletonList.push_back(_skeleton);

	return true;
}

bool FBXLoader::ReadTextureCoord(FbxLayerElementUV* _uv, int _vertexIdx, int _uvIdx, FbxVector2& _dst)
{
	if (_uv == nullptr || _dst == nullptr)
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
	switch (_uv->GetMappingMode())
	{
		case FbxLayerElementUV::eByControlPoint:
		{
			// 정점 마다 UV가 1개씩 있으니 해당 Vertex에서 정보를 가져옴.
			switch (_uv->GetReferenceMode())
			{
				case FbxLayerElementUV::eDirect:
				{
					FbxVector2 fbxUv = _uv->GetDirectArray().GetAt(_vertexIdx); 
					_dst.mData[0] = fbxUv.mData[0];
					_dst.mData[1] = fbxUv.mData[1];
					break;
				}
				case FbxLayerElementUV::eIndexToDirect:
				{
					int id = _uv->GetIndexArray().GetAt(_vertexIdx);
					FbxVector2 fbxUv = _uv->GetDirectArray().GetAt(id);
					_dst.mData[0] = fbxUv.mData[0];
					_dst.mData[1] = fbxUv.mData[1];
					break;
				}
			}
			break;
		}
		case FbxLayerElementUV::eByPolygonVertex:
		{
			// 폴리곤에 하나 있으니 다이렉트로 가져와도 된다.
			switch (_uv->GetReferenceMode())
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

					FbxVector2 fbxUv = _uv->GetDirectArray().GetAt(_uvIdx); // 다이렉트로 가져오면 됨.
					_dst.mData[0] = fbxUv.mData[0];
					_dst.mData[1] = fbxUv.mData[1];
					break;
				}
			}
			break;
		}
		
	}

	return true;
}

bool FBXLoader::ReadColorCoord(FbxLayerElementVertexColor* _color, int _vertexIdx, int _colorIdx, FbxColor& _dst)
{
	if (_color == nullptr)
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
	switch (_color->GetMappingMode())
	{
	case FbxLayerElementUV::eByControlPoint:
	{
		// 정점 마다 UV가 1개씩 있으니 해당 Vertex에서 정보를 가져옴.
		switch (_color->GetReferenceMode())
		{
		case FbxLayerElementUV::eDirect:
		{
			_dst = _color->GetDirectArray().GetAt(_vertexIdx);
			break;
		}
		case FbxLayerElementUV::eIndexToDirect:
		{
			int id = _color->GetIndexArray().GetAt(_vertexIdx);
			_dst = _color->GetDirectArray().GetAt(id);
			break;
		}
		}
		break;
	}
	case FbxLayerElementUV::eByPolygonVertex:
	{
		// 폴리곤에 하나 있으니 다이렉트로 가져와도 된다.
		switch (_color->GetReferenceMode())
		{
		case FbxLayerElementUV::eDirect:
			//{
			//	FbxVector2 fbxUv = _uv->GetDirectArray().GetAt(_uvIdx);
			//	_dst.mData[0] = fbxUv.mData[0];
			//	_dst.mData[1] = fbxUv.mData[1];
			//	break;
			//}
		{
			_dst = _color->GetDirectArray().GetAt(_colorIdx); // 다이렉트로 가져오면 됨.
			break;
		}
		case FbxLayerElementUV::eIndexToDirect:
		{
			//int id = _uv->GetIndexArray().GetAt(_uvIdx);
			//FbxVector2 fbxUv = _uv->GetDirectArray().GetAt(id);
			//_dst.mData[0] = fbxUv.mData[0];
			//_dst.mData[1] = fbxUv.mData[1];

			//_dst = _color->GetDirectArray().GetAt(_colorIdx); // 다이렉트로 가져오면 됨.
			int id = _color->GetIndexArray().GetAt(_colorIdx);
			_dst = _color->GetDirectArray().GetAt(id);
			break;
		}
		}
		break;
	}

	}

	return true;
}

bool FBXLoader::ReadNormal(FbxLayerElementNormal* _normal, int _vertexIdx, int _normalIdx, FbxVector4& _dst)
{
	if (_normal == nullptr)
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
	switch (_normal->GetMappingMode())
	{
	case FbxLayerElementUV::eByControlPoint:
	{
		// 정점 마다 UV가 1개씩 있으니 해당 Vertex에서 정보를 가져옴.
		switch (_normal->GetReferenceMode())
		{
		case FbxLayerElementUV::eDirect:
		{
			_dst = _normal->GetDirectArray().GetAt(_vertexIdx);
			break;
		}
		case FbxLayerElementUV::eIndexToDirect:
		{
			int id = _normal->GetIndexArray().GetAt(_vertexIdx);
			_dst = _normal->GetDirectArray().GetAt(id);
			break;
		}
		}
		break;
	}
	case FbxLayerElementUV::eByPolygonVertex:
	{
		// 폴리곤에 하나 있으니 다이렉트로 가져와도 된다.
		switch (_normal->GetReferenceMode())
		{
		case FbxLayerElementUV::eDirect:
			//{
			//	FbxVector2 fbxUv = _uv->GetDirectArray().GetAt(_uvIdx);
			//	_dst.mData[0] = fbxUv.mData[0];
			//	_dst.mData[1] = fbxUv.mData[1];
			//	break;
			//}
		{
			_dst = _normal->GetDirectArray().GetAt(_normalIdx); // 다이렉트로 가져오면 됨.
			break;
		}
		case FbxLayerElementUV::eIndexToDirect:
		{
			//int id = _uv->GetIndexArray().GetAt(_uvIdx);
			//FbxVector2 fbxUv = _uv->GetDirectArray().GetAt(id);
			//_dst.mData[0] = fbxUv.mData[0];
			//_dst.mData[1] = fbxUv.mData[1];

			//_dst = _color->GetDirectArray().GetAt(_colorIdx); // 다이렉트로 가져오면 됨.
			int id = _normal->GetIndexArray().GetAt(_normalIdx);
			_dst = _normal->GetDirectArray().GetAt(id);
			break;
		}
		}
		break;
	}

	}

	return true;
}

bool FBXLoader::ReadTangent(FbxLayerElementTangent* _tangent, int _vertexIdx, int _tangentIdx, FbxVector4& _dst)
{
	if (_tangent == nullptr)
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
	switch (_tangent->GetMappingMode())
	{
	case FbxLayerElementUV::eByControlPoint:
	{
		// 정점 마다 UV가 1개씩 있으니 해당 Vertex에서 정보를 가져옴.
		switch (_tangent->GetReferenceMode())
		{
		case FbxLayerElementUV::eDirect:
		{
			_dst = _tangent->GetDirectArray().GetAt(_vertexIdx);
			break;
		}
		case FbxLayerElementUV::eIndexToDirect:
		{
			int id = _tangent->GetIndexArray().GetAt(_vertexIdx);
			_dst = _tangent->GetDirectArray().GetAt(id);
			break;
		}
		}
		break;
	}
	case FbxLayerElementUV::eByPolygonVertex:
	{
		// 폴리곤에 하나 있으니 다이렉트로 가져와도 된다.
		switch (_tangent->GetReferenceMode())
		{
		case FbxLayerElementUV::eDirect:
			//{
			//	FbxVector2 fbxUv = _uv->GetDirectArray().GetAt(_uvIdx);
			//	_dst.mData[0] = fbxUv.mData[0];
			//	_dst.mData[1] = fbxUv.mData[1];
			//	break;
			//}
		{
			_dst = _tangent->GetDirectArray().GetAt(_tangentIdx); // 다이렉트로 가져오면 됨.
			break;
		}
		case FbxLayerElementUV::eIndexToDirect:
		{
			//int id = _uv->GetIndexArray().GetAt(_uvIdx);
			//FbxVector2 fbxUv = _uv->GetDirectArray().GetAt(id);
			//_dst.mData[0] = fbxUv.mData[0];
			//_dst.mData[1] = fbxUv.mData[1];

			//_dst = _color->GetDirectArray().GetAt(_colorIdx); // 다이렉트로 가져오면 됨.
			int id = _tangent->GetIndexArray().GetAt(_tangentIdx);
			_dst = _tangent->GetDirectArray().GetAt(id);
			break;
		}
		}
		break;
	}

	}

	return true;
}

int FBXLoader::getSubMaterialIndex(FbxLayerElementMaterial* _material, int _polyIdx)
{
	// 매핑방식
	//eNone,
	//eByControlPoint,  // 제어점
	//eByPolygonVertex, //  
	//eByPolygon, // 폴리곤마다 다를수 있다.
	//eAllSame - 전체표면에 1개의 매핑좌표가 있다.
	int iSubMtrl = 0;
	if (_material != nullptr)
	{
		switch (_material->GetMappingMode())
		{
		case FbxLayerElement::eByPolygon:
		{
			// 매핑 정보가 배열에 저장되는 방식
			switch (_material->GetReferenceMode())
			{
			case FbxLayerElement::eIndex:
			{
				iSubMtrl = _polyIdx;
			}break;
			case FbxLayerElement::eIndexToDirect:
			{
				iSubMtrl = _material->GetIndexArray().GetAt(_polyIdx);
			}break;
			}
		}
		default:
		{
			//break;
		}
		}
	}
	return iSubMtrl;
}

DXTexture* FBXLoader::FindTexture(FbxSurfaceMaterial* _surface, const char* _name, std::wstring* _rst)
{
	if (_surface == nullptr)
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

	// FbxSurfaceMaterial
	// 텍스처 정보를 가져오기 위한 것. 보통 1개의 Surface에 24개 이상의 텍스쳐가 붙어 있다.(24종 이상의 텍스쳐 방식이 존재한다.)
	// 텍스쳐 맵을 가지고 있다(ex. 마스크 텍스처처럼 알파를 가진 놈들 등 여러가지 종류가 있음.)
	std::string textureName;
	auto prop = _surface->FindProperty(_name); 
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
				std::wstring wstrPath = m_wstrResourceDir;
				wstrPath += file;
				if (_rst != nullptr)
				{
					_rst->assign(wstrPath.begin(), wstrPath.end());
				}

				if (DXTextureManager::getInstance()->Load(wstrPath))
				{
					DXTexture* pTexture = DXTextureManager::getInstance()->getTexture(wstrPath);
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

std::wstring FBXLoader::getTextureFileName(FbxSurfaceMaterial* _surface, const char* _name)
{
	if (_surface == nullptr)
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
	auto prop = _surface->FindProperty(_name);
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

FbxAMatrix FBXLoader::getGeometryMatrix(FbxNode* _node)
{
	// 로컬 행렬
	// Affine 행렬은 정점 변환 할 때, 그냥 행렬은 행렬 끼리 연산 할 때 사용
	FbxAMatrix geometryMatrix; // 기하 행렬. 초기 정점 위치를 변환 할 때 사용. 로컬 행렬. 이 매트릭스를 적용해 바뀌지 않으면 월드 행렬에서 변환. 정점 변환.
	FbxVector4 translation = _node->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 rotation = _node->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 scale = _node->GetGeometricScaling(FbxNode::eSourcePivot);
	geometryMatrix.SetT(translation);
	geometryMatrix.SetR(rotation);
	geometryMatrix.SetS(scale);

	return geometryMatrix;
}

FbxAMatrix FBXLoader::getNormalMatrix(const FbxAMatrix& _src)
{
	// 노말 성분은 역행렬의 전치 행렬로 곱해 줘야 함.
	FbxAMatrix normalMatrix = _src;
	normalMatrix = normalMatrix.Inverse();
	normalMatrix = normalMatrix.Transpose();
	return normalMatrix;
}

FbxAMatrix FBXLoader::getWorldMatrix(FbxNode* _node)
{
	// 월드 행렬, 애니메이션에서 사용 하면 안됨. 원래는 밖에서 사용해야 하나 임시로 사용
	FbxVector4 translation_World;
	if (_node->LclTranslation.IsValid())
	{
		translation_World = _node->LclTranslation;
	}

	FbxVector4 rotation_World;
	if (_node->LclRotation.IsValid())
	{
		rotation_World = _node->LclRotation;
	}

	FbxVector4 scale_World;
	if (_node->LclScaling.IsValid())
	{
		scale_World = _node->LclScaling;
	}

	FbxAMatrix worldMatrix;
	worldMatrix.SetT(translation_World);
	worldMatrix.SetR(rotation_World);
	worldMatrix.SetS(scale_World);

	return worldMatrix;
}

FBXObject* FBXLoader::getObject(std::wstring _key)
{
	auto it = m_ObjectMap.find(_key);
	if (it != m_ObjectMap.end())
	{
		return it->second.get();
	}

	return nullptr;
}

void FBXLoader::setResourceDirectory(std::wstring _dir)
{
	m_wstrResourceDir = _dir;
}

Matrix FBXLoader::toMatrix4x4(const FbxAMatrix& _src)
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
	Matrix src = toMatrix4x4(_src);
	rst._11 = src._11; rst._12 = src._13; rst._13 = src._12;
	rst._21 = src._31; rst._22 = src._33; rst._23 = src._32;
	rst._31 = src._21; rst._32 = src._23; rst._33 = src._22;
	rst._41 = src._41; rst._42 = src._43; rst._43 = src._42;
	rst._14 = rst._24 = rst._34 = 0.0f;
	rst._44 = 1.0f;

	return rst;
}

bool FBXLoader::GenerateAnimationTrack(FBXFileData* _data)
{
	FbxTime time;
	UINT StartFrame = _data->AnimationSceneInfo.StartFrame;
	UINT EndFrame = _data->AnimationSceneInfo.EndFrame;
	FbxTime::EMode TimeMode = _data->AnimationSceneInfo.TimeMode;
	std::vector<std::vector<FBXAnimationTrack>> TrackList;
	size_t NodeNum = _data->NodeList.size();
	TrackList.resize(NodeNum);
	for (UINT t = StartFrame; t <= EndFrame; t++)
	{
		time.SetFrame(t, TimeMode); // 이게 시간을 많이 잡아먹어서 최대한 적게 호출하는게 좋다.
		for (size_t NodeIdx = 0; NodeIdx < NodeNum; NodeIdx++)
		{
			FbxNode* currentNode = _data->NodeList[NodeIdx];
			FBXAnimationTrack Track;
			Track.frame = t;
			FbxAMatrix fbxMatrix = currentNode->EvaluateGlobalTransform(time);
			Track.matAnimation = ConvertToDxMatrix(fbxMatrix);
			Matrix4x4Decompose(Track.matAnimation, Track.scale, Track.rotation, Track.translation);

			TrackList[NodeIdx].push_back(Track);
		}
	}

	for (size_t NodeIdx = 0; NodeIdx < NodeNum; NodeIdx++)
	{
		// Original Animation Track List Map.
		FbxNode* currentNode = _data->NodeList[NodeIdx];
		std::string NodeName = currentNode->GetName();
		_data->AnimationTrackMap.insert(std::make_pair(NodeName, TrackList[NodeIdx]));

		// Generate Matrix List Map of Interpolation Animation.
		size_t TrackSize = TrackList[NodeIdx].size();
		std::vector<Matrix> InterpolationMatrixList;
		//InterpolationMatrixList.resize(TrackSize * _data->InterpolationSampling);
		for (size_t FrameIdx = 0; FrameIdx < TrackSize; FrameIdx++)
		{
			FBXAnimationTrack A, B;
			UINT FrameA = max(FrameIdx + 0, _data->AnimationSceneInfo.StartFrame);
			UINT FrameB = min(FrameIdx + 1, _data->AnimationSceneInfo.EndFrame);
			A = TrackList[NodeIdx][FrameA];
			B = TrackList[NodeIdx][FrameB];
			if (A.frame == B.frame) // End Frame
			{
				//InterpolationMatrixList[FrameIdx] = TrackList[NodeIdx][FrameIdx].matAnimation;
				InterpolationMatrixList.push_back(TrackList[NodeIdx][FrameIdx].matAnimation);
				continue;
			}

			//float t = (FrameIdx - A.frame) / (B.frame - A.frame);
			float tick = 1.0f / _data->InterpolationSampling; // Sampling 수 만큼 보간.
			for (float t = 0.0f; t < 1.0f; t += tick)
			{
				Vector3 translation = LinearInterpolation(A.translation, B.translation, t);
				Vector3 scale = LinearInterpolation(A.scale, B.scale, t);
				Matrix matScale;
				//matScale = matScale.Identity();
				//matScale.Identity();
				matScale._11 = scale.x;
				matScale._22 = scale.y;
				matScale._33 = scale.z;
				Vector4 qRotation = SphereLinearInterpolation(A.rotation, B.rotation, t);
				Matrix matRotation = QuaternionToMatrix4x4(qRotation);
				Matrix rst = matScale * matRotation;
				rst._41 = translation.x;
				rst._42 = translation.y;
				rst._43 = translation.z;

				//InterpolationMatrixList[FrameIdx] = rst;
				InterpolationMatrixList.push_back(rst);
			}
		}
		_data->InterpolationFrameMatrixList.insert(std::make_pair(NodeName, InterpolationMatrixList));
	}

	return true;
}

bool FBXLoader::GenerateObjectFromFileData(FBXFileData* _src, FBXObject* _dst)
{
	if (_src == nullptr)
	{
		return false;
	}

	FBXFileData* pData = _src;

	for (auto it : pData->NodeDataList)
	{
		if (it.AttributeType != FbxNodeAttribute::EType::eMesh)
		{
			continue;
		}
		else
		{
			FBXObject* pObject = new FBXObject;
			pObject->m_strNodeName = it.Name;
			pObject->m_animationSceneInfo = pData->AnimationSceneInfo;
			pObject->BindPoseMap.insert(it.BindPoseMap.begin(), it.BindPoseMap.end());
			pObject->BindPoseKeyToIndexMap.insert(it.BindPoseKeyToIndexMap.begin(), it.BindPoseKeyToIndexMap.end());
			pObject->FileData = pData;
			for (auto material : it.Materials)
			{
				if (material.isValid())
				{
					pObject->Materials.push_back(material);
				}
			}

			for (auto& material : pObject->Materials)
			{
				/*material.create();
				if (DXTextureManager::getInstance()->Load(m_wstrResourceDir + material.DiffuseTexture))
				{
					DXTexture* pTexture = DXTextureManager::getInstance()->getTexture(m_wstrResourceDir + material.DiffuseTexture);
					if (pTexture != nullptr)
					{
						material.setTexture(pTexture);
					}
				}*/
			}

			_dst->child.push_back(pObject);
		}
	}

	return true;
}

bool FBXLoader::GenerateStaticMeshFromFileData(FBXFileData* _src, StaticMeshComponent* _dst)
{
	if (_src == nullptr)
	{
		return false;
	}

	FBXFileData* pData = _src;
	for (auto it : pData->NodeDataList)
	{
		if (it.AttributeType != FbxNodeAttribute::EType::eMesh)
		{
			continue;
		}
		else
		{
			if (it.Materials.empty())
			{
				continue;
			}

			size_t materialCnt = it.Materials.size();
			_dst->Meshes.resize(materialCnt);
			for (size_t idx = 0; idx < materialCnt; idx++)
			{
				auto material = it.Materials[idx];
				_dst->Meshes[idx].Vertices.assign(material.VertexList.begin(), material.VertexList.end());
				_dst->Meshes[idx].Indices.assign(material.IndexList.begin(), material.IndexList.end());
				if (DXTextureManager::getInstance()->Load(m_wstrResourceDir + material.DiffuseTexture))
				{
					DXTexture* pTexture = DXTextureManager::getInstance()->getTexture(m_wstrResourceDir + material.DiffuseTexture);
					if (pTexture != nullptr)
					{
						_dst->Meshes[idx].MaterialSlot = new Material;
						_dst->Meshes[idx].MaterialSlot->AddTexture(pTexture);
					}
				}
				else
				{
					_dst->Meshes[idx].MaterialSlot = nullptr;
				}
				
			}
			
		}
	}

	return true;
}
