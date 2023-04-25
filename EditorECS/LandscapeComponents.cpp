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

	// Shadow
	ShadowVertexShader = DXShaderManager::GetInstance()->GetVertexShader(L"Shadow");
	ShadowPixelShader = DXShaderManager::GetInstance()->GetPixelShader(L"Shadow");

	return true;
}

void LandscapeComponents::Build(int column, int row, int sectionSize, int cellDistance)
{
	Row = row;
	Column = column;
	SectionSize = sectionSize;
	CellDistance = cellDistance;

	Components.clear();
	Components.resize(Row * Column);
	Transform->Translation.x = -static_cast<float>(Column) * static_cast<float>(SectionSize + 1) * static_cast<float>(CellDistance) / 2.0f;
	Transform->Translation.z = -static_cast<float>(Row) * static_cast<float>(SectionSize + 1) * static_cast<float>(CellDistance) / 2.0f;
	for (int compRow = 0; compRow < Row; compRow++)
	{
		for (int compCol = 0; compCol < Column; compCol++)
		{
			int compIndex = compRow * Column + compCol;
			Components[compIndex].Row = compRow;
			Components[compIndex].Column = compCol;
			size_t vertexCnt = (SectionSize + 1) * (SectionSize + 1);
			Components[compIndex].Vertices.resize(vertexCnt);
			float cx = static_cast<float>(compCol) * static_cast<float>(SectionSize) + static_cast<float>(SectionSize) / 2.0f;
			float cz = static_cast<float>(compRow) * static_cast<float>(SectionSize) + static_cast<float>(SectionSize) / 2.0f;

			for (int sectRow = 0; sectRow < SectionSize + 1; sectRow++)
			{
				for (int sectCol = 0; sectCol < SectionSize + 1; sectCol++)
				{
					size_t idx = sectRow * (SectionSize + 1) + sectCol;
					// compRow == 0, compCol == 0, x(0 ~ sectionSize + 1), z(0 ~ sectionSize + 1) 
					// compRow == 0, compCol == 1, x(sectionSize ~ sectionSize * compCol + sectionSize + 1), z(0 ~ sectionSize + 1)
					// compRow == 1, compCol == 0, x(0 ~ sectionSize + 1), z(sectionSize ~ sectionSize * compRow + sectionSize + 1) 
					// compRow == 1, compCol == 1, x(sectionSize ~ sectionSize * compCol + sectionSize + 1), z(0 ~ sectionSize + 1)
					float x = (SectionSize * compCol + sectCol) * CellDistance;
					float z = (SectionSize * compRow + sectRow) * CellDistance;
					Vector3 pos = Vector3(x, 0.0f, z);
					Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
					Vector3 normal = Vector3(0.0f, 1.0f, 0.0f);

					float u = static_cast<float>(sectCol) / static_cast<float>(sectionSize);
					float v = static_cast<float>(sectRow) / static_cast<float>(sectionSize);
					Vector2 texture = Vector2(u, v);
					Components[compIndex].Vertices[idx] = Vertex(pos, normal, color, texture);
					//component.Vertices.push_back(Vertex(pos, normal, color, texture));
					if (sectRow == 0 && sectCol == 0)
					{
						Components[compIndex].MinVertex = pos;
					}
					else if (sectRow == sectionSize && sectCol == sectionSize)
					{
						Components[compIndex].MaxVertex = pos;
					}
				}
			}

			size_t cellCnt = sectionSize * sectionSize;
			size_t faceCnt = sectionSize * sectionSize * 2;
			size_t IndexCount = faceCnt * 3;
			Components[compIndex].Indices.resize(IndexCount);
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

					Components[compIndex].Indices[idx + 0] = sectRow * (sectionSize + 1) + sectCol;
					Components[compIndex].Indices[idx + 1] = (sectRow + 1) * (sectionSize + 1) + sectCol;
					Components[compIndex].Indices[idx + 2] = sectRow * (sectionSize + 1) + sectCol + 1;

					Components[compIndex].Indices[idx + 3] = sectRow * (sectionSize + 1) + sectCol + 1;
					Components[compIndex].Indices[idx + 4] = (sectRow + 1) * (sectionSize + 1) + sectCol;
					Components[compIndex].Indices[idx + 5] = (sectRow + 1) * (sectionSize + 1) + sectCol + 1;

					/*Components[compIndex].Faces.push_back(
						Face(Components[compIndex].Vertices[Components[compIndex].Indices[idx + 0]],
							Components[compIndex].Vertices[Components[compIndex].Indices[idx + 1]],
							Components[compIndex].Vertices[Components[compIndex].Indices[idx + 2]]));

					Components[compIndex].Faces.push_back(
						Face(Components[compIndex].Vertices[Components[compIndex].Indices[idx + 3]],
							Components[compIndex].Vertices[Components[compIndex].Indices[idx + 4]],
							Components[compIndex].Vertices[Components[compIndex].Indices[idx + 5]]));*/

					Components[compIndex].Faces.push_back(
						Face(Components[compIndex].Indices[idx + 0],
							Components[compIndex].Indices[idx + 1],
							Components[compIndex].Indices[idx + 2]));

					Components[compIndex].Faces.push_back(
						Face(Components[compIndex].Indices[idx + 3],
							Components[compIndex].Indices[idx + 4],
							Components[compIndex].Indices[idx + 5]));

					idx += 6;
				}
			}
			Vector3 center = (Components[compIndex].MinVertex + Components[compIndex].MaxVertex) / 2.0f;
			Vector3 extend = (Components[compIndex].MaxVertex - Components[compIndex].MinVertex) / 2.0f;
			extend.y += 0.01f;
			Components[compIndex].Box = DirectX::BoundingBox(center, extend);
			Components[compIndex].BaseTexture = BaseTexture;
			Components[compIndex].Initialize();
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
	DXDevice::g_pImmediateContext->PSSetConstantBuffers(4, 1, &TransformBuffer);
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

void LandscapeComponents::RenderShadow()
{
	DXDevice::g_pImmediateContext->IASetInputLayout(VertexLayout);
	DXDevice::g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DXDevice::g_pImmediateContext->VSSetShader(ShadowVertexShader, NULL, 0);
	//DXDevice::g_pImmediateContext->PSSetShader(ShadowPixelShader, NULL, 0);
	DXDevice::g_pImmediateContext->PSSetShader(nullptr, NULL, 0);
	DXDevice::g_pImmediateContext->HSSetShader(nullptr, NULL, 0);
	DXDevice::g_pImmediateContext->DSSetShader(nullptr, NULL, 0);
	DXDevice::g_pImmediateContext->GSSetShader(nullptr, NULL, 0);
	DXDevice::g_pImmediateContext->UpdateSubresource(TransformBuffer, 0, NULL, &TransformData, 0, 0);
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(0, 1, &TransformBuffer);

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

void LandscapeComponents::Splatting(Vector3 pos, float radius, int idx, float strength)
{
	for (auto& component : Components)
	{
		DirectX::BoundingSphere sphere = DirectX::BoundingSphere(pos, radius);
		if (sphere.Intersects(component.Box))
		{
			for (auto& face : component.Faces)
			{
				if (pos.Distance(pos, component.Vertices[face.V0].Pos) < radius)
				{
					component.Splatting(pos, radius, idx, strength);
				}
				else if (pos.Distance(pos, component.Vertices[face.V1].Pos) < radius)
				{
					component.Splatting(pos, radius, idx, strength);
				}
				else if (pos.Distance(pos, component.Vertices[face.V2].Pos) < radius)
				{
					component.Splatting(pos, radius, idx, strength);
				}
			}
		}
	}
}

bool LandscapeComponents::Save(std::wstring filename)
{
	// XML 파일 선언
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);

	//루트 노드 추가
	TiXmlElement* pRoot = new TiXmlElement("Landscape");
	doc.LinkEndChild(pRoot);

	////주석 문장 추가
	//TiXmlComment* pComment = new TiXmlComment();
	//pComment->SetValue("This is Test Comment");
	//pRoot->LinkEndChild(pComment);

	// 하위노드 및 데이터 추가
	TiXmlElement* pInfo = new TiXmlElement("Information");
	pRoot->LinkEndChild(pInfo);

	//하위노드 및 속성 추가
	TiXmlElement* pSizeInfo = new TiXmlElement("Size");
	pInfo->LinkEndChild(pSizeInfo);
	pSizeInfo->SetAttribute("ComponentsSize", Components.size());
	pSizeInfo->SetAttribute("Row", Row);
	pSizeInfo->SetAttribute("Column", Column);
	pSizeInfo->SetAttribute("SectionSize", SectionSize);
	pSizeInfo->SetAttribute("CellDistance", CellDistance);

	TiXmlElement* pMaterial = new TiXmlElement("Material");
	pInfo->LinkEndChild(pMaterial);
	std::string material(MaterialName.begin(), MaterialName.end());
	pMaterial->SetAttribute("Name", material.c_str());

	// 컴포넌트 하위 노드
	TiXmlElement* pComponents = new TiXmlElement("Components");
	pRoot->LinkEndChild(pComponents);
	for (auto& it : Components)
	{
		TiXmlElement* pComponent = new TiXmlElement("Component");
		pComponents->LinkEndChild(pComponent);
		pComponent->SetAttribute("Column", it.Column);
		pComponent->SetAttribute("Row", it.Row);

		// Vertices
		TiXmlElement* pVertices = new TiXmlElement("Vertices");
		pComponent->LinkEndChild(pVertices);
		pVertices->SetAttribute("Count", it.Vertices.size());
		pVertices->SetDoubleAttribute("MaxX", it.MaxVertex.x);
		pVertices->SetDoubleAttribute("MaxY", it.MaxVertex.y);
		pVertices->SetDoubleAttribute("MaxZ", it.MaxVertex.z);
		pVertices->SetDoubleAttribute("MinX", it.MinVertex.x);
		pVertices->SetDoubleAttribute("MinY", it.MinVertex.y);
		pVertices->SetDoubleAttribute("MinZ", it.MinVertex.z);

		for (size_t idx = 0; idx < it.Vertices.size(); idx++)
		{
			TiXmlElement* pVertexElem = new TiXmlElement("Vertex");
			pVertices->LinkEndChild(pVertexElem);
			pVertexElem->SetAttribute("Idx", idx);

			TiXmlElement* pPosElem = new TiXmlElement("Position");
			pVertexElem->LinkEndChild(pPosElem);
			pPosElem->SetDoubleAttribute("X", it.Vertices[idx].Pos.x);
			pPosElem->SetDoubleAttribute("Y", it.Vertices[idx].Pos.y);
			pPosElem->SetDoubleAttribute("Z", it.Vertices[idx].Pos.z);
			
			TiXmlElement* pTexcoordElem = new TiXmlElement("Texcoord");
			pVertexElem->LinkEndChild(pTexcoordElem);
			pTexcoordElem->SetDoubleAttribute("X", it.Vertices[idx].Texture.x);
			pTexcoordElem->SetDoubleAttribute("Y", it.Vertices[idx].Texture.y);

			TiXmlElement* pColorElem = new TiXmlElement("Color");
			pVertexElem->LinkEndChild(pColorElem);
			pColorElem->SetDoubleAttribute("R", it.Vertices[idx].Color.x);
			pColorElem->SetDoubleAttribute("G", it.Vertices[idx].Color.y);
			pColorElem->SetDoubleAttribute("B", it.Vertices[idx].Color.z);
			pColorElem->SetDoubleAttribute("W", it.Vertices[idx].Color.w);

			TiXmlElement* pNormalElem = new TiXmlElement("Normal");
			pVertexElem->LinkEndChild(pNormalElem);
			pNormalElem->SetDoubleAttribute("X", it.Vertices[idx].Normal.x);
			pNormalElem->SetDoubleAttribute("Y", it.Vertices[idx].Normal.y);
			pNormalElem->SetDoubleAttribute("Z", it.Vertices[idx].Normal.z);

			TiXmlElement* pTangentElem = new TiXmlElement("Tangent");
			pVertexElem->LinkEndChild(pTangentElem);
			pTangentElem->SetDoubleAttribute("X", it.Vertices[idx].Tangent.x);
			pTangentElem->SetDoubleAttribute("Y", it.Vertices[idx].Tangent.y);
			pTangentElem->SetDoubleAttribute("Z", it.Vertices[idx].Tangent.z);
		}

		// Indices
		TiXmlElement* pIndices = new TiXmlElement("Indices");
		pComponent->LinkEndChild(pIndices);
		pIndices->SetAttribute("Count", it.Indices.size());

		for (size_t idx = 0; idx < it.Indices.size(); idx++)
		{
			TiXmlElement* pIndexElem = new TiXmlElement("Index");
			pIndices->LinkEndChild(pIndexElem);
			pIndexElem->SetAttribute("Idx", idx);
			pIndexElem->SetAttribute("Val", it.Indices[idx]);
		}

		// Faces
		TiXmlElement* pFaces = new TiXmlElement("Faces");
		pComponent->LinkEndChild(pFaces);
		pFaces->SetAttribute("Count", it.Faces.size());

		for (size_t idx = 0; idx < it.Faces.size(); idx++)
		{
			TiXmlElement* pFaceElem = new TiXmlElement("Face");
			pFaces->LinkEndChild(pFaceElem);
			pFaceElem->SetAttribute("Idx", idx);
			pFaceElem->SetAttribute("V0", it.Faces[idx].V0);
			pFaceElem->SetAttribute("V1", it.Faces[idx].V1);
			pFaceElem->SetAttribute("V2", it.Faces[idx].V2);
		}

	}

	std::string saveFile(filename.begin(), filename.end());
	return doc.SaveFile(saveFile.c_str());
}

bool LandscapeComponents::Load(std::wstring filename)
{
	TiXmlDocument doc;
	std::string readfile(filename.begin(), filename.end());
	if (!doc.LoadFile(readfile.c_str()))
	{
		return false;
	}

	TiXmlElement* root = doc.FirstChildElement("Landscape");
	
	// Read Info
	TiXmlElement* info = root->FirstChildElement("Information");
	TiXmlElement* size = info->FirstChildElement("Size");
	TiXmlAttribute* pAttribute = size->FirstAttribute();
	int componentsSize = pAttribute->IntValue();
	Components.resize(componentsSize);
	pAttribute = pAttribute->Next();

	Row = pAttribute->IntValue(); 
	pAttribute = pAttribute->Next();

	Column = pAttribute->IntValue();
	pAttribute = pAttribute->Next();

	SectionSize = pAttribute->IntValue();
	pAttribute = pAttribute->Next();

	CellDistance = pAttribute->IntValue();
	pAttribute = pAttribute->Next();

	// Read Material
	TiXmlElement* pMaterial = info->FirstChildElement("Material");
	pAttribute = pMaterial->FirstAttribute();
	std::string materialName = pAttribute->Value();
	if (!MaterialName.empty())
	{
		MaterialName.assign(materialName.begin(), materialName.end());
	}

	TiXmlElement* pComponents = root->FirstChildElement("Components");
	TiXmlElement* pComponent = nullptr; // = pComponents->FirstChildElement("Component");
	for (size_t compIdx = 0; compIdx < componentsSize; compIdx++)
	{
		if (compIdx == 0)
		{
			pComponent = pComponents->FirstChildElement("Component");
		}
		else
		{
			pComponent = pComponent->NextSiblingElement();
		}

		pAttribute = pComponent->FirstAttribute();

		Components[compIdx].Column = pAttribute->IntValue();
		pAttribute = pAttribute->Next();

		Components[compIdx].Row = pAttribute->IntValue();
		pAttribute = pAttribute->Next();


		// Vertices
		TiXmlElement* pVertices = pComponent->FirstChildElement("Vertices");
		pAttribute = pVertices->FirstAttribute();

		Components[compIdx].VertexCount = pAttribute->IntValue();
		Components[compIdx].Vertices.resize(Components[compIdx].VertexCount);
		pAttribute = pAttribute->Next();

		Components[compIdx].MaxVertex.x = pAttribute->DoubleValue();
		pAttribute = pAttribute->Next();

		Components[compIdx].MaxVertex.y = pAttribute->DoubleValue();
		pAttribute = pAttribute->Next();

		Components[compIdx].MaxVertex.z = pAttribute->DoubleValue();
		pAttribute = pAttribute->Next();

		Components[compIdx].MinVertex.x = pAttribute->DoubleValue();
		pAttribute = pAttribute->Next();

		Components[compIdx].MinVertex.y = pAttribute->DoubleValue();
		pAttribute = pAttribute->Next();

		Components[compIdx].MinVertex.z = pAttribute->DoubleValue();
		pAttribute = pAttribute->Next();

		TiXmlElement* pVertex = nullptr;
		for (size_t vertexIdx = 0; vertexIdx < Components[compIdx].VertexCount; vertexIdx++)
		{
			if (vertexIdx == 0)
			{
				pVertex = pVertices->FirstChildElement("Vertex");
			}
			else
			{
				pVertex = pVertex->NextSiblingElement();
			}

			pAttribute = pVertex->FirstAttribute();
			int idx = pAttribute->IntValue();

			TiXmlElement* pPos = pVertex->FirstChildElement("Position");
			pAttribute = pPos->FirstAttribute();
			Components[compIdx].Vertices[idx].Pos.x = pAttribute->DoubleValue();
			pAttribute = pAttribute->Next();
			Components[compIdx].Vertices[idx].Pos.y = pAttribute->DoubleValue();
			pAttribute = pAttribute->Next();
			Components[compIdx].Vertices[idx].Pos.z = pAttribute->DoubleValue();
			pAttribute = pAttribute->Next();

			TiXmlElement* pTexcoord = pVertex->FirstChildElement("Texcoord");
			pAttribute = pTexcoord->FirstAttribute();
			Components[compIdx].Vertices[idx].Texture.x = pAttribute->DoubleValue();
			pAttribute = pAttribute->Next();
			Components[compIdx].Vertices[idx].Texture.y = pAttribute->DoubleValue();
			pAttribute = pAttribute->Next();
			
			TiXmlElement* pColor = pVertex->FirstChildElement("Color");
			pAttribute = pColor->FirstAttribute();
			Components[compIdx].Vertices[idx].Color.x = pAttribute->DoubleValue();
			pAttribute = pAttribute->Next();
			Components[compIdx].Vertices[idx].Color.y = pAttribute->DoubleValue();
			pAttribute = pAttribute->Next();
			Components[compIdx].Vertices[idx].Color.z = pAttribute->DoubleValue();
			pAttribute = pAttribute->Next();
			Components[compIdx].Vertices[idx].Color.w = pAttribute->DoubleValue();
			pAttribute = pAttribute->Next();

			TiXmlElement* pNormal = pVertex->FirstChildElement("Normal");
			pAttribute = pNormal->FirstAttribute();
			Components[compIdx].Vertices[idx].Normal.x = pAttribute->DoubleValue();
			pAttribute = pAttribute->Next();
			Components[compIdx].Vertices[idx].Normal.y = pAttribute->DoubleValue();
			pAttribute = pAttribute->Next();
			Components[compIdx].Vertices[idx].Normal.z = pAttribute->DoubleValue();
			pAttribute = pAttribute->Next();
			
			TiXmlElement* pTangent = pVertex->FirstChildElement("Tangent");
			pAttribute = pTangent->FirstAttribute();
			Components[compIdx].Vertices[idx].Tangent.x = pAttribute->DoubleValue();
			pAttribute = pAttribute->Next();
			Components[compIdx].Vertices[idx].Tangent.y = pAttribute->DoubleValue();
			pAttribute = pAttribute->Next();
			Components[compIdx].Vertices[idx].Tangent.z = pAttribute->DoubleValue();
			pAttribute = pAttribute->Next();
		}


		// Indices
		TiXmlElement* pIndices = pComponent->FirstChildElement("Indices");
		pAttribute = pIndices->FirstAttribute();

		Components[compIdx].IndexCount = pAttribute->IntValue();
		Components[compIdx].Indices.resize(Components[compIdx].IndexCount);

		TiXmlElement* pIndex = nullptr;
		for (size_t indicesIdx = 0; indicesIdx < Components[compIdx].IndexCount; indicesIdx++)
		{
			if (indicesIdx == 0)
			{
				pIndex = pIndices->FirstChildElement("Index");
			}
			else
			{
				pIndex = pIndex->NextSiblingElement();
			}

			pAttribute = pIndex->FirstAttribute();
			int idx = pAttribute->IntValue();
			pAttribute = pAttribute->Next();

			Components[compIdx].Indices[idx] = pAttribute->IntValue();
		}

		// Faces
		TiXmlElement* pFaces = pComponent->FirstChildElement("Faces");
		pAttribute = pFaces->FirstAttribute();

		Components[compIdx].FaceCount = pAttribute->IntValue();
		Components[compIdx].Faces.resize(Components[compIdx].FaceCount);

		TiXmlElement* pFace = nullptr;
		for (size_t faceIdx = 0; faceIdx < Components[compIdx].FaceCount; faceIdx++)
		{
			if (faceIdx == 0)
			{
				pFace = pFaces->FirstChildElement("Face");
			}
			else
			{
				pFace = pFace->NextSiblingElement();
			}

			pAttribute = pFace->FirstAttribute();
			int idx = pAttribute->IntValue();
			pAttribute = pAttribute->Next();

			Components[compIdx].Faces[idx].V0 = pAttribute->IntValue();
			pAttribute = pAttribute->Next();
			Components[compIdx].Faces[idx].V1 = pAttribute->IntValue();
			pAttribute = pAttribute->Next();
			Components[compIdx].Faces[idx].V2 = pAttribute->IntValue();
			pAttribute = pAttribute->Next();
		}

	}

	return true;
}

bool LandscapeComponents::WriteXML(TiXmlElement* parent)
{
	TiXmlElement* pLandscapeComp = new TiXmlElement("LandscapeComponents");
	parent->LinkEndChild(pLandscapeComp);

	// 하위노드 및 데이터 추가
	TiXmlElement* pInfo = new TiXmlElement("Information");
	pLandscapeComp->LinkEndChild(pInfo);

	//하위노드 및 속성 추가
	TiXmlElement* pSizeInfo = new TiXmlElement("Size");
	pInfo->LinkEndChild(pSizeInfo);
	pSizeInfo->SetAttribute("ComponentsSize", Components.size());
	pSizeInfo->SetAttribute("Row", Row);
	pSizeInfo->SetAttribute("Column", Column);
	pSizeInfo->SetAttribute("SectionSize", SectionSize);
	pSizeInfo->SetAttribute("CellDistance", CellDistance);

	TiXmlElement* pMaterial = new TiXmlElement("Material");
	pInfo->LinkEndChild(pMaterial);
	std::string material(MaterialName.begin(), MaterialName.end());
	pMaterial->SetAttribute("Name", material.c_str());

	// 컴포넌트 하위 노드
	TiXmlElement* pComponents = new TiXmlElement("Components");
	pLandscapeComp->LinkEndChild(pComponents);
	for (auto& it : Components)
	{
		TiXmlElement* pComponent = new TiXmlElement("Component");
		pComponents->LinkEndChild(pComponent);
		pComponent->SetAttribute("Column", it.Column);
		pComponent->SetAttribute("Row", it.Row);

		// Vertices
		TiXmlElement* pVertices = new TiXmlElement("Vertices");
		pComponent->LinkEndChild(pVertices);
		pVertices->SetAttribute("Count", it.Vertices.size());

		if (!WriteVector3(pVertices, it.MaxVertex, "MaxVertex"))
		{
			OutputDebugString(L"EditorECS::LandscapeComponents::WriteXML::Failed Write Max Vertex.\n");
		}

		if (!WriteVector3(pVertices, it.MinVertex, "MinVertex"))
		{
			OutputDebugString(L"EditorECS::LandscapeComponents::WriteXML::Failed Write Min Vertex.\n");
		}

		for (size_t idx = 0; idx < it.Vertices.size(); idx++)
		{
			TiXmlElement* pVertexElem = new TiXmlElement("Vertex");
			pVertices->LinkEndChild(pVertexElem);
			pVertexElem->SetAttribute("Idx", idx);

			if (!WriteVector3(pVertexElem, it.Vertices[idx].Pos, "Position"))
			{
				OutputDebugString(L"EditorECS::LandscapeComponents::WriteXML::Failed Write Vertex Position.\n");
			}

			if (!WriteVector2(pVertexElem, it.Vertices[idx].Texture, "Texcoord"))
			{
				OutputDebugString(L"EditorECS::LandscapeComponents::WriteXML::Failed Write Vertex Texcoord.\n");
			}

			if (!WriteVector4(pVertexElem, it.Vertices[idx].Color, "Color"))
			{
				OutputDebugString(L"EditorECS::LandscapeComponents::WriteXML::Failed Write Vertex Color.\n");
			}

			if (!WriteVector3(pVertexElem, it.Vertices[idx].Normal, "Normal"))
			{
				OutputDebugString(L"EditorECS::LandscapeComponents::WriteXML::Failed Write Vertex Normal.\n");
			}

			if (!WriteVector3(pVertexElem, it.Vertices[idx].Tangent, "Tangent"))
			{
				OutputDebugString(L"EditorECS::LandscapeComponents::WriteXML::Failed Write Vertex Tangent.\n");
			}
		}

		// Indices
		TiXmlElement* pIndices = new TiXmlElement("Indices");
		pComponent->LinkEndChild(pIndices);
		pIndices->SetAttribute("Count", it.Indices.size());

		for (size_t idx = 0; idx < it.Indices.size(); idx++)
		{
			TiXmlElement* pIndexElem = new TiXmlElement("Index");
			pIndices->LinkEndChild(pIndexElem);
			pIndexElem->SetAttribute("Idx", idx);
			pIndexElem->SetAttribute("Val", it.Indices[idx]);
		}

		// Faces
		TiXmlElement* pFaces = new TiXmlElement("Faces");
		pComponent->LinkEndChild(pFaces);
		pFaces->SetAttribute("Count", it.Faces.size());

		for (size_t idx = 0; idx < it.Faces.size(); idx++)
		{
			TiXmlElement* pFaceElem = new TiXmlElement("Face");
			pFaces->LinkEndChild(pFaceElem);
			pFaceElem->SetAttribute("Idx", idx);
			pFaceElem->SetAttribute("V0", it.Faces[idx].V0);
			pFaceElem->SetAttribute("V1", it.Faces[idx].V1);
			pFaceElem->SetAttribute("V2", it.Faces[idx].V2);
		}

	}

	return true;
}

bool LandscapeComponents::ReadXML(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	TiXmlElement* root = parent->FirstChildElement("LandscapeComponents");
	if (root == nullptr)
	{
		return false;
	}

	// Read Info
	TiXmlElement* info = root->FirstChildElement("Information");
	TiXmlElement* size = info->FirstChildElement("Size");
	TiXmlAttribute* pAttribute = size->FirstAttribute();
	int componentsSize = pAttribute->IntValue();
	Components.resize(componentsSize);
	pAttribute = pAttribute->Next();

	Row = pAttribute->IntValue();
	pAttribute = pAttribute->Next();

	Column = pAttribute->IntValue();
	pAttribute = pAttribute->Next();

	SectionSize = pAttribute->IntValue();
	pAttribute = pAttribute->Next();

	CellDistance = pAttribute->IntValue();
	pAttribute = pAttribute->Next();

	// Read Material
	TiXmlElement* pMaterial = info->FirstChildElement("Material");
	pAttribute = pMaterial->FirstAttribute();
	std::string materialName = pAttribute->Value();
	if (!MaterialName.empty())
	{
		MaterialName.assign(materialName.begin(), materialName.end());
	}

	TiXmlElement* pComponents = root->FirstChildElement("Components");
	TiXmlElement* pComponent = nullptr; // = pComponents->FirstChildElement("Component");
	for (size_t compIdx = 0; compIdx < componentsSize; compIdx++)
	{
		if (compIdx == 0)
		{
			pComponent = pComponents->FirstChildElement("Component");
		}
		else
		{
			pComponent = pComponent->NextSiblingElement();
		}

		pAttribute = pComponent->FirstAttribute();

		Components[compIdx].Column = pAttribute->IntValue();
		pAttribute = pAttribute->Next();

		Components[compIdx].Row = pAttribute->IntValue();
		pAttribute = pAttribute->Next();


		// Vertices
		TiXmlElement* pVertices = pComponent->FirstChildElement("Vertices");
		pAttribute = pVertices->FirstAttribute();

		Components[compIdx].VertexCount = pAttribute->IntValue();
		Components[compIdx].Vertices.resize(Components[compIdx].VertexCount);
		pAttribute = pAttribute->Next();

		TiXmlElement* pMaxVertex = pVertices->FirstChildElement("MaxVertex");
		pAttribute = pMaxVertex->FirstAttribute();
		if (!ReadVector3(pAttribute, Components[compIdx].MaxVertex))
		{

		}

		TiXmlElement* pMinVertex = pVertices->FirstChildElement("MinVertex");
		pAttribute = pMinVertex->FirstAttribute();
		if (!ReadVector3(pAttribute, Components[compIdx].MinVertex))
		{

		}

		Components[compIdx].Box.Center = (Components[compIdx].MaxVertex + Components[compIdx].MinVertex) * 0.5f;
		Components[compIdx].Box.Extents = (Components[compIdx].MaxVertex - Components[compIdx].MinVertex) * 0.5f;
		Components[compIdx].Box.Extents.y += 0.01f;

		TiXmlElement* pVertex = nullptr;
		for (size_t vertexIdx = 0; vertexIdx < Components[compIdx].VertexCount; vertexIdx++)
		{
			if (vertexIdx == 0)
			{
				pVertex = pVertices->FirstChildElement("Vertex");
			}
			else
			{
				pVertex = pVertex->NextSiblingElement();
			}

			pAttribute = pVertex->FirstAttribute();
			int idx = pAttribute->IntValue();

			TiXmlElement* pPos = pVertex->FirstChildElement("Position");
			pAttribute = pPos->FirstAttribute();
			if (!ReadVector3(pAttribute, Components[compIdx].Vertices[idx].Pos))
			{

			}

			TiXmlElement* pTexcoord = pVertex->FirstChildElement("Texcoord");
			pAttribute = pTexcoord->FirstAttribute();
			if (!ReadVector2(pAttribute, Components[compIdx].Vertices[idx].Texture))
			{

			}

			TiXmlElement* pColor = pVertex->FirstChildElement("Color");
			pAttribute = pColor->FirstAttribute();
			if (!ReadVector4(pAttribute, Components[compIdx].Vertices[idx].Color))
			{

			}

			TiXmlElement* pNormal = pVertex->FirstChildElement("Normal");
			pAttribute = pNormal->FirstAttribute();
			if (!ReadVector3(pAttribute, Components[compIdx].Vertices[idx].Normal))
			{

			}

			TiXmlElement* pTangent = pVertex->FirstChildElement("Tangent");
			pAttribute = pTangent->FirstAttribute();
			if (!ReadVector3(pAttribute, Components[compIdx].Vertices[idx].Tangent))
			{

			}
		}

		// Indices
		TiXmlElement* pIndices = pComponent->FirstChildElement("Indices");
		pAttribute = pIndices->FirstAttribute();

		Components[compIdx].IndexCount = pAttribute->IntValue();
		Components[compIdx].Indices.resize(Components[compIdx].IndexCount);

		TiXmlElement* pIndex = nullptr;
		for (size_t indicesIdx = 0; indicesIdx < Components[compIdx].IndexCount; indicesIdx++)
		{
			if (indicesIdx == 0)
			{
				pIndex = pIndices->FirstChildElement("Index");
			}
			else
			{
				pIndex = pIndex->NextSiblingElement();
			}

			pAttribute = pIndex->FirstAttribute();
			int idx = pAttribute->IntValue();
			pAttribute = pAttribute->Next();

			Components[compIdx].Indices[idx] = pAttribute->IntValue();
		}

		// Faces
		TiXmlElement* pFaces = pComponent->FirstChildElement("Faces");
		pAttribute = pFaces->FirstAttribute();

		Components[compIdx].FaceCount = pAttribute->IntValue();
		Components[compIdx].Faces.resize(Components[compIdx].FaceCount);

		TiXmlElement* pFace = nullptr;
		for (size_t faceIdx = 0; faceIdx < Components[compIdx].FaceCount; faceIdx++)
		{
			if (faceIdx == 0)
			{
				pFace = pFaces->FirstChildElement("Face");
			}
			else
			{
				pFace = pFace->NextSiblingElement();
			}

			pAttribute = pFace->FirstAttribute();
			int idx = pAttribute->IntValue();
			pAttribute = pAttribute->Next();

			Components[compIdx].Faces[idx].V0 = pAttribute->IntValue();
			pAttribute = pAttribute->Next();
			Components[compIdx].Faces[idx].V1 = pAttribute->IntValue();
			pAttribute = pAttribute->Next();
			Components[compIdx].Faces[idx].V2 = pAttribute->IntValue();
			pAttribute = pAttribute->Next();
		}

	}

	return true;
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

