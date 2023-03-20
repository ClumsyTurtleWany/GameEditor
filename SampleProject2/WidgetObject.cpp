#include "WidgetObject.h"
#include "Input.hpp"

WidgetObject::WidgetObject()
{
	Initialize();
}

WidgetObject::~WidgetObject()
{
}

bool WidgetObject::Initialize()
{
	Vertices.resize(4);

	Vertices[0].Pos = { -1.0f, +1.0f, 0.0f }; // p1-LT
	Vertices[1].Pos = { -0.9f, +1.0f, 0.0f }; // p2-RT
	Vertices[2].Pos = { -1.0f, +0.9f, 0.0f }; // p3-LB
	Vertices[3].Pos = { -0.9f, +0.9f, 0.0f }; // p4-LB

	Vertices[0].Normal = { 0.0f, 0.0f, -1.0f };
	Vertices[1].Normal = { 0.0f, 0.0f, -1.0f };
	Vertices[2].Normal = { 0.0f, 0.0f, -1.0f };
	Vertices[3].Normal = { 0.0f, 0.0f, -1.0f };

	Vertices[0].Color = { 1.0f, 1.0f, 1.0f, 1.0f }; // p1-LT
	Vertices[1].Color = { 1.0f, 1.0f, 1.0f, 1.0f }; // p2-RT
	Vertices[2].Color = { 1.0f, 1.0f, 1.0f, 1.0f }; // p3-LB
	Vertices[3].Color = { 1.0f, 1.0f, 1.0f, 1.0f }; // p4-LB

	Vertices[0].Texture = { 0.0f, 0.0f }; // p1-LT
	Vertices[1].Texture = { 1.0f, 0.0f }; // p2-RT
	Vertices[2].Texture = { 0.0f, 1.0f }; // p3-LB
	Vertices[3].Texture = { 1.0f, 1.0f }; // p4-LB

	Indices.resize(6);
	Indices[0] = 0;
	Indices[1] = 1;
	Indices[2] = 2;
	Indices[3] = 2;
	Indices[4] = 1;
	Indices[5] = 3;

	VertexBuffer = DXShaderManager::GetInstance()->CreateVertexBuffer(Vertices);
	IndexBuffer = DXShaderManager::GetInstance()->CreateIndexBuffer(Indices);

	VertexLayout = DXShaderManager::GetInstance()->GetInputLayout(L"StaticMesh");
	VertexShader = DXShaderManager::GetInstance()->GetVertexShader(L"StaticMesh");
	PixelShader = DXShaderManager::GetInstance()->GetPixelShader(L"T_UI");
	TransformBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<TransformMatrix>(TransformData);

	return true;
}

bool WidgetObject::PostRender()
{
	// 일단 컴포넌트 렌더만 돌려본 거라 임시로 추가, 나중에 시스템에 붙이고 나면 빼도 될지도 -> 아니네!
	CameraMatrix CameraMatrixData;
	ID3D11Buffer* CameraMatrixBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<CameraMatrix>(CameraMatrixData);
	DXDevice::g_pImmediateContext->UpdateSubresource(CameraMatrixBuffer, 0, NULL, &CameraMatrixData, 0, 0);
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(1, 1, &CameraMatrixBuffer);

	// 머터리얼 담당
	DXDevice::g_pImmediateContext->IASetInputLayout(VertexLayout);
	DXDevice::g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DXDevice::g_pImmediateContext->VSSetShader(VertexShader, NULL, 0);
	DXDevice::g_pImmediateContext->UpdateSubresource(TransformBuffer, 0, NULL, &TransformData, 0, 0);
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(0, 1, &TransformBuffer);

	// 메쉬 담당
	UINT Strides = sizeof(Vertex); // 정점 1개의 바이트 용량
	UINT Offsets = 0; // 정점 버퍼에서 출발 지점(바이트)
	DXDevice::g_pImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Strides, &Offsets);
	DXDevice::g_pImmediateContext->UpdateSubresource(VertexBuffer, 0, NULL, &Vertices.at(0), 0, 0);

	// 스태틱 매쉬 담당
	DXDevice::g_pImmediateContext->PSSetShader(PixelShader, NULL, 0);
	ID3D11ShaderResourceView* resourceView = Texture->GetResourceView();
	DXDevice::g_pImmediateContext->PSSetShaderResources(0, 1, &resourceView);

	if (IndexBuffer == nullptr)
	{
		DXDevice::g_pImmediateContext->Draw(static_cast<UINT>(Vertices.size()), 0);
	}
	else
	{
		DXDevice::g_pImmediateContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		DXDevice::g_pImmediateContext->UpdateSubresource(IndexBuffer, 0, NULL, &Indices.at(0), 0, 0);
		DXDevice::g_pImmediateContext->DrawIndexed(static_cast<UINT>(Indices.size()), 0, 0);
	}

	return true;
}

bool WidgetObject::Render()
{
	PostRender();
	return true;
}

bool WidgetObject::Frame()
{
	if (m_pCutInfoList.size() == 0) return false;

	switch (m_Type)
	{

	case(IMAGE):
	{
		UpdateCut(0);
	} break;

	case(BUTTON):
	{
		SetButtonState();
		Drag();
	} break;

	case(SPRITE):
	{
	} break;

	}

	return true;
}

bool WidgetObject::SetPosition(Vector2 originPos)
{
	m_OriginPos = originPos;
	m_OriginalOriginPos = originPos;

	return true;
}

bool WidgetObject::AddCut(Vector2 pxWH, Vector4 uv, std::wstring tn)
{
	CI* newCI = new CI;
	newCI->pxWH = pxWH;
	newCI->uv = uv;
	newCI->tn = tn;

	if (DXTextureManager::GetInstance()->Load(tn))
	{
		newCI->tc = DXTextureManager::GetInstance()->GetTexture(tn);
	}

	newCI->cn = m_iCurrentCutNum;
	m_iCurrentCutNum++;

	m_pCutInfoList.push_back(newCI);

	if (m_pCutInfoList.size() == 1) // 첫 번째로 들어오는 이미지를 기준으로 충돌범위 설정
	{
		UpdateCut(0);
		m_CollisionBox[0] = { Vertices[0].Pos };
		m_CollisionBox[1] = { Vertices[3].Pos };
	}

	return true;
}

bool WidgetObject::UpdateCut(int cutNum)
{
	if (m_pCutInfoList.size() == 0) return false;

	Vector2 ndcWH = PtoN(m_pCutInfoList[cutNum]->pxWH);

	Vertices[0].Pos = { m_OriginPos.x - ndcWH.x / 2, m_OriginPos.y + ndcWH.y / 2, m_fDepth }; // p1-LT
	Vertices[1].Pos = { m_OriginPos.x + ndcWH.x / 2, m_OriginPos.y + ndcWH.y / 2, m_fDepth }; // p2-RT
	Vertices[2].Pos = { m_OriginPos.x - ndcWH.x / 2, m_OriginPos.y - ndcWH.y / 2, m_fDepth }; // p3-LB
	Vertices[3].Pos = { m_OriginPos.x + ndcWH.x / 2, m_OriginPos.y - ndcWH.y / 2, m_fDepth }; // p4-RB

	Vertices[0].Texture = { m_pCutInfoList[cutNum]->uv.x, m_pCutInfoList[cutNum]->uv.y }; // p1-LT
	Vertices[1].Texture = { m_pCutInfoList[cutNum]->uv.z, m_pCutInfoList[cutNum]->uv.y }; // p2-RT
	Vertices[2].Texture = { m_pCutInfoList[cutNum]->uv.x, m_pCutInfoList[cutNum]->uv.w }; // p3-LB
	Vertices[3].Texture = { m_pCutInfoList[cutNum]->uv.z, m_pCutInfoList[cutNum]->uv.w }; // p4-RB

	Texture = m_pCutInfoList[cutNum]->tc;

	return true;
}

Vector2 WidgetObject::PtoN(Vector2 pxWH)
{
	Vector2 result;

	result.x = pxWH.x / DXDevice::g_ViewPort.Width * 2.0f;
	result.y = pxWH.y / DXDevice::g_ViewPort.Height * 2.0f;

	return result;
}

bool WidgetObject::SetButtonState()
{
	if (m_bDisable) // 비활성화 상태라면
	{
		UpdateCut(DISABLE);
		return true;
	}

	POINT cp;
	::GetCursorPos(&cp);
	::ScreenToClient(DXDevice::g_hWnd, &cp);

	float mouseNdcX = (cp.x / DXDevice::g_ViewPort.Width) * 2.0f - 1.0f;
	float mouseNdcY = -((cp.y / DXDevice::g_ViewPort.Height) * 2.0f - 1.0f);

	// 마우스가 위젯 위에 위치해있다면
	if (m_CollisionBox[0].x <= mouseNdcX && m_CollisionBox[1].x >= mouseNdcX &&
		m_CollisionBox[0].y >= mouseNdcY && m_CollisionBox[1].y <= mouseNdcY)
	{
		KeyState LButtonState = Input::GetInstance()->getKey(VK_LBUTTON);
		if (LButtonState == KeyState::Down || LButtonState == KeyState::Hold)
		{
			if (LButtonState == KeyState::Down) { m_bClicked = true; }	// 클릭처리는 한번만 되게끔, 이거 먹히나근데
			UpdateCut(CLICK);
		}
		else { UpdateCut(HOVER);}
	}
	else { UpdateCut(NORMAL); }

	if (m_bClicked && Input::GetInstance()->getKey(VK_LBUTTON) == KeyState::Up) { m_bClicked = false; }

	return true;
}

bool WidgetObject::Drag()
{
	if (m_bDraggable && m_bClicked) 
	{
		m_OriginPos += PtoN({(float)Input::GetInstance()->m_ptOffset.x, -((float)Input::GetInstance()->m_ptOffset.y)});
	}
	else if (m_bDraggable) { m_OriginPos = m_OriginalOriginPos; }

	return true;
}
