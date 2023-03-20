#pragma once
#include "SpriteManager.h"
#include "DXStatePreset.h"
#include "FileIOManager.h"

namespace EFFECTUTIL
{
	class EffectEmitter
	{
	public:
		std::string									m_szName;
		std::wstring								m_wszName;
		ID3D11Device*								m_pDevice;
		ID3D11DeviceContext*						m_pDContext;

	public:
		EffectEmitter() 
		{
			m_pDevice = nullptr;
			m_pDContext = nullptr;
		}
		virtual ~EffectEmitter() {}

		const WCHAR* getWszName()
		{
			if (m_wszName.size()) { return m_wszName.c_str(); }
			return nullptr;
		}

		const CHAR* getSzName()
		{
			if (m_szName.size()) { return m_szName.c_str(); }
			return nullptr;
		}

		void setName(std::wstring wszName)
		{
			m_wszName = wszName;
			m_szName = EFFECTUTIL::atl_W2M(wszName);
		}

		void setName(std::string szName)
		{
			m_szName = szName;
			m_wszName = EFFECTUTIL::atl_M2W(szName);
		}

		virtual void setDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext)
		{
			m_pDevice = pDevice;
			m_pDContext = pDContext;
		}
	};

	struct CBUF_BILLBOARDMAT
	{
		Matrix BiilTMat;
	};

	class ParticleEmitter : public EffectEmitter
	{
	public:
		//이미터의 위치
		Vector3											m_vPos;
		Vector3											m_vPrevPos;

		//이미터의 회전
		Vector3											m_vRight;
		Vector3											m_vUp;
		Vector3											m_vLook;

		CBUF_COORDCONV_MATSET							m_wvpMat;

		//좌표계 변환 행렬
		Matrix											m_matWorld;
		Matrix											m_matView;
		Matrix											m_matProj;

		//파티클 생성 타이머
		float											m_fSpawnTime;
		float											m_fTimer;

		//파티클의 속성 정보
		PointParticleEmitterProperty					m_eProp;

		ShaderGroup										m_shaderGroup;
		EffectSprite*									m_pSprite;
		DXTexture*										m_pTexture;
		UVRECT											m_UVRect;

		//렌더 옵션
		//파티클 리스트 : 일단 리스트의 순서는 정점 리스트의 순서와 동일하게 처리한다.
		std::vector<PointParticle>						m_particles;

		//파티클 정점 정보
		std::vector<PointParticleVertex>				m_vertices;
		UINT											m_iVertexCount;
		UINT											m_iStride;
		UINT											m_iOffset;

		//빌보드 행렬
		CBUF_BILLBOARDMAT								m_matBillboard;

		//버퍼 정보
		Microsoft::WRL::ComPtr<ID3D11Buffer>			m_pVBuf;
		Microsoft::WRL::ComPtr<ID3D11Buffer>			m_pCBuf;
		Microsoft::WRL::ComPtr<ID3D11Buffer>			m_pCBufBillboard;

		//정점 레이아웃 정보
		ID3D11InputLayout*								m_pVLayout;

		//시스템에서 넘겨받는 재생속도
		float											m_playSpeed;

		//렌더 옵션 프리셋 사용여부
		int												m_iBlendStateOption;
		bool											m_bEnableDepth;
		bool											m_bEnableDepthWrite;

	public:
		ParticleEmitter();
		virtual ~ParticleEmitter();

		virtual bool	init();
		virtual bool	update();
		virtual bool	render();
		virtual bool	release();

		virtual void	bindToPipeline();

		virtual void	setMatrix(const Matrix* pWorldM, const Matrix* pViewM, const Matrix* pProjM);
		virtual void	updateState();
		virtual void	updateCoordConvMat();

		virtual HRESULT	createEmitter(PointParticleEmitterProperty eProp,
			std::wstring wszTexPathName);

		virtual HRESULT	createUVSpriteEmitter(PointParticleEmitterProperty eProp,
			std::wstring wszSpriteName);

		virtual HRESULT	createMTSpriteEmitter(PointParticleEmitterProperty eProp,
			std::wstring wszSpriteName);

		virtual void	setRenderOption(int iBlendStateOption,
			bool bEnableDepth,
			bool bEnableDepthWrite);

		virtual HRESULT updateBuffer(ID3D11Resource* pBuf, void* pResource, UINT iBufSize);
	};
}

