#pragma once
#include "Define.h"
#include "DXTextureManager.hpp"
#include "DXShaderManager.h"
#include "CameraMatrix.h" //Vector����� ���⿡�־��ī��..
#include "Mesh.h"

enum UIType
{
	IMAGE,
	BUTTON,
	SPRITE,
};

enum ButtonState
{
	NORMAL,
	HOVER,
	CLICK,
	DISABLE,
};

struct CI // Cut Information
{
	Vector2 pxWH; // pixel Width Height
	Vector4 uv; // UV (Top & Left, Bottom & Right)

	std::wstring tn; // Texture name

	DXTexture* tc; // Texture class

	int cn;	// cut number
};

class WidgetObject
{
public:
	WidgetObject();
	~WidgetObject();

public:
	bool Initialize();
	bool PostRender();
	bool Render();
	bool Frame();

	bool SetPosition(Vector2 orginPos);
	bool AddCut(Vector2 pxWH, Vector4 uv, std::wstring tn);
	bool UpdateCut(int cutNum);
	Vector2 PtoN(Vector2 pxWH);

	bool SetButtonState();
	bool Drag();

public:
	std::vector<Vertex> Vertices;
	std::vector<Face>	Faces;
	std::vector<DWORD>	Indices;

	int VertexCount = 0;
	int FaceCount = 0;
	int IndexCount = 0;

	ID3D11InputLayout* VertexLayout = nullptr;
	ID3D11VertexShader* VertexShader = nullptr;
	ID3D11Buffer* VertexBuffer = nullptr;
	ID3D11Buffer* IndexBuffer = nullptr;
	DXTexture* Texture = nullptr;
	ID3D11PixelShader* PixelShader = nullptr;
	ID3D11Buffer* TransformBuffer = nullptr;
	TransformMatrix	TransformData;
	ID3D11Buffer* CameraMatrixBuffer;


// �������ʹ� ������ Ŀ����
public:
	std::vector<CI*>	m_pCutInfoList;
	int					m_iCurrentCutNum = 0;

	UIType			m_Type;
	int				m_ID; // ��� ���⼭ �ʿ�����ʳ�? ��
	Vector2			m_OriginPos;
	float			m_fDepth = 0.0f;
	float			m_fAlpha = 1.0f;

	bool			m_bIsDead = false;
	bool			m_bDisable = false; // ��ư�̶� ��������Ʈ ����
	bool			m_bClicked = false; // �̺�Ʈ ó����, ��..
	Vector2			m_CollisionBox[2];  // NDC ����, ���� Top Left, Bottom Right

	bool			m_bDraggable = false; // ī��� �巡�� �����ϵ���
	Vector2			m_OriginalOriginPos;  // ���� �ڸ��� �ٽ� ���ƿ�����

};

