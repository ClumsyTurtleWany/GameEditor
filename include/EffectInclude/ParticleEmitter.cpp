#include "ParticleEmitter.h"

namespace EFFECTUTIL
{
	ParticleEmitter::ParticleEmitter()
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

		m_pRState = nullptr;
		m_pBState = nullptr;
		m_pDSState = nullptr;

		m_UVRect = { {0.0f, 0.0f}, {1.0f, 1.0f} };

		m_playSpeed = 1.0f;
	}

	ParticleEmitter::~ParticleEmitter()
	{
	}

	bool ParticleEmitter::init()
	{
		return true;
	}

	bool ParticleEmitter::update()
	{
		float dt = Timer::GetInstance()->SecondPerFrame * m_playSpeed;

		activateTarget(dt);

		for (UINT i = 0; i < m_iVertexCount; i++)
		{
			m_particles[i].fElapsedTime += dt;

			if (!m_particles[i].bLoop)
			{
				if (m_particles[i].fElapsedTime > m_particles[i].fLifeSpan)
				{
					m_particles[i].bEnable = false;

					//������ ������ �Ӽ��� ������ ������ Ȱ��ȭ ���η� ����Ѵ�.
					m_vertices[i].scale.z = 0.0f;
				}

				if (m_particles[i].fElapsedTime <= m_particles[i].fLifeSpan)
				{
					updateParticleState(i, dt);
				}
			}
			else
			{
				updateParticleState(i, dt);
			}
		}

		updateBuffer(m_pVBuf.Get(), &m_vertices.at(0), m_vertices.size() * sizeof(PointParticleVertex));

		return true;
	}

	bool ParticleEmitter::render()
	{
		bindToPipeline();

		m_pDContext->Draw(m_iVertexCount, 0);

		return true;
	}

	bool ParticleEmitter::release()
	{
		m_pVLayout = nullptr;

		m_vertices.clear();
		m_particles.clear();

		return true;
	}

	void ParticleEmitter::bindToPipeline()
	{
		m_pDContext->IASetVertexBuffers(0, 1, m_pVBuf.GetAddressOf(), &m_iStride, &m_iOffset);
		m_pDContext->IASetInputLayout(m_pVLayout);

		//�� ���ڸ� GS���� �ø� ���̹Ƿ� IA�������������� �Է� ���Ͽ�Ҵ� POINTLIST�� �����Ѵ�.
		m_pDContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		//GS�� �������� ������ ����, ��, ���� ��� ������� ���ҽ� ���ε�
		m_pDContext->GSSetConstantBuffers(0, 1, m_pCBuf.GetAddressOf());
		m_pDContext->GSSetConstantBuffers(1, 1, m_pCBufBillboard.GetAddressOf());

		if (m_pTexture)
		{
			ID3D11ShaderResourceView* pTemp = m_pTexture->GetResourceView();
			m_pDContext->PSSetShaderResources(0, 1, &pTemp);
		}

		m_shaderGroup.bindToPipeline(m_pDContext);

		m_pDContext->PSSetSamplers(0, m_pSamplers.size(), m_pSamplers.data());

		m_pDContext->RSSetState(m_pRState);
		m_pDContext->OMSetBlendState(m_pBState, NULL, -1);
		m_pDContext->OMSetDepthStencilState(m_pDSState, 1);
	}

	void ParticleEmitter::setMatrix(const Matrix* pWorldM, const Matrix* pViewM, const Matrix* pProjM)
	{
		if (pWorldM) { m_matWorld = *pWorldM; }
		if (pViewM) { m_matView = *pViewM; }
		if (pProjM) { m_matProj = *pProjM; }

		//ī�޶� ȸ�� ����� ������� ����� ��������� �籸���Ѵ�.
		//������ �̵������� �״�� ������ �־�� �ϱ⿡ �̵� ������ �ݿ��Ѵ�.
		m_matBillboard.BiilTMat = m_matView.Invert();
		m_matBillboard.BiilTMat._41 = m_matWorld._41;
		m_matBillboard.BiilTMat._42 = m_matWorld._42;
		m_matBillboard.BiilTMat._43 = m_matWorld._43;
		m_matBillboard.BiilTMat = m_matBillboard.BiilTMat.Transpose();

		updateBuffer(m_pCBufBillboard.Get(), &m_matBillboard, sizeof(CBUF_BILLBOARDMAT));

		//if (m_eProp.bUseBillBoard)
		//{
		//	//ī�޶� ȸ�� ����� ������� ����� ��������� �籸���Ѵ�.
		//	//������ �̵������� �״�� ������ �־�� �ϱ⿡ �̵� ������ �ݿ��Ѵ�.
		//	m_matBillboard.BiilTMat = m_matView.Invert();
		//	m_matBillboard.BiilTMat = m_matBillboard.BiilTMat.Transpose();
		//}

		//��ü�� ī�޶��� ������ ����ִ� ��� ������ �Լ��� ��� ����� �� �ִ�.
		//m_matWorld = Matrix::CreateBillboard();

		updateState();
		updateCoordConvMat();
	}

	void ParticleEmitter::updateState()
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

	void ParticleEmitter::updateCoordConvMat()
	{
		m_matWorld.Transpose(m_wvpMat.matTWorld);
		m_matView.Transpose(m_wvpMat.matTView);
		m_matProj.Transpose(m_wvpMat.matTProj);

		updateBuffer(m_pCBuf.Get(), &m_wvpMat, sizeof(CBUF_COORDCONV_MATSET));
	}

	HRESULT ParticleEmitter::createEmitter(PointParticleEmitterProperty eProp,
		std::wstring wszTexPathName)
	{
		HRESULT hr = S_OK;

		m_eProp = eProp;

		m_matWorld = Matrix::CreateTranslation(m_eProp.vInitPos);
		m_fSpawnTime = 1.0f / m_eProp.fSpawnRate;
		m_fTimer = m_fSpawnTime;

		std::wstring wszTexName;
		getSplitName(wszTexPathName, wszTexName);

		std::wstring wszTexPath = L"../resource/EffectPart/texture/particle/AlphaSingle/";
		wszTexPath += wszTexName;

		DXTextureManager::GetInstance()->Load(wszTexPath);
		m_pTexture = DXTextureManager::GetInstance()->GetTexture(wszTexPath);

		if (!m_pTexture)
		{
			wszTexPath = L"../resource/EffectPart/texture/particle/NoAlphaSingle/";
			wszTexPath += wszTexName;

			DXTextureManager::GetInstance()->Load(wszTexPath);
			m_pTexture = DXTextureManager::GetInstance()->GetTexture(wszTexPath);

			if (!m_pTexture)
			{
				hr = E_FAIL; return hr;
			}
		}

		m_pVLayout = DXShaderManager::GetInstance()->GetInputLayout(L"VL_PointParticle");

		m_vertices.clear();
		m_particles.clear();

		m_vertices.resize(m_eProp.iMaxParticleNum);
		m_particles.resize(m_eProp.iMaxParticleNum);

		m_iVertexCount = m_vertices.size();

		hr = createDXSimpleBuf(m_pDevice, m_iVertexCount * sizeof(PointParticleVertex), &m_vertices.at(0), D3D11_BIND_VERTEX_BUFFER, m_pVBuf.GetAddressOf());
		if (FAILED(hr)) { return hr; }

		hr = createDXSimpleBuf(m_pDevice, sizeof(CBUF_COORDCONV_MATSET), &m_wvpMat, D3D11_BIND_CONSTANT_BUFFER, m_pCBuf.GetAddressOf());
		hr = createDXSimpleBuf(m_pDevice, sizeof(CBUF_BILLBOARDMAT), &m_matBillboard, D3D11_BIND_CONSTANT_BUFFER, m_pCBufBillboard.GetAddressOf());

		return hr;
	}

	HRESULT ParticleEmitter::createUVSpriteEmitter(PointParticleEmitterProperty eProp,
		std::wstring wszSpriteName)
	{
		HRESULT hr = S_OK;

		m_eProp = eProp;

		m_matWorld = Matrix::CreateTranslation(m_eProp.vInitPos);
		m_fSpawnTime = 1.0f / m_eProp.fSpawnRate;
		m_fTimer = m_fSpawnTime;

		m_pSprite = SPRITE_MGR.getUVPtr(wszSpriteName);

		if (!m_pSprite)
		{
			hr = E_FAIL; return hr;
		}

		m_pTexture = m_pSprite->m_pTexArr[0];

		m_pVLayout = DXShaderManager::GetInstance()->GetInputLayout(L"VL_PointParticle");

		m_vertices.clear();
		m_particles.clear();

		m_vertices.resize(m_eProp.iMaxParticleNum);
		m_particles.resize(m_eProp.iMaxParticleNum);

		m_iVertexCount = m_vertices.size();

		hr = createDXSimpleBuf(m_pDevice, m_iVertexCount * sizeof(PointParticleVertex), &m_vertices.at(0), D3D11_BIND_VERTEX_BUFFER, m_pVBuf.GetAddressOf());
		if (FAILED(hr)) { return hr; }

		hr = createDXSimpleBuf(m_pDevice, sizeof(CBUF_COORDCONV_MATSET), &m_wvpMat, D3D11_BIND_CONSTANT_BUFFER, m_pCBuf.GetAddressOf());
		hr = createDXSimpleBuf(m_pDevice, sizeof(CBUF_BILLBOARDMAT), &m_matBillboard, D3D11_BIND_CONSTANT_BUFFER, m_pCBufBillboard.GetAddressOf());

		return hr;
	}

	HRESULT ParticleEmitter::createMTSpriteEmitter(PointParticleEmitterProperty eProp,
		std::wstring wszSpriteName)
	{
		HRESULT hr = S_OK;

		m_eProp = eProp;

		m_matWorld = Matrix::CreateTranslation(m_eProp.vInitPos);
		m_fSpawnTime = 1.0f / m_eProp.fSpawnRate;
		m_fTimer = m_fSpawnTime;

		m_pSprite = SPRITE_MGR.getMTPtr(wszSpriteName);

		if (!m_pSprite)
		{
			hr = E_FAIL; return hr;
		}

		m_pTexture = m_pSprite->m_pTexArr[0];

		m_pVLayout = DXShaderManager::GetInstance()->GetInputLayout(L"VL_PointParticle");

		m_vertices.clear();
		m_particles.clear();

		m_vertices.resize(m_eProp.iMaxParticleNum);
		m_particles.resize(m_eProp.iMaxParticleNum);

		m_iVertexCount = m_vertices.size();

		hr = createDXSimpleBuf(m_pDevice, m_iVertexCount * sizeof(PointParticleVertex), &m_vertices.at(0), D3D11_BIND_VERTEX_BUFFER, m_pVBuf.GetAddressOf());
		if (FAILED(hr)) { return hr; }

		hr = createDXSimpleBuf(m_pDevice, sizeof(CBUF_COORDCONV_MATSET), &m_wvpMat, D3D11_BIND_CONSTANT_BUFFER, m_pCBuf.GetAddressOf());
		hr = createDXSimpleBuf(m_pDevice, sizeof(CBUF_BILLBOARDMAT), &m_matBillboard, D3D11_BIND_CONSTANT_BUFFER, m_pCBufBillboard.GetAddressOf());

		return hr;
	}

	void ParticleEmitter::setRenderOption(int iBlendStateOption,
		bool bEnableDepth,
		bool bEnableDepthWrite,
		bool bBackCull)
	{
		if (bBackCull)
		{
			m_pRState = DXSTATE_MGR.getRState(L"RS_SOLID");
		}
		else
		{
			m_pRState = DXSTATE_MGR.getRState(L"RS_SOLID_NOCULL");
		}

		m_pSamplers.push_back(DXSTATE_MGR.getSState(L"SS_LINEAR_WRAP"));

		m_shaderGroup.pVS = DXShaderManager::GetInstance()->GetVertexShader(L"VS_PointParticle");
		m_shaderGroup.pGS = DXShaderManager::GetInstance()->GetGeometryShader(L"GS_PointParticle");

		switch (iBlendStateOption)
		{
		case BLENDSTATE_TYPE::BS_ALPHABLEND:
		{
			m_pBState = DXSTATE_MGR.getBState(L"BS_ALPHABLEND");
			m_shaderGroup.pPS = DXShaderManager::GetInstance()->GetPixelShader(L"PS_PointParticle_Alphablend");
		} break;

		case BLENDSTATE_TYPE::BS_DUALSOURCEBLEND:
		{
			m_pBState = DXSTATE_MGR.getBState(L"BS_DUALSOURCEBLEND");
			m_shaderGroup.pPS = DXShaderManager::GetInstance()->GetPixelShader(L"PS_PointParticle_Dualsource");
		} break;

		case BLENDSTATE_TYPE::BS_MSALPHATEST:
		{
			m_pBState = DXSTATE_MGR.getBState(L"BS_MSALPHATEST");
			m_shaderGroup.pPS = DXShaderManager::GetInstance()->GetPixelShader(L"PS_PointParticle_AlphaTest");
		} break;

		default:
		{
			m_pBState = DXSTATE_MGR.getBState(L"BS_DEFAULT");
			m_shaderGroup.pPS = DXShaderManager::GetInstance()->GetPixelShader(L"PS_PointParticle_Alphablend");
		}
		}

		if (bEnableDepth)
		{
			if (bEnableDepthWrite)
			{
				m_pDSState = DXSTATE_MGR.getDSState(L"DSS_DEFAULT");
			}
			else
			{
				m_pDSState = DXSTATE_MGR.getDSState(L"DSS_COMPLESS_NOWRITE");
			}
		}
		else
		{
			m_pDSState = DXSTATE_MGR.getDSState(L"DSS_DISABLE");
		}

	}

	HRESULT ParticleEmitter::updateBuffer(ID3D11Resource* pBuf, void* pResource, UINT iBufSize)
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
	void ParticleEmitter::activateTarget(float dt)
	{
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
	}
	void ParticleEmitter::updateParticleState(int idx, float dt)
	{
		if (m_pSprite)
		{
			UINT animIdx = (m_particles[idx].fElapsedTime / m_particles[idx].fLifeSpan) * m_pSprite->m_spriteInfo.m_iTotalFrame;
			if (animIdx >= m_pSprite->m_spriteInfo.m_iTotalFrame)
			{
				animIdx = m_pSprite->m_spriteInfo.m_iTotalFrame - 1;
			}

			if (m_pSprite->m_spriteInfo.m_spriteType == SPRITE_TYPE::SPRITE_UV)
			{
				m_UVRect = m_pSprite->m_UVSpriteArr[animIdx];
				m_pTexture = m_pSprite->m_pTexArr[0];
			}
			else
			{
				m_pTexture = m_pSprite->m_pTexArr[animIdx];
			}
		}

		m_particles[idx].vVelocity += m_particles[idx].vAccelelation * dt;
		m_particles[idx].vPos = m_particles[idx].vPos + m_particles[idx].vVelocity * dt;

		m_particles[idx].fPYR += m_particles[idx].fPYRVelocity * dt;

		float T = m_particles[idx].fElapsedTime / m_particles[idx].fLifeSpan;
		float invT = (m_particles[idx].fLifeSpan - m_particles[idx].fElapsedTime) / m_particles[idx].fLifeSpan;

		m_particles[idx].vScale =
		{
			m_particles[idx].vInitScale.x * (m_eProp.fScaleOverLifespan.x * invT + m_eProp.fScaleOverLifespan.z * T),
			m_particles[idx].vInitScale.y * (m_eProp.fScaleOverLifespan.y * invT + m_eProp.fScaleOverLifespan.w * T)
		};

		Color curColor = Color::Lerp(m_particles[idx].initColor, m_particles[idx].lastColor, T);

		m_vertices[idx].p = m_particles[idx].vPos;
		m_vertices[idx].n = { 0.0f, 0.0f, 0.0f };
		m_vertices[idx].c = curColor;
		m_vertices[idx].t = { 0.0f, 0.0f };
		m_vertices[idx].spriteRect = { m_UVRect.vMin.x, m_UVRect.vMin.y, m_UVRect.vMax.x, m_UVRect.vMax.y }; //Sprite UV Coord�� GS�ܰ迡�� ó���Ѵ�.
		m_vertices[idx].rot = DirectX::XMMatrixRotationRollPitchYawFromVector(m_particles[idx].fPYR);
		m_vertices[idx].rot = m_vertices[idx].rot.Transpose();
		m_vertices[idx].scale = { m_particles[idx].vScale.x, m_particles[idx].vScale.y, 1.0f };
	}
}