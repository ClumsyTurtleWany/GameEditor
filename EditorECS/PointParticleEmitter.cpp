#include "PointParticleEmitter.h"

namespace EFFECTUTIL
{
	PointParticleEmitter::PointParticleEmitter()
	{
		//��ƼŬ ���� Ÿ�̸�
		m_fSpawnTime = 0.0f;
		m_fTimer = 0.0f;

		ZeroMemory(&m_shaderGroup, sizeof(ShaderGroup));
		m_pSprite = nullptr;
		m_pTexture = nullptr;

		m_iVertexCount = 0;
		m_iStride = sizeof(PointParticleVertex);
		m_iOffset = 0;

		m_pVLayout = nullptr;

		m_iBlendStateOption = BLENDSTATE_TYPE::BS_DEFAULT;
		m_bEnableDepth = false;
		m_bEnableDepthWrite = false;

		m_UVRect = { {0.0f, 0.0f}, {1.0f, 1.0f} };
	}

	PointParticleEmitter::~PointParticleEmitter()
	{
	}

	bool PointParticleEmitter::init()
	{
		return true;
	}

	bool PointParticleEmitter::update()
	{
		float dt = Timer::GetInstance()->GetDeltaTime();

		m_fTimer += dt;

		if (m_fTimer > m_fSpawnTime)
		{
			m_fTimer -= m_fSpawnTime;

			PointParticle* replaceTarget = nullptr;

			for (UINT i = 0; i < m_iVertexCount; i++)
			{
				if (m_particles[i].bEnable == false)
				{
					replaceTarget = &m_particles[i];
					break;
				}
			}

			if (replaceTarget)
			{
				replaceTarget->setParticle(m_eProp);
			}
		}

		for (UINT i = 0; i < m_iVertexCount; i++)
		{
			m_particles[i].fElapsedTime += dt;

			if (m_particles[i].fElapsedTime > m_particles[i].fLifeSpan)
			{
				m_particles[i].bEnable = false;

				//������ ȸ�� �Ӽ��� ������ ������ Ȱ��ȭ ���η� ����Ѵ�.
				m_vertices[i].rot.z = 0.0f;
			}

			if (m_particles[i].fElapsedTime <= m_particles[i].fLifeSpan)
			{
				if (m_pSprite)
				{
					UINT idx = (m_particles[i].fElapsedTime / m_particles[i].fLifeSpan) * m_pSprite->m_spriteInfo.m_iTotalFrame;
					if (idx >= m_pSprite->m_spriteInfo.m_iTotalFrame)
					{
						idx = m_pSprite->m_spriteInfo.m_iTotalFrame - 1;
					}

					if (m_pSprite->m_spriteInfo.m_spriteType == SPRITE_TYPE::SPRITE_UV)
					{
						m_UVRect = m_pSprite->m_UVSpriteArr[idx];
						m_pTexture = m_pSprite->m_pTexArr[0];
					}
					else
					{
						m_pTexture = m_pSprite->m_pTexArr[idx];
					}
				}

				m_particles[i].vVelocity += m_particles[i].vAccelelation * dt;
				m_particles[i].vPos = m_particles[i].vPos + m_particles[i].vVelocity * dt;

				m_particles[i].fRoll += m_particles[i].fRotVelocity * dt;

				float T = m_particles[i].fElapsedTime / m_particles[i].fLifeSpan;
				float invT = (m_particles[i].fLifeSpan - m_particles[i].fElapsedTime) / m_particles[i].fLifeSpan;

				m_particles[i].vScale =
				{
					m_particles[i].vInitScale.x * (m_eProp.fScaleOverLifespan.x * invT + m_eProp.fScaleOverLifespan.z * T),
					m_particles[i].vInitScale.y * (m_eProp.fScaleOverLifespan.y * invT + m_eProp.fScaleOverLifespan.w * T)
				};

				Color curColor = Color::Lerp(m_particles[i].initColor, m_particles[i].lastColor, T);
				
				m_vertices[i].p = m_particles[i].vPos;
				m_vertices[i].n = { 0.0f, 0.0f, 0.0f };
				m_vertices[i].c = curColor;
				m_vertices[i].t = { 0.0f, 0.0f };
				m_vertices[i].spriteRect = { m_UVRect.vMin.x, m_UVRect.vMin.y, m_UVRect.vMax.x, m_UVRect.vMax.y }; //Sprite UV Coord�� GS�ܰ迡�� ó���Ѵ�.
				m_vertices[i].rot = { cosf(DEG2RAD(m_particles[i].fRoll)), sinf(DEG2RAD(m_particles[i].fRoll)), 1.0f };
				m_vertices[i].scale = { m_particles[i].vScale.x, m_particles[i].vScale.y, 0.0f };
			}

			updateBuffer(m_pVBuf.Get(), &m_vertices.at(0), m_vertices.size() * sizeof(PointParticleVertex));
		}

		updateBuffer(m_pCBuf.Get(), &m_wvpMat, sizeof(CBUF_COORDCONV_MATSET));
		return true;
	}

	bool PointParticleEmitter::render()
	{
		if (m_eProp.bShow)
		{
			bindToPipeline();

			m_pDContext->Draw(m_iVertexCount, 0);
		}

		return true;
	}

	bool PointParticleEmitter::release()
	{
		m_pVLayout = nullptr;

		m_vertices.clear();
		m_particles.clear();

		return true;
	}

	void PointParticleEmitter::bindToPipeline()
	{
		m_pDContext->IASetVertexBuffers(0, 1, m_pVBuf.GetAddressOf(), &m_iStride, &m_iOffset);
		m_pDContext->IASetInputLayout(m_pVLayout);

		//�� ���ڸ� GS���� �ø� ���̹Ƿ� IA�������������� �Է� ���Ͽ�Ҵ� POINTLIST�� �����Ѵ�.
		m_pDContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		//VS, GS�� �������� ������ ����, ��, ���� ��� ������� ���ҽ� ���ε�
		m_pDContext->VSSetConstantBuffers(0, 1, m_pCBuf.GetAddressOf());
		m_pDContext->GSSetConstantBuffers(0, 1, m_pCBuf.GetAddressOf());

		if (m_pTexture)
		{
			ID3D11ShaderResourceView* pTemp = m_pTexture->GetResourceView();
			m_pDContext->PSSetShaderResources(0, 1, &pTemp);
		}

		switch (m_iBlendStateOption)
		{
		case BLENDSTATE_TYPE::BS_ALPHABLEND:
		{
			m_pDContext->OMSetBlendState(DXSTATE_MGR.getBState(L"BS_ALPHABLEND"), NULL, -1);
			m_shaderGroup.pPS = DXShaderManager::GetInstance()->GetPixelShader(L"PS_PointParticle_Alphablend");
		} break;

		case BLENDSTATE_TYPE::BS_DUALSOURCEBLEND:
		{
			m_pDContext->OMSetBlendState(DXSTATE_MGR.getBState(L"BS_DUALSOURCEBLEND"), NULL, -1);
			m_shaderGroup.pPS = DXShaderManager::GetInstance()->GetPixelShader(L"PS_PointParticle_Dualsource");
		} break;

		case BLENDSTATE_TYPE::BS_MSALPHATEST:
		{
			m_pDContext->OMSetBlendState(DXSTATE_MGR.getBState(L"BS_MSALPHATEST"), NULL, -1);
			m_shaderGroup.pPS = DXShaderManager::GetInstance()->GetPixelShader(L"PS_PointParticle_Alphatest");
		} break;

		default:
		{
			m_pDContext->OMSetBlendState(DXSTATE_MGR.getBState(L"BS_DEFAULT"), NULL, -1);
			m_shaderGroup.pPS = DXShaderManager::GetInstance()->GetPixelShader(L"PS_PointParticle_Alphablend");
		}
		}

		m_shaderGroup.bindToPipeline(m_pDContext);

		if (m_bEnableDepth)
		{
			if (m_bEnableDepthWrite)
			{
				m_pDContext->OMSetDepthStencilState(DXSTATE_MGR.getDSState(L"DSS_DEFAULT"), 1);
			}
			else
			{
				m_pDContext->OMSetDepthStencilState(DXSTATE_MGR.getDSState(L"DSS_COMPLESS_NOWRITE"), 1);
			}
		}
		else
		{
			m_pDContext->OMSetDepthStencilState(DXSTATE_MGR.getDSState(L"DSS_DISABLE"), 1);
		}
	}

	void PointParticleEmitter::setMatrix(const Matrix* pWorldM, const Matrix* pViewM, const Matrix* pProjM)
	{
		if (pWorldM) { m_matWorld = *pWorldM; }
		if (pViewM) { m_matView = *pViewM; }
		if (pProjM) { m_matProj = *pProjM; }

		//ī�޶� ȸ�� ����� ������� ����� ��������� �籸���Ѵ�.
		//������ �̵������� �״�� ������ �־�� �ϱ⿡ �̵� ������ �ݿ��Ѵ�.
		Matrix billBoardMat = m_matView.Invert();
		billBoardMat._41 = m_matWorld._41;
		billBoardMat._42 = m_matWorld._42;
		billBoardMat._43 = m_matWorld._43;

		m_matWorld = billBoardMat;

		//��ü�� ī�޶��� ������ ����ִ� ��� ������ �Լ��� ��� ����� �� �ִ�.
		//m_matWorld = Matrix::CreateBillboard();

		updateState();
		updateCoordConvMat();
	}

	void PointParticleEmitter::updateState()
	{
		//ȸ���� ���� ���� ������ ȸ������ ���ͷ� ǥ���ϱ⿡�� ������ ���ʿ��ϰ� ����.
		//������ �࿡ ���� ȸ���� ���� �Ǵ� �ٸ� ȸ�� ����� �������� ���� ���Ŀ� 
		//����� ������Ŀ��� �����ϴ� ���� �Ϲ����̴�.
		//�� �Լ��� ������ ������ ��ȭ�� �ݿ������� �ʴ´�.

		// ���� ��� R*T
		// Right.x	Right.y	Right.z	0
		// Up.x		Up.y	Up.z	0
		// Look.x	Look.y	Look.z	0
		// Pos.x	Pos.y	Pos.z	1

		//��ġ ������ ��Ŀ��� �����Ѵ�.
		m_vPrevPos = m_vPos;
		m_vPos = { m_matWorld._41, m_matWorld._42, m_matWorld._43 };

		//obj Right-Axis
		m_vRight = { m_matWorld._11, m_matWorld._12, m_matWorld._13 };

		//obj Up-Axis
		m_vUp = { m_matWorld._21, m_matWorld._22, m_matWorld._23 };

		//obj At-Axis
		m_vLook = { m_matWorld._31, m_matWorld._32 , m_matWorld._33 };

		//ȸ�� ��Ŀ��� ������ �� �຤�ʹ� ������ ��Ŀ� ���� ����ȭ�Ǿ� ���� ���� ���¶�� �����Ѵ�.
		m_vRight.Normalize();
		m_vUp.Normalize();
		m_vLook.Normalize();
	}

	void PointParticleEmitter::updateCoordConvMat()
	{
		m_matWorld.Transpose(m_wvpMat.matTWorld);
		m_matView.Transpose(m_wvpMat.matTView);
		m_matProj.Transpose(m_wvpMat.matTProj);

		updateBuffer(m_pCBuf.Get(), &m_wvpMat, sizeof(CBUF_COORDCONV_MATSET));
	}

	HRESULT PointParticleEmitter::createEmitter(PointParticleEmitterProperty eProp,
		std::wstring wszTexPathName)
	{
		HRESULT hr = S_OK;

		m_eProp = eProp;

		m_matWorld = Matrix::CreateTranslation(m_eProp.vInitPos);
		m_fSpawnTime = 1.0f / m_eProp.fSpawnRate;
		m_fTimer = 0.0f;

		DXTextureManager::GetInstance()->Load(wszTexPathName);

		m_pTexture = DXTextureManager::GetInstance()->GetTexture(wszTexPathName);

		if (!m_pTexture)
		{
			hr = E_FAIL; return hr;
		}

		m_pVLayout = DXShaderManager::GetInstance()->GetInputLayout(L"VS_PointParticle");

		m_vertices.clear();
		m_particles.clear();

		m_vertices.resize(m_eProp.iMaxParticleNum);
		m_particles.resize(m_eProp.iMaxParticleNum);

		m_iVertexCount = m_vertices.size();

		hr = createDXSimpleBuf(m_pDevice, m_iVertexCount * sizeof(PointParticleVertex), &m_vertices.at(0), D3D11_BIND_VERTEX_BUFFER, m_pVBuf.GetAddressOf());
		if (FAILED(hr)) { return hr; }

		hr = createDXSimpleBuf(m_pDevice, sizeof(CBUF_COORDCONV_MATSET), &m_wvpMat, D3D11_BIND_CONSTANT_BUFFER, m_pCBuf.GetAddressOf());

		return hr;
	}

	HRESULT PointParticleEmitter::createUVSpriteEmitter(PointParticleEmitterProperty eProp,
		std::wstring wszSpriteName)
	{
		HRESULT hr = S_OK;

		m_eProp = eProp;

		m_matWorld = Matrix::CreateTranslation(m_eProp.vInitPos);
		m_fSpawnTime = 1.0f / m_eProp.fSpawnRate;
		m_fTimer = 0.0f;

		m_pSprite = SPRITE_MGR.getUVPtr(wszSpriteName);

		if (!m_pSprite)
		{
			hr = E_FAIL; return hr;
		}

		m_pTexture = m_pSprite->m_pTexArr[0];

		m_pVLayout = DXShaderManager::GetInstance()->GetInputLayout(L"VS_PointParticle");

		m_vertices.clear();
		m_particles.clear();

		m_vertices.resize(m_eProp.iMaxParticleNum);
		m_particles.resize(m_eProp.iMaxParticleNum);

		m_iVertexCount = m_vertices.size();

		hr = createDXSimpleBuf(m_pDevice, m_iVertexCount * sizeof(PointParticleVertex), &m_vertices.at(0), D3D11_BIND_VERTEX_BUFFER, m_pVBuf.GetAddressOf());
		if (FAILED(hr)) { return hr; }

		hr = createDXSimpleBuf(m_pDevice, sizeof(CBUF_COORDCONV_MATSET), &m_wvpMat, D3D11_BIND_CONSTANT_BUFFER, m_pCBuf.GetAddressOf());

		return hr;
	}

	HRESULT PointParticleEmitter::createMTSpriteEmitter(PointParticleEmitterProperty eProp,
		std::wstring wszSpriteName)
	{
		HRESULT hr = S_OK;

		m_eProp = eProp;

		m_matWorld = Matrix::CreateTranslation(m_eProp.vInitPos);
		m_fSpawnTime = 1.0f / m_eProp.fSpawnRate;
		m_fTimer = 0.0f;

		m_pSprite = SPRITE_MGR.getMTPtr(wszSpriteName);

		if (!m_pSprite)
		{
			hr = E_FAIL; return hr;
		}

		m_pTexture = m_pSprite->m_pTexArr[0];

		m_pVLayout = DXShaderManager::GetInstance()->GetInputLayout(L"VS_PointParticle");

		m_vertices.clear();
		m_particles.clear();

		m_vertices.resize(m_eProp.iMaxParticleNum);
		m_particles.resize(m_eProp.iMaxParticleNum);

		m_iVertexCount = m_vertices.size();

		hr = createDXSimpleBuf(m_pDevice, m_iVertexCount * sizeof(PointParticleVertex), &m_vertices.at(0), D3D11_BIND_VERTEX_BUFFER, m_pVBuf.GetAddressOf());
		if (FAILED(hr)) { return hr; }

		hr = createDXSimpleBuf(m_pDevice, sizeof(CBUF_COORDCONV_MATSET), &m_wvpMat, D3D11_BIND_CONSTANT_BUFFER, m_pCBuf.GetAddressOf());

		return hr;
	}

	void PointParticleEmitter::setRenderOption(int iBlendStateOption,
		bool bEnableDepth,
		bool bEnableDepthWrite)
	{
		m_iBlendStateOption = iBlendStateOption;
		m_bEnableDepth = bEnableDepth;
		m_bEnableDepthWrite = bEnableDepthWrite;
	}

	HRESULT PointParticleEmitter::updateBuffer(ID3D11Resource* pBuf, void* pResource, UINT iBufSize)
	{
		HRESULT hr = S_OK;

		if (pBuf && pResource)
		{
			D3D11_MAPPED_SUBRESOURCE mappedRS;
			ZeroMemory(&mappedRS, sizeof(D3D11_MAPPED_SUBRESOURCE));

			hr = m_pDContext->Map(pBuf, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRS);

			if (SUCCEEDED(hr))
			{
				memcpy(mappedRS.pData, pResource, iBufSize);
				m_pDContext->Unmap(pBuf, 0);
			}
		}
		else { hr = E_POINTER; }

		return hr;
	}
}