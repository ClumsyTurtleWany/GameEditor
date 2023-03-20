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
		//�̹����� ��ġ
		Vector3											m_vPos;
		Vector3											m_vPrevPos;

		//�̹����� ȸ��
		Vector3											m_vRight;
		Vector3											m_vUp;
		Vector3											m_vLook;

		CBUF_COORDCONV_MATSET							m_wvpMat;

		//��ǥ�� ��ȯ ���
		Matrix											m_matWorld;
		Matrix											m_matView;
		Matrix											m_matProj;

		//��ƼŬ ���� Ÿ�̸�
		float											m_fSpawnTime;
		float											m_fTimer;

		//��ƼŬ�� �Ӽ� ����
		PointParticleEmitterProperty					m_eProp;

		ShaderGroup										m_shaderGroup;
		EffectSprite*									m_pSprite;
		DXTexture*										m_pTexture;
		UVRECT											m_UVRect;

		//���� �ɼ�
		//��ƼŬ ����Ʈ : �ϴ� ����Ʈ�� ������ ���� ����Ʈ�� ������ �����ϰ� ó���Ѵ�.
		std::vector<PointParticle>						m_particles;

		//��ƼŬ ���� ����
		std::vector<PointParticleVertex>				m_vertices;
		UINT											m_iVertexCount;
		UINT											m_iStride;
		UINT											m_iOffset;

		//������ ���
		CBUF_BILLBOARDMAT								m_matBillboard;

		//���� ����
		Microsoft::WRL::ComPtr<ID3D11Buffer>			m_pVBuf;
		Microsoft::WRL::ComPtr<ID3D11Buffer>			m_pCBuf;
		Microsoft::WRL::ComPtr<ID3D11Buffer>			m_pCBufBillboard;

		//���� ���̾ƿ� ����
		ID3D11InputLayout*								m_pVLayout;

		//�ý��ۿ��� �Ѱܹ޴� ����ӵ�
		float											m_playSpeed;

		//���� �ɼ� ������ ��뿩��
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

