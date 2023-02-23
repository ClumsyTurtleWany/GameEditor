#include "pch.h"
#include "FBXLoader.hpp"

bool FBXLoader::initialize()
{
	m_pManager = FbxManager::Create(); // static���� ���� ��.
	if (m_pManager != nullptr)
	{
		m_pImporter = FbxImporter::Create(m_pManager, ""); // �����ڵ� ��� ����. static���� ���� ��.
		if (m_pImporter == nullptr)
		{
			OutputDebugString(L"WanyCore::FBXLoader::initialize::Failed Create Fbx Importer.\n");
			return false;
		}

		return true;
	}
	else
	{
		OutputDebugString(L"WanyCore::FBXLoader::initialize::Failed Create Fbx Manager.\n");
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
		OutputDebugString(L"WanyCore::FBXLoader::Load::Failed Initialize Importer.\n");
		return false;
	}

	// �⺻������ ������ ���ϵ��� Scene ������ �����.
	// Scene�� Ʈ�� ������ �̷���� �־� Root���� ���۵Ǹ� ���� Root�� NULL�� �̷��������.
	// ���� ���� ���� ���� �����ؼ� �ε� ���� �ʿ䰡 ����.

	FbxScene* pScene = FbxScene::Create(m_pManager, "");
	if (pScene == nullptr)
	{
		OutputDebugString(L"WanyCore::FBXLoader::Load::Failed Create Scene.\n");
		return false;
	}

	if (!m_pImporter->Import(pScene))
	{
		OutputDebugString(L"WanyCore::FBXLoader::Load::Failed Import Scene.\n");
		return false;
	}

	FBXFileData fbxFile;
	if (!ParseScene(pScene, &fbxFile))
	{
		OutputDebugString(L"WanyCore::FBXLoader::Load::Failed Parse Scene.\n");
		return false;
	}

	FbxNode* pRoot = pScene->GetRootNode();
	if (!ParseNode(pRoot, &fbxFile))
	{
		OutputDebugString(L"WanyCore::FBXLoader::Load::Failed Parse Root Node.\n");
		return false;
	}

	if (!PreProcess(&fbxFile))
	{
		OutputDebugString(L"WanyCore::FBXLoader::Load::Failed Pre Process.\n");
		return false;
	}

	if (!GenerateObjectFromFileData(&fbxFile, _dst))
	{
		OutputDebugString(L"WanyCore::FBXLoader::Load::Failed Generate FBX Object.\n");
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
		OutputDebugString(L"WanyCore::FBXLoader::Load::Failed Initialize Importer.\n");
		return false;
	}

	// �⺻������ ������ ���ϵ��� Scene ������ �����.
	// Scene�� Ʈ�� ������ �̷���� �־� Root���� ���۵Ǹ� ���� Root�� NULL�� �̷��������.
	// ���� ���� ���� ���� �����ؼ� �ε� ���� �ʿ䰡 ����.

	FbxScene* pScene = FbxScene::Create(m_pManager, "");
	if (pScene == nullptr)
	{
		OutputDebugString(L"WanyCore::FBXLoader::Load::Failed Create Scene.\n");
		return false;
	}

	if (!m_pImporter->Import(pScene))
	{
		OutputDebugString(L"WanyCore::FBXLoader::Load::Failed Import Scene.\n");
		return false;
	}

	FBXFileData fbxFile;
	if (!ParseScene(pScene, &fbxFile))
	{
		OutputDebugString(L"WanyCore::FBXLoader::Load::Failed Parse Scene.\n");
		return false;
	}

	FbxNode* pRoot = pScene->GetRootNode();
	if (!ParseNode(pRoot, &fbxFile))
	{
		OutputDebugString(L"WanyCore::FBXLoader::Load::Failed Parse Root Node.\n");
		return false;
	}

	if (!PreProcess(&fbxFile))
	{
		OutputDebugString(L"WanyCore::FBXLoader::Load::Failed Pre Process.\n");
		return false;
	}

	if (!GenerateStaticMeshFromFileData(&fbxFile, _dst))
	{
		OutputDebugString(L"WanyCore::FBXLoader::Load::Failed Generate FBX Object.\n");
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

	// �� FBX�� ����� ��(3D Max, ���� �� ������ ��)���� ���� ��� ������ �ٸ� �� �ֱ� ������ ���� �� �ε� �� �� �ִ�.
	// ������ FBX�� �⺻������ ������ ��ǥ��, Y-Up
	//FbxSystemUnit::cm.ConvertScene(_scene); // ���� ����. cm ������ ����
	FbxSystemUnit::m.ConvertScene(_scene); // ���� ����. m ������ ����
	//FbxAxisSystem::MayaZUp.ConvertScene(pScene); // ���� �� ����. ���� �������� ����. ���� ��ȯ�� �ƴ� ��Ŀ� ���� �Ǵ� ����.
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

	//FbxAxisSystem::DirectX.ConvertScene(pScene); // ���� �� ����. DirectX �������� �ٲ㵵 ���� �츮�� ����ϴ� ��� �ٸ�. (Right Vector�� ���̳ʽ���.).
	//FbxAxisSystem sceneAxisSystemAfter = pScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem::MayaZUp.ConvertScene(_scene); // ���� �� ����. ���� �������� ����. ���� ��ȯ�� �ƴ� ��Ŀ� ���� �Ǵ� ����.


	// Animation Scene Information
	FbxAnimStack* pStack = _scene->GetSrcObject<FbxAnimStack>(0);
	FbxLongLong s = 0;
	FbxLongLong n = 0;
	FbxTime::EMode TimeMode = FbxTime::GetGlobalTimeMode();
	if (pStack != nullptr)
	{
		FbxTime::SetGlobalTimeMode(FbxTime::eFrames30);
		//FbxTime::EMode TimeMode = FbxTime::GetGlobalTimeMode();
		FbxTimeSpan localTimeSpan = pStack->GetLocalTimeSpan();// �ð� ����. ������ ����
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
		OutputDebugString(L"WanyCore::FBXLoader::ParseNode::Node or Dest is nullptr.\n");
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
		// eNull, // ���� ������Ʈ�� �̿� �� ���� ����.
		// eMarker,
		// eSkeleton, // �ִϸ��̼ǿ� ������.
		// eMesh, // �������� ������.
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
			case FbxNodeAttribute::EType::eNull: // ���� Root ���� NULL�� ����.
			{
				isValid = true;
				//FbxNull* pDummy = _node->GetNull();
				NodeData.Dummy = _node->GetNull();
				if (NodeData.Dummy != nullptr)
				{
					// Dummy: �ڽ� ������Ʈ�� ������ �θ� ������Ʈ�� ������ �����ֱ� ���� ����.
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
					// Skeleton: �ִϸ��̼��� ���� ����
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
					// Mesh: ���� ������ ������
					// Scene graph ����(Ʈ���� ��� ������ �� �־ ���� �� ��� �� �������ϴ� ���) �̶�� �θ�.
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

		// ���� ���
		// Affine ����� ���� ��ȯ �� ��, �׳� ����� ��� ���� ���� �� �� ���
		NodeData.LocalGeometryMatrix = getGeometryMatrix(_node); // ���� ���. �ʱ� ���� ��ġ�� ��ȯ �� �� ���. ���� ���. �� ��Ʈ������ ������ �ٲ��� ������ ���� ��Ŀ��� ��ȯ. ���� ��ȯ.
		_dst->LocalGeometryMatrixMap.insert(std::make_pair(NodeData.Name, NodeData.LocalGeometryMatrix));

		// �븻 ������ ������� ��ġ ��ķ� ���� ��� ��.
		NodeData.LocalNormalMatrix = getNormalMatrix(NodeData.LocalGeometryMatrix);
		_dst->NormalMatrixMap.insert(std::make_pair(NodeData.Name, NodeData.LocalGeometryMatrix));


		// �� ���̽� ���� �ٸ� �ؽ��ĸ� ��� �� �� �ִ�. �̰��� ���� ���׸��� �̶�� ��. (1���� ������Ʈ�� �������� �ؽ��� ���)
		// ���� ���׸����� ������ �ϱ� ���ؼ� ���� �ؽ��ĸ� ����ϴ� ���̽����� ��� ���.
		NodeData.MaterialNum = _node->GetMaterialCount(); // �ؽ��İ� ���� ����.
		for (int idx = 0; idx < NodeData.MaterialNum; idx++)
		{
			FbxMaterial material;
			// �ؽ�ó ������ �������� ���� ��. ���� 1���� Surface�� 24�� �̻��� �ؽ��İ� �پ� �ִ�.(24�� �̻��� �ؽ��� ����� �����Ѵ�.)
			// �ؽ��� ���� ������ �ִ�(ex. ����ũ �ؽ�óó�� ���ĸ� ���� ��� �� �������� ������ ����.)
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

	int childCount = _node->GetChildCount(); // Child ������ 0�̸� ���� �Ž�, 0�� �ƴϸ� ���� �Ž��� �� �� ����.
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

	// Layer ���� �ʿ�. �������� ���� ������ ���� ������ �ϴ°� == ��Ƽ �н� ������. �ؽ��ķ� ġȯ�ϸ� ��Ƽ �ؽ�ó ������.
	std::vector<FbxLayerElementUV*> UVList;
	std::vector<FbxLayerElementVertexColor*> ColorList;
	std::vector<FbxLayerElementMaterial*> MaterialList;
	std::vector<FbxLayerElementNormal*> NormalList;
	
	int layerCount = _mesh->GetLayerCount();
	//meshData.LayerList.resize(layerCount);
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

		//meshData.LayerList[layerIdx].ElementUV = pUV;
		//meshData.LayerList[layerIdx].ElementColor = pColor;
		//meshData.LayerList[layerIdx].ElementMaterialList = pMaterial;
		//meshData.LayerList[layerIdx].ElementNormalList = pNormal;
	}

	//FbxNode* pNode = _mesh->GetNode(); // ������ Mesh�� �ƴ� Node�� �����°� �´�.  
	//// �� ���̽� ���� �ٸ� �ؽ��ĸ� ��� �� �� �ִ�. �̰��� ���� ���׸��� �̶�� ��. (1���� ������Ʈ�� �������� �ؽ��� ���)
	//// ���� ���׸����� ������ �ϱ� ���ؼ� ���� �ؽ��ĸ� ����ϴ� ���̽����� ��� ���.
	//int MaterialCnt = pNode->GetMaterialCount(); // �ؽ��İ� ���� ����.
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
	//	// �ؽ�ó ������ �������� ���� ��. ���� 1���� Surface�� 24�� �̻��� �ؽ��İ� �پ� �ִ�.(24�� �̻��� �ؽ��� ����� �����Ѵ�.)
	//	// �ؽ��� ���� ������ �ִ�(ex. ����ũ �ؽ�óó�� ���ĸ� ���� ��� �� �������� ������ ����.)
	//	FbxSurfaceMaterial* pSurface = pNode->GetMaterial(idx);
	//	if (pSurface != nullptr)
	//	{
	//		_dst->Materials[idx].DiffuseTexture = getTextureFileName(pSurface, FbxSurfaceMaterial::sDiffuse);
	//	}
	//}


	//// ���� ���
	//// Affine ����� ���� ��ȯ �� ��, �׳� ����� ��� ���� ���� �� �� ���
	//FbxAMatrix geometryMatrix = getGeometryMatrix(pNode); // ���� ���. �ʱ� ���� ��ġ�� ��ȯ �� �� ���. ���� ���. �� ��Ʈ������ ������ �ٲ��� ������ ���� ��Ŀ��� ��ȯ. ���� ��ȯ.

	//// �븻 ������ ������� ��ġ ��ķ� ���� ��� ��.
	//FbxAMatrix normalMatrix = getNormalMatrix(geometryMatrix);


	size_t LayerIdx = 0;
	int polyCount = _mesh->GetPolygonCount();
	// 3���� = 1������(Triangle) �� ���� �ְ�
	// 4���� = 1������(Quad) �� ���� �ִ�.
	// ������ -> ���̽� -> ����
	int basePolyIdx = 0; // Color
	int MaterialIdx = 0;
	FbxVector4* pVertexPosition = _mesh->GetControlPoints(); // ������(Control point == Vertices). ������ ���� ��ġ. 
	for (int polyIdx = 0; polyIdx < polyCount; polyIdx++)
	{
		int polySize = _mesh->GetPolygonSize(polyIdx);
		int faceCount = polySize - 2;
		if (!MaterialList.empty())
		{
			MaterialIdx = getSubMaterialIndex(MaterialList[LayerIdx], polyIdx);
		}

		for (int faceIdx = 0; faceIdx < faceCount; faceIdx++)
		{
			// FBX�� �ð� �ݴ� ������. DirectX�� ��ǥ�� ������ �ٸ���. 
			// Max ��ǥ�� Y���� Z��, Z���� Y��, (DX.x == Max.x, DX.y = Max.z, DX.z = Max.y)
			// DX ���� 		Max ����
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
				vertex = _dstData->LocalGeometryMatrix.MultT(vertex); // ���� ��� ��ȯ, Transform�� T, �� �켱 ���
				//vertex = worldMatrix.MultT(vertex); // ���� ��ȯ ���. ���߿� �� ��.
				//vertex = matGlobalTransform.MultT(vertex);

				Vector3 pos;
				pos.x = vertex.mData[0];
				pos.y = vertex.mData[2];
				pos.z = vertex.mData[1];

				Vector3 normal;
				Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
				Vector2 texture;
				FbxVector2 tex; // = ;
				int uvidx = UVidx[idx];
				if (!UVList.empty())
				{
					if (ReadTextureCoord(UVList[LayerIdx], vertexIdx, uvidx, tex))
					{
						texture.x = tex.mData[0];
						texture.y = 1.0f - tex.mData[1];
					}
				}

				if (!ColorList.empty())
				{
					int colorIdx = basePolyIdx + vertexColorIdx[idx];
					FbxColor fbxColor;
					if (ReadColorCoord(ColorList[LayerIdx], vertexIdx, colorIdx, fbxColor))
					{
						color.x = fbxColor.mRed;
						color.y = fbxColor.mGreen;
						color.z = fbxColor.mBlue;
						//color.w = fbxColor.mAlpha;
						color.w = 1.0f;
					}
				}

				if (!NormalList.empty())
				{
					int normalIdx = basePolyIdx + vertexColorIdx[idx];
					FbxVector4 fbxNormal;
					if (ReadNormal(NormalList[LayerIdx], vertexIdx, normalIdx, fbxNormal))
					{
						fbxNormal = _dstData->LocalNormalMatrix.MultT(fbxNormal);
						//fbxNormal = normalMatrix_World.MultT(fbxNormal); // ���� ��ȯ. ���߿� �� ��.
						//fbxNormal = matNormalGlobal.MultT(fbxNormal);
						normal.x = fbxNormal.mData[0];
						normal.y = fbxNormal.mData[2];
						normal.z = fbxNormal.mData[1];
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

	// Layer ���� �ʿ�. �������� ���� ������ ���� ������ �ϴ°� == ��Ƽ �н� ������. �ؽ��ķ� ġȯ�ϸ� ��Ƽ �ؽ�ó ������.
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
	// VertexCnt == �޽��� ���� ������ �����ؾ� �Ѵ�.
	int VertexCnt = _mesh->GetControlPointsCount();
	_dstData->SkinningList.resize(VertexCnt);

	// Skinning: ������ ������ �ִ� ��ĵ��� ã�� ����.
	int DeformerCnt = _mesh->GetDeformerCount(FbxDeformer::EDeformerType::eSkin);
	for (int idx = 0; idx < DeformerCnt; idx++)
	{
		// Deformer(���� ����): ���뿡 ��Ų�� ���̴� �۾� ����.
		FbxDeformer* pDeformer = _mesh->GetDeformer(idx, FbxDeformer::EDeformerType::eSkin);
		FbxSkin* pSkin = reinterpret_cast<FbxSkin*>(pDeformer);

		FbxGeometry* pGeometry = pSkin->GetGeometry();
		int ClusterCnt = pSkin->GetClusterCount(); // ClusterCnt ���� Bone�� VertexCnt ����ŭ�� ������ ������ �ش�.
		for (int clusterIdx = 0; clusterIdx < ClusterCnt; clusterIdx++)
		{
			// Cluster: ���뿡 ���̴� ���(��, Sphere), Mesh�� ���� ���. ���밡 ������ ��ġ�� ���� ���.
			FbxCluster* pCluster = pSkin->GetCluster(clusterIdx);

			FbxCluster::ELinkMode LinkMode = pCluster->GetLinkMode();
			FbxNode* pLinkNode = pCluster->GetLink(); // Bone���� ���� ��.
			std::string LinkedNodeName = pLinkNode->GetName();

			FbxNodeAttribute::EType attributeType;
			FbxNodeAttribute* pAttribute = pLinkNode->GetNodeAttribute();
			if (pAttribute != nullptr)
			{
				attributeType = pAttribute->GetAttributeType();
			}

			int BoneIndex = 0;// = pLinkNode �� Bone ������; // ������Ʈ���� ã�Ƽ� �־���� ��.

			// � ������ ������ �޴��� üũ
			int* indices = pCluster->GetControlPointIndices();
			double* weights = pCluster->GetControlPointWeights();
			int clusterSize = pCluster->GetControlPointIndicesCount(); // �ϳ��� ����(BoneIndex�� ���)�� ������ �ִ� Vertex �� ��.
			for (int vertexIdx = 0; vertexIdx < clusterSize; vertexIdx++)
			{
				// �� �������� ������ �־�� ��. ������ 4���� �ѱ⵵�� �߱� ������ Weight�� ū ������ ��� �� �־�� ��.
				// ���� ���밡 ������ �ִ� ������ ����ġ. VertexIndex�� ������ Weight��ŭ BoneIndex ����� ������ �ش�.
				int VertexIndex = indices[vertexIdx];
				float Weight = static_cast<float>(weights[vertexIdx]);

				//_dst->SkinningList[VertexIndex].setKey(LinkedNodeName);
				//_dst->SkinningList[VertexIndex].insert(BoneIndex, Weight);
				_dstData->SkinningList[VertexIndex].insert(Weight, LinkedNodeName);
			}

			// ���� �������� ��ȯ�ϴ� ����� �ʿ��ϴ�.
			// Mesh�� �������� ���� ��� ������ �ֱ� ������(�ִϸ��̼�) 
			// Skin Mesh�� Bone�� ��Ż���� �ʰ� �ϱ� ���ؼ��� Bone�������� ��ȯ�� �ʿ���.
			FbxAMatrix matXBindPose;
			FbxAMatrix matRefGlobalInitPosition;
			pCluster->GetTransformLinkMatrix(matXBindPose); // Dress Pose == Bind Pose�� ����� ��.(Animation Matrix)
			pCluster->GetTransformMatrix(matRefGlobalInitPosition); // Offset Matrix, �θ� �ڽ� ���밡 �پ� �ٳ�� �ϱ� ������ Pivot(Dummy, offset)�� �����Ǿ�� �Ѵ� == �̵��� �� �ʿ��ϴ�.  
			FbxAMatrix matInversedBindPose = matRefGlobalInitPosition.Inverse() * matXBindPose; // ���� ����� ������ �� ���ε� ��� ������ ���� �ִϸ��̼�
			matInversedBindPose = matInversedBindPose.Inverse(); // ���� ����ķ� ����� �־ ������ ���� �־�� ������ ���� ��ķ� ��ȯ �����ϴ�.
			Matrix matInvBindPose = ConvertToDxMatrix(matInversedBindPose); // ������ ���� �� VertexBuffer�� �ְ� ���� ��İ� �����൵ �ǰ�, VertexBuffer�� ���ΰ� ���� ��� �տ� ���ص� ��.
			//_dst->BindPoseMap.insert(std::make_pair(BoneIndex, matInvBindPose)); // ��� ���� ���� ���� ���� �ְ�
			_dstData->BindPoseMap.insert(std::make_pair(LinkedNodeName, matInvBindPose)); // ��� ���� ���� ���� ���� �ְ�
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

	// �ؽ�ó ���� ����� �������� ����.
	// EMappingMode
	// eNone: ������ �������� �ʾҴ�.
	// eByControlPoint: ���������� 1���� ������ �Ǿ� �ִ�. (�� ������ UV��ǥ�� ��� ����.)
	// eByPolygonVertex: �� ������ ���� �����￡ ���ο� ��ǥ�� �ִ�. (�����￡�� Ž�� �ؾ���)
	// eByPolygon: ��ü �����￡ ���� ��ǥ�� �ִ�. 
	// eByEdge: ������ 1���� ���� ��ǥ�� �ִ�.
	// eAllSame: ��ü �����￡ 1���� ���� ��ǥ�� �ִ�.
	// ���� ��� -> ���� ��� Ȯ�� �ʿ�. == Reference Mode
	// eDirect: ����Ʈ�� ����Ǿ� ����. N��° ���� ������ DirectArray�� N��° ��ġ�� �ִ�.
	// eIndex: FBX 5.0 ���� �������� ���Ǿ���. ������ ������� ����. �� �̻��� ���������� eIndexToDirect�� ��ü��.
	// eIndexToDirect: �ε����� ����Ǿ� ����. 
	switch (_uv->GetMappingMode())
	{
		case FbxLayerElementUV::eByControlPoint:
		{
			// ���� ���� UV�� 1���� ������ �ش� Vertex���� ������ ������.
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
			// �����￡ �ϳ� ������ ���̷�Ʈ�� �����͵� �ȴ�.
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

					FbxVector2 fbxUv = _uv->GetDirectArray().GetAt(_uvIdx); // ���̷�Ʈ�� �������� ��.
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

	// �ؽ�ó ���� ����� �������� ����.
	// EMappingMode
	// eNone: ������ �������� �ʾҴ�.
	// eByControlPoint: ���������� 1���� ������ �Ǿ� �ִ�. (�� ������ UV��ǥ�� ��� ����.)
	// eByPolygonVertex: �� ������ ���� �����￡ ���ο� ��ǥ�� �ִ�. (�����￡�� Ž�� �ؾ���)
	// eByPolygon: ��ü �����￡ ���� ��ǥ�� �ִ�. 
	// eByEdge: ������ 1���� ���� ��ǥ�� �ִ�.
	// eAllSame: ��ü �����￡ 1���� ���� ��ǥ�� �ִ�.
	// ���� ��� -> ���� ��� Ȯ�� �ʿ�. == Reference Mode
	// eDirect: ����Ʈ�� ����Ǿ� ����. N��° ���� ������ DirectArray�� N��° ��ġ�� �ִ�.
	// eIndex: FBX 5.0 ���� �������� ���Ǿ���. ������ ������� ����. �� �̻��� ���������� eIndexToDirect�� ��ü��.
	// eIndexToDirect: �ε����� ����Ǿ� ����. 
	switch (_color->GetMappingMode())
	{
	case FbxLayerElementUV::eByControlPoint:
	{
		// ���� ���� UV�� 1���� ������ �ش� Vertex���� ������ ������.
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
		// �����￡ �ϳ� ������ ���̷�Ʈ�� �����͵� �ȴ�.
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
			_dst = _color->GetDirectArray().GetAt(_colorIdx); // ���̷�Ʈ�� �������� ��.
			break;
		}
		case FbxLayerElementUV::eIndexToDirect:
		{
			//int id = _uv->GetIndexArray().GetAt(_uvIdx);
			//FbxVector2 fbxUv = _uv->GetDirectArray().GetAt(id);
			//_dst.mData[0] = fbxUv.mData[0];
			//_dst.mData[1] = fbxUv.mData[1];

			//_dst = _color->GetDirectArray().GetAt(_colorIdx); // ���̷�Ʈ�� �������� ��.
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

	// �ؽ�ó ���� ����� �������� ����.
	// EMappingMode
	// eNone: ������ �������� �ʾҴ�.
	// eByControlPoint: ���������� 1���� ������ �Ǿ� �ִ�. (�� ������ UV��ǥ�� ��� ����.)
	// eByPolygonVertex: �� ������ ���� �����￡ ���ο� ��ǥ�� �ִ�. (�����￡�� Ž�� �ؾ���)
	// eByPolygon: ��ü �����￡ ���� ��ǥ�� �ִ�. 
	// eByEdge: ������ 1���� ���� ��ǥ�� �ִ�.
	// eAllSame: ��ü �����￡ 1���� ���� ��ǥ�� �ִ�.
	// ���� ��� -> ���� ��� Ȯ�� �ʿ�. == Reference Mode
	// eDirect: ����Ʈ�� ����Ǿ� ����. N��° ���� ������ DirectArray�� N��° ��ġ�� �ִ�.
	// eIndex: FBX 5.0 ���� �������� ���Ǿ���. ������ ������� ����. �� �̻��� ���������� eIndexToDirect�� ��ü��.
	// eIndexToDirect: �ε����� ����Ǿ� ����. 
	switch (_normal->GetMappingMode())
	{
	case FbxLayerElementUV::eByControlPoint:
	{
		// ���� ���� UV�� 1���� ������ �ش� Vertex���� ������ ������.
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
		// �����￡ �ϳ� ������ ���̷�Ʈ�� �����͵� �ȴ�.
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
			_dst = _normal->GetDirectArray().GetAt(_normalIdx); // ���̷�Ʈ�� �������� ��.
			break;
		}
		case FbxLayerElementUV::eIndexToDirect:
		{
			//int id = _uv->GetIndexArray().GetAt(_uvIdx);
			//FbxVector2 fbxUv = _uv->GetDirectArray().GetAt(id);
			//_dst.mData[0] = fbxUv.mData[0];
			//_dst.mData[1] = fbxUv.mData[1];

			//_dst = _color->GetDirectArray().GetAt(_colorIdx); // ���̷�Ʈ�� �������� ��.
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

int FBXLoader::getSubMaterialIndex(FbxLayerElementMaterial* _material, int _polyIdx)
{
	// ���ι��
	//eNone,
	//eByControlPoint,  // ������
	//eByPolygonVertex, //  
	//eByPolygon, // �����︶�� �ٸ��� �ִ�.
	//eAllSame - ��üǥ�鿡 1���� ������ǥ�� �ִ�.
	int iSubMtrl = 0;
	if (_material != nullptr)
	{
		switch (_material->GetMappingMode())
		{
		case FbxLayerElement::eByPolygon:
		{
			// ���� ������ �迭�� ����Ǵ� ���
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
	//static const char* sDiffuse; // ������ ���� ����ϴ� �ؽ��� ���. ���� Diffuse�� ������ ����. �⺻ ���
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
	// �ؽ�ó ������ �������� ���� ��. ���� 1���� Surface�� 24�� �̻��� �ؽ��İ� �پ� �ִ�.(24�� �̻��� �ؽ��� ����� �����Ѵ�.)
	// �ؽ��� ���� ������ �ִ�(ex. ����ũ �ؽ�óó�� ���ĸ� ���� ��� �� �������� ������ ����.)
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
	//static const char* sDiffuse; // ������ ���� ����ϴ� �ؽ��� ���. ���� Diffuse�� ������ ����. �⺻ ���
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
	// ���� ���
	// Affine ����� ���� ��ȯ �� ��, �׳� ����� ��� ���� ���� �� �� ���
	FbxAMatrix geometryMatrix; // ���� ���. �ʱ� ���� ��ġ�� ��ȯ �� �� ���. ���� ���. �� ��Ʈ������ ������ �ٲ��� ������ ���� ��Ŀ��� ��ȯ. ���� ��ȯ.
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
	// �븻 ������ ������� ��ġ ��ķ� ���� ��� ��.
	FbxAMatrix normalMatrix = _src;
	normalMatrix = normalMatrix.Inverse();
	normalMatrix = normalMatrix.Transpose();
	return normalMatrix;
}

FbxAMatrix FBXLoader::getWorldMatrix(FbxNode* _node)
{
	// ���� ���, �ִϸ��̼ǿ��� ��� �ϸ� �ȵ�. ������ �ۿ��� ����ؾ� �ϳ� �ӽ÷� ���
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
		time.SetFrame(t, TimeMode); // �̰� �ð��� ���� ��ƸԾ �ִ��� ���� ȣ���ϴ°� ����.
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
			float tick = 1.0f / _data->InterpolationSampling; // Sampling �� ��ŭ ����.
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
