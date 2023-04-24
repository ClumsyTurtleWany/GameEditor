#include "ParticleEmitter.h"

namespace EFFECTUTIL
{
	ParticleEmitter::ParticleEmitter()
	{
		//파티클 생성 타이머
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

					//정점의 스케일 속성의 마지막 성분을 활성화 여부로 사용한다.
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

		//점 입자를 GS에서 늘릴 것이므로 IA스테이지에서의 입력 기하요소는 POINTLIST로 설정한다.
		m_pDContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		//GS에 공통으로 적용할 월드, 뷰, 투영 행렬 상수버퍼 리소스 바인딩
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

		//카메라 회전 행렬의 역행렬을 사용해 월드행렬을 재구성한다.
		//기존의 이동정보는 그대로 가지고 있어야 하기에 이동 성분을 반영한다.
		m_matBillboard.BiilTMat = m_matView.Invert();
		m_matBillboard.BiilTMat._41 = m_matWorld._41;
		m_matBillboard.BiilTMat._42 = m_matWorld._42;
		m_matBillboard.BiilTMat._43 = m_matWorld._43;
		m_matBillboard.BiilTMat = m_matBillboard.BiilTMat.Transpose();

		updateBuffer(m_pCBufBillboard.Get(), &m_matBillboard, sizeof(CBUF_BILLBOARDMAT));

		//if (m_eProp.bUseBillBoard)
		//{
		//	//카메라 회전 행렬의 역행렬을 사용해 월드행렬을 재구성한다.
		//	//기존의 이동정보는 그대로 가지고 있어야 하기에 이동 성분을 반영한다.
		//	m_matBillboard.BiilTMat = m_matView.Invert();
		//	m_matBillboard.BiilTMat = m_matBillboard.BiilTMat.Transpose();
		//}

		//객체가 카메라의 정보를 들고있는 경우 다음의 함수를 대신 사용할 수 있다.
		//m_matWorld = Matrix::CreateBillboard();

		updateState();
		updateCoordConvMat();
	}

	void ParticleEmitter::updateState()
	{
		//회전을 정규 기저 벡터의 회전값의 벡터로 표현하기에는 연산이 불필요하게 많다.
		//임의의 축에 대한 회전을 수행 또는 다른 회전 행렬의 복합적인 곱셈 이후에 
		//적용된 최종행렬에서 추출하는 것이 일반적이다.
		//이 함수는 복잡한 스케일 변화를 반영하지는 않는다.

		// 월드 행렬 R*T
		// Right.x	Right.y	Right.z	0
		// Up.x		Up.y	Up.z	0
		// Look.x	Look.y	Look.z	0
		// Pos.x	Pos.y	Pos.z	1

		//전치 이전의 행렬에서 추출한다.
		m_vPrevPos = m_vPos;
		m_vPos = { m_matWorld._41, m_matWorld._42, m_matWorld._43 };

		//obj Right-Axis
		m_vRight = { m_matWorld._11, m_matWorld._12, m_matWorld._13 };

		//obj Up-Axis
		m_vUp = { m_matWorld._21, m_matWorld._22, m_matWorld._23 };

		//obj At-Axis
		m_vLook = { m_matWorld._31, m_matWorld._32 , m_matWorld._33 };

		//회전 행렬에서 추출한 각 축벡터는 스케일 행렬에 의해 정규화되어 있지 않은 상태라고 가정한다.
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
		m_vertices[idx].spriteRect = { m_UVRect.vMin.x, m_UVRect.vMin.y, m_UVRect.vMax.x, m_UVRect.vMax.y }; //Sprite UV Coord는 GS단계에서 처리한다.
		m_vertices[idx].rot = DirectX::XMMatrixRotationRollPitchYawFromVector(m_particles[idx].fPYR);
		m_vertices[idx].rot = m_vertices[idx].rot.Transpose();
		m_vertices[idx].scale = { m_particles[idx].vScale.x, m_particles[idx].vScale.y, 1.0f };
	}
}