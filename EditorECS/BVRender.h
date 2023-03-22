#pragma once
#include "Define.h"
#include "EffectInclude/EffectDefine.h"

#define MAX_NUMBER_OF_CIRCLE_VERTEX 60

struct LineVertex
{
	Vector3 p;
	Vector4 c;
};

struct ShaderGroup
{
	ID3D11VertexShader*		pVS;
	ID3D11HullShader*		pHS;
	ID3D11DomainShader*		pDS;
	ID3D11GeometryShader*	pGS;
	ID3D11PixelShader*		pPS;
	ID3D11ComputeShader*	pCS;

	ShaderGroup()
	{
		pVS = nullptr;
		pHS = nullptr;
		pDS = nullptr;
		pGS = nullptr;
		pPS = nullptr;
		pCS = nullptr;

		//µ¿ÀÏ ±¸¹®
		//ZeroMemory(this, sizeof(ShaderGroup));
	}

	void bindToPipeline(ID3D11DeviceContext* pDContext)
	{
		if (pDContext)
		{
			pDContext->VSSetShader(pVS, NULL, 0);
			pDContext->HSSetShader(pHS, NULL, 0);
			pDContext->DSSetShader(pDS, NULL, 0);
			pDContext->GSSetShader(pGS, NULL, 0);
			pDContext->PSSetShader(pPS, NULL, 0);
			pDContext->CSSetShader(pCS, NULL, 0);
		}
	}
};

class BVRenderObject
{
public:
	ID3D11Device*									m_pDevice;
	ID3D11DeviceContext*							m_pDContext;

	//RenderType [ 0 = BOX / 1 = SPHERE ]
	bool											m_renderType;
	ShaderGroup										m_shaders;

	std::vector<LineVertex>							m_vertices;
	EFFECTUTIL::CBUF_COORDCONV_MATSET				m_wvpMat;

	UINT											m_stride;
	UINT											m_offset;

	Microsoft::WRL::ComPtr<ID3D11Buffer>			m_pVBuf;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			m_pCBuf;

public:


	BVRenderObject(bool renderType = 0)
	{
		m_pDevice = nullptr;
		m_pDContext = nullptr;

		m_renderType = renderType;

		m_stride = sizeof(LineVertex);
		m_offset = 0;
	}

	~BVRenderObject()
	{

	}

	void setVertices(DirectX::BoundingOrientedBox& OBB)
	{
		Vector3 Axis[3] =
		{
			{1.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 1.0f}
		};

		Matrix rot = Matrix::CreateFromQuaternion(OBB.Orientation);

		Axis[0] = Vector3::Transform(Axis[0], rot);
		Axis[1] = Vector3::Transform(Axis[1], rot);
		Axis[2] = Vector3::Transform(Axis[2], rot);

		Color vColor = { 1.0f, 0.0f, 0.0f, 1.0f };

		LineVertex vertices[] =
		{
			//0 LBB -> 1 LBF -> 2 RBF -> 3 RBB // ¾Æ·§¸é
			{{ OBB.Center - Axis[0] * OBB.Extents.x - Axis[1] * OBB.Extents.y + Axis[2] * OBB.Extents.z },	vColor },	//LBB
			{{ OBB.Center - Axis[0] * OBB.Extents.x - Axis[1] * OBB.Extents.y - Axis[2] * OBB.Extents.z },	vColor },	//LBF
			{{ OBB.Center + Axis[0] * OBB.Extents.x - Axis[1] * OBB.Extents.y - Axis[2] * OBB.Extents.z },	vColor },	//RBF
			{{ OBB.Center + Axis[0] * OBB.Extents.x - Axis[1] * OBB.Extents.y + Axis[2] * OBB.Extents.z },  vColor },	//RBB

			//0 LBB -> 4 LTB -> 7 RTB -> 3 RBB // µÞ¸é
			{{ OBB.Center - Axis[0] * OBB.Extents.x - Axis[1] * OBB.Extents.y + Axis[2] * OBB.Extents.z },	vColor },	//LBB
			{{ OBB.Center - Axis[0] * OBB.Extents.x + Axis[1] * OBB.Extents.y + Axis[2] * OBB.Extents.z },	vColor },	//LTB
			{{ OBB.Center + Axis[0] * OBB.Extents.x + Axis[1] * OBB.Extents.y + Axis[2] * OBB.Extents.z },	vColor },	//RTB
			{{ OBB.Center + Axis[0] * OBB.Extents.x - Axis[1] * OBB.Extents.y + Axis[2] * OBB.Extents.z },  vColor },	//RBB

			//7 RTB -> 6 RTF -> 5 LTF -> 4 LTB // À­¸é 
			{{ OBB.Center + Axis[0] * OBB.Extents.x + Axis[1] * OBB.Extents.y + Axis[2] * OBB.Extents.z },	vColor },	//RTB
			{{ OBB.Center + Axis[0] * OBB.Extents.x + Axis[1] * OBB.Extents.y - Axis[2] * OBB.Extents.z },	vColor },	//RTF
			{{ OBB.Center - Axis[0] * OBB.Extents.x + Axis[1] * OBB.Extents.y - Axis[2] * OBB.Extents.z },	vColor },	//LTF
			{{ OBB.Center - Axis[0] * OBB.Extents.x + Axis[1] * OBB.Extents.y + Axis[2] * OBB.Extents.z },  vColor },	//LTB

			//5 LTF -> 1 LBF -> 2 RBF -> 6 RTF // ¾Õ¸é
			{{ OBB.Center - Axis[0] * OBB.Extents.x + Axis[1] * OBB.Extents.y - Axis[2] * OBB.Extents.z },	vColor },	//LTF	
			{{ OBB.Center - Axis[0] * OBB.Extents.x - Axis[1] * OBB.Extents.y - Axis[2] * OBB.Extents.z },	vColor },	//LBF
			{{ OBB.Center + Axis[0] * OBB.Extents.x - Axis[1] * OBB.Extents.y - Axis[2] * OBB.Extents.z },	vColor },	//RBF
			{{ OBB.Center + Axis[0] * OBB.Extents.x + Axis[1] * OBB.Extents.y - Axis[2] * OBB.Extents.z },	vColor },	//RTF
		};

		for (auto& it : vertices)
		{
			m_vertices.push_back(it);
		}

		m_renderType = 0;
	}

	void setVertices(DirectX::BoundingSphere& Sphere)
	{
		Color vColor = { 1.0f, 0.0f, 0.0f, 1.0f };

		Vector3 unit = { 1.0f * Sphere.Radius, 0.0f, 0.0f };

		LineVertex verticesX[MAX_NUMBER_OF_CIRCLE_VERTEX];
		LineVertex verticesZ[MAX_NUMBER_OF_CIRCLE_VERTEX];

		float rotAngle = TAU / (float)MAX_NUMBER_OF_CIRCLE_VERTEX;

		Matrix rotX = Matrix::CreateFromYawPitchRoll(rotAngle, 0.0f, 0.0f);
		Matrix rotZ = Matrix::CreateFromYawPitchRoll(0.0f, 0.0f, rotAngle);

		verticesX[0] = { { unit.x, unit.y, unit.z }, vColor };
		verticesZ[0] = { { unit.x, unit.y, unit.z }, vColor };

		for (int i = 1; i < MAX_NUMBER_OF_CIRCLE_VERTEX; i++)
		{
			Vector3 P0 = Vector3::Transform(verticesX[i - 1].p, rotX);
			Vector3 P2 = Vector3::Transform(verticesZ[i - 1].p, rotZ);

			verticesX[i] = { { P0.x, P0.y, P0.z }, vColor };
			verticesZ[i] = { { P2.x, P2.y, P2.z }, vColor };
		}

		for (auto& it : verticesX) { m_vertices.push_back(it); }
		m_vertices.push_back(verticesX[0]);
		for (auto& it : verticesZ) { m_vertices.push_back(it); }
		m_vertices.push_back(verticesX[0]);


		m_renderType = 1;
	}

	void init(ID3D11Device* pDevice, 
		ID3D11DeviceContext* 
		pDContext, 
		DirectX::BoundingOrientedBox& OBB)
	{
		m_pDevice = pDevice;
		m_pDContext = pDContext;

		setVertices(OBB);

		m_shaders.pVS = DXShaderManager::GetInstance()->GetVertexShader(L"VS_LineObject");
		m_shaders.pPS = DXShaderManager::GetInstance()->GetPixelShader(L"PS_LineObject");

		if (m_pDevice)
		{
			int size = m_vertices.size() * sizeof(LineVertex);
			EFFECTUTIL::createDXSimpleBuf(m_pDevice,
				size,
				&m_vertices.at(0),
				D3D11_BIND_VERTEX_BUFFER,
				m_pVBuf.GetAddressOf());

			size = sizeof(EFFECTUTIL::CBUF_COORDCONV_MATSET);
			EFFECTUTIL::createDXSimpleBuf(m_pDevice,
				size,
				&m_wvpMat,
				D3D11_BIND_CONSTANT_BUFFER,
				m_pCBuf.GetAddressOf());
		}
	}

	void init(ID3D11Device* pDevice,
		ID3D11DeviceContext*
		pDContext,
		DirectX::BoundingSphere& sphere)
	{
		m_pDevice = pDevice;
		m_pDContext = pDContext;

		setVertices(sphere);

		m_shaders.pVS = DXShaderManager::GetInstance()->GetVertexShader(L"VS_LineObject");
		m_shaders.pPS = DXShaderManager::GetInstance()->GetPixelShader(L"PS_LineObject");

		if (m_pDevice)
		{
			int size = m_vertices.size() * sizeof(LineVertex);
			EFFECTUTIL::createDXSimpleBuf(m_pDevice,
				size,
				&m_vertices.at(0),
				D3D11_BIND_VERTEX_BUFFER,
				m_pVBuf.GetAddressOf());

			size = sizeof(EFFECTUTIL::CBUF_COORDCONV_MATSET);
			EFFECTUTIL::createDXSimpleBuf(m_pDevice,
				size,
				&m_wvpMat,
				D3D11_BIND_CONSTANT_BUFFER,
				m_pCBuf.GetAddressOf());
		}
	}

	void update(Matrix* pWorld, Matrix* pView, Matrix* pProj)
	{
		if (pWorld) { m_wvpMat.matTWorld = pWorld->Transpose(); }
		if (pView) { m_wvpMat.matTView = pView->Transpose(); }
		if (pProj) { m_wvpMat.matTProj = pProj->Transpose(); }

		if (m_pDContext)
		{
			D3D11_MAPPED_SUBRESOURCE ms;
			ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));

			HRESULT hr = m_pDContext->Map(m_pCBuf.Get(),
							0,
							D3D11_MAP_WRITE_DISCARD,
							NULL,
							&ms);

			if (SUCCEEDED(hr))
			{
				memcpy(ms.pData, &m_wvpMat, sizeof(EFFECTUTIL::CBUF_COORDCONV_MATSET));

				m_pDContext->Unmap(m_pCBuf.Get(), NULL);
			}
		}
	}

	void render()
	{
		if (m_pDContext)
		{
			m_pDContext->IASetVertexBuffers(0, 1, m_pVBuf.GetAddressOf(), &m_stride, &m_offset);
			m_pDContext->IASetInputLayout(DXShaderManager::GetInstance()->GetInputLayout(L"VS_LineObject"));
			m_pDContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

			m_shaders.bindToPipeline(m_pDContext);

			m_pDContext->VSSetConstantBuffers(0, 1, m_pCBuf.GetAddressOf());

			m_pDContext->Draw(m_vertices.size(), 0);
		}
	}

	void release()
	{

	}
};