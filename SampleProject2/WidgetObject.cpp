#include "WidgetObject.h"
#include "Input.hpp"

WidgetObject::WidgetObject()
{
	Initialize();
}

WidgetObject::~WidgetObject()
{
	VertexLayout->Release();
	VertexShader->Release();
	VertexBuffer->Release();
	IndexBuffer->Release();
	PixelShader->Release();
	TransformBuffer->Release();
	CameraMatrixBuffer->Release();
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

	CameraMatrix CameraMatrixData;
	CameraMatrixBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<CameraMatrix>(CameraMatrixData);
	


	return true;
}

bool WidgetObject::PostRender()
{
	DXDevice::g_pImmediateContext->UpdateSubresource(CameraMatrixBuffer, 0, NULL, &CameraData, 0, 0);
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(1, 1, &CameraMatrixBuffer);

	// ���͸��� ���
	DXDevice::g_pImmediateContext->IASetInputLayout(VertexLayout);
	DXDevice::g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DXDevice::g_pImmediateContext->VSSetShader(VertexShader, NULL, 0);
	DXDevice::g_pImmediateContext->UpdateSubresource(TransformBuffer, 0, NULL, &TransformData, 0, 0);
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(0, 1, &TransformBuffer);

	// �޽� ���
	UINT Strides = sizeof(Vertex); // ���� 1���� ����Ʈ �뷮
	UINT Offsets = 0; // ���� ���ۿ��� ��� ����(����Ʈ)
	DXDevice::g_pImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Strides, &Offsets);
	DXDevice::g_pImmediateContext->UpdateSubresource(VertexBuffer, 0, NULL, &Vertices.at(0), 0, 0);

	// ����ƽ �Ž� ���
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
	if (m_pCutInfoList.size() == 0) return false;	// ������ �ϳ��� ���� ���� ������Ʈ �Ÿ����
	CheckMouseHover();	// ���콺 ȣ�� üũ, ������ ������ ���� ����

	switch (m_Type)
	{

	case(IMAGE):
	{
		if (m_b3D) UpdateCut3D(0);
		else UpdateCut(0);
	} break;

	case(BUTTON):
	{
		SetButtonState();
		if (!m_bIsAnime) { Drag(); }
	} break;

	case(SPRITE):
	{
	} break;

	}

	if (m_bIsAnime) 
	{ Animation(); }

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

	if (m_pCutInfoList.size() == 1) // ù ��°�� ������ �̹����� �������� �浹���� ����
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

	if (m_bOPS) // ������ �߾��� ���
	{
		Vertices[0].Pos = { m_OriginPos.x - ndcWH.x / 2, m_OriginPos.y + ndcWH.y / 2, m_fDepth }; // p1-LT
		Vertices[1].Pos = { m_OriginPos.x + ndcWH.x / 2, m_OriginPos.y + ndcWH.y / 2, m_fDepth }; // p2-RT
		Vertices[2].Pos = { m_OriginPos.x - ndcWH.x / 2, m_OriginPos.y - ndcWH.y / 2, m_fDepth }; // p3-LB
		Vertices[3].Pos = { m_OriginPos.x + ndcWH.x / 2, m_OriginPos.y - ndcWH.y / 2, m_fDepth }; // p4-RB
	}
	else // ������ �»���� ��� 
	{
		Vertices[0].Pos = { m_OriginPos.x ,			  m_OriginPos.y			 , m_fDepth }; // p1-LT
		Vertices[1].Pos = { m_OriginPos.x + ndcWH.x , m_OriginPos.y			 , m_fDepth }; // p2-RT
		Vertices[2].Pos = { m_OriginPos.x ,			  m_OriginPos.y - ndcWH.y, m_fDepth }; // p3-LB
		Vertices[3].Pos = { m_OriginPos.x + ndcWH.x , m_OriginPos.y - ndcWH.y, m_fDepth }; // p4-RB
	}

	// Color, ���İ� �����
	Vertices[0].Color = { 1.0f, 1.0f, 1.0f, m_fAlpha }; // p1-LT
	Vertices[1].Color = { 1.0f, 1.0f, 1.0f, m_fAlpha }; // p2-RT
	Vertices[2].Color = { 1.0f, 1.0f, 1.0f, m_fAlpha }; // p3-LB
	Vertices[3].Color = { 1.0f, 1.0f, 1.0f, m_fAlpha }; // p4-LB

	Vertices[0].Texture = { m_pCutInfoList[cutNum]->uv.x, m_pCutInfoList[cutNum]->uv.y }; // p1-LT
	Vertices[1].Texture = { m_pCutInfoList[cutNum]->uv.z, m_pCutInfoList[cutNum]->uv.y }; // p2-RT
	Vertices[2].Texture = { m_pCutInfoList[cutNum]->uv.x, m_pCutInfoList[cutNum]->uv.w }; // p3-LB
	Vertices[3].Texture = { m_pCutInfoList[cutNum]->uv.z, m_pCutInfoList[cutNum]->uv.w }; // p4-RB

	Texture = m_pCutInfoList[cutNum]->tc;

	m_CollisionBox[0] = { Vertices[0].Pos };
	m_CollisionBox[1] = { Vertices[3].Pos };

	return true;
}

// 3D ������ UI��
bool WidgetObject::UpdateCut3D(int cutNum)
{
	//Vertices[0].Pos = {-1.0f, 1.0f, 0.0f}; // p1-LT
	//Vertices[1].Pos = { 1.0f, 1.0f, 0.0f}; // p2-RT
	//Vertices[2].Pos = {-1.0f,-1.0f, 0.0f}; // p3-LB
	//Vertices[3].Pos = { 1.0f,-1.0f, 0.0f}; // p4-RB
	Vector2 ndcWH = PtoN(m_pCutInfoList[cutNum]->pxWH);
	Vertices[0].Pos = { m_OriginPos.x - ndcWH.x / 2, m_OriginPos.y + ndcWH.y / 2, 0.0f }; // p1-LT
	Vertices[1].Pos = { m_OriginPos.x + ndcWH.x / 2, m_OriginPos.y + ndcWH.y / 2, 0.0f }; // p2-RT
	Vertices[2].Pos = { m_OriginPos.x - ndcWH.x / 2, m_OriginPos.y - ndcWH.y / 2, 0.0f }; // p3-LB
	Vertices[3].Pos = { m_OriginPos.x + ndcWH.x / 2, m_OriginPos.y - ndcWH.y / 2, 0.0f }; // p4-RB

	// Color, ���İ� �����
	Vertices[0].Color = { 1.0f, 1.0f, 1.0f, m_fAlpha }; // p1-LT
	Vertices[1].Color = { 1.0f, 1.0f, 1.0f, m_fAlpha }; // p2-RT
	Vertices[2].Color = { 1.0f, 1.0f, 1.0f, m_fAlpha }; // p3-LB
	Vertices[3].Color = { 1.0f, 1.0f, 1.0f, m_fAlpha }; // p4-LB

	Vertices[0].Texture = { m_pCutInfoList[cutNum]->uv.x, m_pCutInfoList[cutNum]->uv.y }; // p1-LT
	Vertices[1].Texture = { m_pCutInfoList[cutNum]->uv.z, m_pCutInfoList[cutNum]->uv.y }; // p2-RT
	Vertices[2].Texture = { m_pCutInfoList[cutNum]->uv.x, m_pCutInfoList[cutNum]->uv.w }; // p3-LB
	Vertices[3].Texture = { m_pCutInfoList[cutNum]->uv.z, m_pCutInfoList[cutNum]->uv.w }; // p4-RB

	CameraData.View = MainCamera->View.Transpose();
	CameraData.Projection = MainCamera->Projection.Transpose();

	Matrix BiilTMat;
	BiilTMat = Matrix::CreateScale(scale) * MainCamera->View.Invert() ;
	BiilTMat._41 = m_OrginPos3D.x;
	BiilTMat._42 = m_OrginPos3D.y;
	BiilTMat._43 = m_OrginPos3D.z;
	BiilTMat = BiilTMat.Transpose();

	TransformData.Mat = BiilTMat;
	TransformData.InversedMat = TransformData.Mat.Invert().Transpose();

	return true;
}

Vector2 WidgetObject::PtoN(Vector2 pxWH)
{
	Vector2 result;

	result.x = (pxWH.x / DXDevice::g_ViewPort.Width) * 2.0f;
	result.y = (pxWH.y / DXDevice::g_ViewPort.Height) * 2.0f;

	return result;
}

Vector2 WidgetObject::PtoN_Pos(Vector2 pxPos)
{
	Vector2 result;

	result.x = -1.0f + (pxPos.x / DXDevice::g_ViewPort.Width)*2.0f;
	result.y = -(-1.0f + (pxPos.y / DXDevice::g_ViewPort.Height)*2.0f);

	return result;
}

Vector2 WidgetObject::NtoP_Pos(Vector2 ndcPos)
{
	Vector2 result;

	result.x = DXDevice::g_ViewPort.Width * (ndcPos.x + 1.0f)/2;
	result.y = DXDevice::g_ViewPort.Height * (-ndcPos.y + 1.0f)/2;

	return result;
}

// ������ �»���� ������Ʈ�� ���, ���� �缳��
void WidgetObject::SetOrginStandard(bool ops)
{
	m_bOPS = ops;

	Vector2 ndcWH = PtoN(m_pCutInfoList[0]->pxWH);	// �ϴ� ù��° �� ����, �� ���������~
	if (!ops) // ������ �»���̶�� (�߾��� �ƴ϶�)
	{
		m_OriginPos.x -= ndcWH.x / 2;
		m_OriginPos.y += ndcWH.y / 2;
	}
}

void WidgetObject::CheckMouseHover()
{
	if (m_bIsDead) return;	// ���� ���¶�� ������ üũ ��

	POINT cp;
	::GetCursorPos(&cp);
	::ScreenToClient(DXDevice::g_hWnd, &cp);

	float mouseNdcX = (cp.x / DXDevice::g_ViewPort.Width) * 2.0f - 1.0f;
	float mouseNdcY = -((cp.y / DXDevice::g_ViewPort.Height) * 2.0f - 1.0f);

	if (m_CollisionBox[0].x <= mouseNdcX && m_CollisionBox[1].x >= mouseNdcX &&
		m_CollisionBox[0].y >= mouseNdcY && m_CollisionBox[1].y <= mouseNdcY) 
	{m_bHover = true;}

	else { m_bHover = false; }
}

bool WidgetObject::SetButtonState()
{
	if (m_bDisable) // ��Ȱ��ȭ ���¶��
	{
		UpdateCut(DISABLE);
		return true;
	}


	// ���콺�� ���� ���� ��ġ���ִٸ�
	if (m_bHover)
	{
		KeyState LButtonState = Input::GetInstance()->getKey(VK_LBUTTON);
		if (LButtonState == KeyState::Down || LButtonState == KeyState::Hold)
		{
			if (LButtonState == KeyState::Down) { m_bClicked = true; }	// Ŭ��ó���� �ѹ��� �ǰԲ�, �̰� �������ٵ�
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
	//else { m_OriginPos = m_OriginalOriginPos; }
	//�巡�� Ǯ����, �̰� ���⼭ �ٷﵵ �ǳ� �ͱ��ѵ� �� �巡�� ��ü�� ī�常�̴ϱ�..
	else if (m_bDraggable && !m_bIsAnime && m_OriginPos != m_OriginalOriginPos)
	{
		Vector2 AnimPos[2] = { NtoP_Pos(m_OriginPos), NtoP_Pos(m_OriginalOriginPos) };
		SetAnimation(AnimPos, 0.3f);
	}

	return true;
}

// ��ġ�� �ϴ� �ȼ������� �޾ƿ�
void WidgetObject::SetAnimation(Vector2 transform[2], float rotation[2], Vector2 scale[2], float playtime)
{
	m_bIsAnime = true;

	m_AnimT[0] = PtoN_Pos(transform[0]);
	m_AnimT[0] = PtoN_Pos(transform[1]);
	m_AnimR[0] = rotation[0];
	m_AnimR[1] = rotation[1];
	m_AnimS[0] = scale[0];
	m_AnimS[1] = scale[1];
						  
	m_AnimPT = playtime;
	m_AnimCT = 0.0f;
}

void WidgetObject::SetAnimation(Vector2 transform[2], float playtime, bool fade)
{
	m_bIsAnime = true;
	if (fade) 
	{m_bIsFade = true; m_fAlpha = 0.0f;}

	m_AnimT[0] = PtoN_Pos(transform[0]);
	m_AnimT[1] = PtoN_Pos(transform[1]);

	m_AnimPT = playtime;
	m_AnimCT = 0.0f;
}

// �ϴ��� �̵���, + ���̵�
void WidgetObject::Animation()
{
	m_AnimCT += Timer::GetInstance()->GetDeltaTime();

	if (m_AnimCT >= m_AnimPT) // �ð� �� �������
	{
		m_AnimCT = m_AnimPT;
		m_bIsAnime = false;
		if (m_bIsFade) { m_fAlpha = 0.0f; m_bIsFade = false;}
		m_OriginalOriginPos = m_AnimT[1];
	}

	m_OriginPos.x = ((m_AnimPT-m_AnimCT)/m_AnimPT)*m_AnimT[0].x + (m_AnimCT/m_AnimPT)*m_AnimT[1].x;
	m_OriginPos.y = ((m_AnimPT - m_AnimCT) / m_AnimPT) * m_AnimT[0].y + (m_AnimCT / m_AnimPT) * m_AnimT[1].y;

	if (m_bIsFade) // ���̵��� �ɼ��� �� �ִϸ��̼��̶��
	{
		// ���̵� ��/�ƿ� �ۼ�Ʈ
		float fadeIn = 0.25f;
		float fadeOut = 0.2f;
		// �÷���Ÿ���� �ʹ� fadeIn%���� ���̵���
		if ((m_AnimCT / m_AnimPT) <= fadeIn) { m_fAlpha = (m_AnimCT / m_AnimPT) / fadeIn; }
		// �÷���Ÿ���� �Ĺ� fadeOut%���� ���̵�ƿ�
		else if ((m_AnimCT / m_AnimPT) >= (1.0f-fadeOut)) { m_fAlpha = ((m_AnimPT - m_AnimCT) / m_AnimPT) / fadeOut; }
		// �߰����� �׳� Ǯ����
		else { m_fAlpha = 1.0f; }
	}

	UpdateCut(0); // �� �븻���·� �ִϸ��̼� �ǰ� �ص� ��������..?
}
