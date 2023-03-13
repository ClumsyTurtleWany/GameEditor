#pragma once
#include <atlconv.h>
#include <set>
#include <fstream>

#include "Define.h"
#include "RNGenerator.h"

namespace EFFECTUTIL
{
	//����� â�� DX���ҽ��� ���� ���� �ڵ� ��� �Լ�
	inline void printCreateErr(UINT iErr)
	{
		std::wstring errStr = L"Error Code : ";
		errStr += std::to_wstring(iErr) + L"\n";
		OutputDebugString(errStr.c_str());
	}

	#define VNAME2WSTR(id)						(L#id)
	#define VNAME2STR(id)						(#id)

	#define PI								3.1415926f
	#define TAU								6.2831853f
	
	#define RAD2DEG(x)						(x)*(180.0f/PI)
	#define DEG2RAD(x)						(x)*(PI/180.0f)
	
	#define FZERO_BOUND						1.0E-5F
	
	#define RAY_LENGTH_LIMIT				10000.0f

	//wide ���ڿ�(�����ڵ�) / ��Ƽ����Ʈ ���ڿ� ��ȯ �Լ�
	//ATL (Active Template Library)
	static std::wstring atl_M2W(const std::string& multibyteStr)
	{
		USES_CONVERSION;
		return std::wstring(A2W(multibyteStr.c_str()));
	};

	static std::string atl_W2M(const std::wstring& widebyteStr)
	{
		USES_CONVERSION;
		return std::string(W2A(widebyteStr.c_str()));
	};

	//�ܺ� ���� ������ ���
	HRESULT createDXSimpleBuf(ID3D11Device* pDevice,
		size_t iBufSize,
		void* pResource,
		D3D11_BIND_FLAG bindType,
		ID3D11Buffer** ppOut)
	{
		HRESULT hr = S_OK;

		if (!pDevice || !pResource || !ppOut) { hr = E_POINTER; return hr; }

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

		bd.ByteWidth = iBufSize;
		bd.Usage = D3D11_USAGE_DYNAMIC;						//GPU�� �б�����, CPU�� ���� �������� �׼��� ����
		bd.BindFlags = bindType;							//���� ���۷� ��� - DX Map�Լ� ����� ���� ����
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;			//CPU ���� ���� - DX Map�Լ� ����� ���� ����

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
		sd.pSysMem = pResource;

		hr = pDevice->CreateBuffer(&bd, &sd, ppOut);

		return hr;
	}

	HRESULT createDXBuf(ID3D11Device* pDevice,
		D3D11_BUFFER_DESC* pBd,
		D3D11_SUBRESOURCE_DATA* pSd,
		ID3D11Buffer** ppOut)
	{
		HRESULT hr = S_OK;

		if (!pDevice || !pBd || !pSd || !ppOut) { hr = E_POINTER; return hr; }
		hr = pDevice->CreateBuffer(pBd, pSd, ppOut);

		return hr;
	}

	struct ShaderGroup
	{
		ID3D11VertexShader*		pVS;
		ID3D11HullShader*		pHS;
		ID3D11DomainShader*		pDS;
		ID3D11GeometryShader*	pGS;
		ID3D11PixelShader*		pPS;
		ID3D11ComputeShader*	pCS;

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

	struct CBUF_COORDCONV_MATSET
	{
		Matrix	matTWorld;
		Matrix	matTView;
		Matrix	matTProj;
	};

	struct PointParticleVertex
	{
		Vector3 p;
		Vector3 n;
		Vector4 c;
		Vector2 t;
		Vector4 spriteRect;
		Vector3 rot;
		Vector3 scale;
	};

	struct PointParticleEmitterProperty
	{
		//��ƼŬ �ʱ� Ʈ������
		Vector3			vInitPos;
		Vector3			vMinPosOffset;
		Vector3			vMaxPosOffset;

		float			fInitRot;
		Vector2			fExtraRotRange;

		Vector2			vInitScale;
		Vector4			vExtraScaleRange;

		//��ƼŬ ����
		int				iMaxParticleNum;
		float			fSpawnRate;
		Vector2			fLifespanRange;

		//��ƼŬ ���� ��ȭ
		Vector3			vInitMinVelocity;
		Vector3			vInitMaxVelocity;
		Vector2			fRotVelocity;
		Vector4			fScaleOverLifespan;

		Vector3			vInitMinAcceleration;
		Vector3			vInitMaxAcceleration;

		Color			initColorOverLifespan;
		Color			lastColorOverLifespan;

		bool			bShow;

		PointParticleEmitterProperty()
		{
			bShow = true;
			fInitRot = 0.0f;
			vInitScale = { 1.0f, 1.0f };
			iMaxParticleNum = 0;
			fSpawnRate = 0.0f;
		}
	};

	struct PointParticle
	{
		bool			bEnable;

		Vector3			vPos;
		Vector3			vVelocity;
		Vector3			vAccelelation;
		Vector2			vInitScale;
		Vector2			vScale;

		float			fRoll;
		float			fRotVelocity;

		Color			initColor;
		Color			lastColor;

		float			fLifeSpan;
		float			fElapsedTime;

		PointParticle()
		{
			bEnable = false;
			fRoll = 0.0f;
			fRotVelocity = 0.0f;
			fLifeSpan = 0.0f;
			fElapsedTime = 0.0f;

			initColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			lastColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		}

		void setParticle(PointParticleEmitterProperty& eProp)
		{
			bEnable = true;
			fElapsedTime = 0.0f;
			vPos =
			{
				eProp.vInitPos.x + RNG.getF(eProp.vMinPosOffset.x, eProp.vMaxPosOffset.x),
				eProp.vInitPos.y + RNG.getF(eProp.vMinPosOffset.y, eProp.vMaxPosOffset.y),
				eProp.vInitPos.z + RNG.getF(eProp.vMinPosOffset.z, eProp.vMaxPosOffset.z)
			};

			vVelocity =
			{
				RNG.getF(eProp.vInitMinVelocity.x, eProp.vInitMaxVelocity.x),
				RNG.getF(eProp.vInitMinVelocity.y, eProp.vInitMaxVelocity.y),
				RNG.getF(eProp.vInitMinVelocity.z, eProp.vInitMaxVelocity.z)
			};

			vAccelelation =
			{
				RNG.getF(eProp.vInitMinAcceleration.x, eProp.vInitMaxAcceleration.x),
				RNG.getF(eProp.vInitMinAcceleration.y, eProp.vInitMaxAcceleration.y),
				RNG.getF(eProp.vInitMinAcceleration.z, eProp.vInitMaxAcceleration.z)
			};

			fRoll = eProp.fInitRot + RNG.getF(eProp.fExtraRotRange.x, eProp.fExtraRotRange.y);
			fRotVelocity = RNG.getF(eProp.fRotVelocity.x, eProp.fRotVelocity.y);
			vInitScale =
			{
				(eProp.vInitScale.x + RNG.getF(eProp.vExtraScaleRange.x, eProp.vExtraScaleRange.z)) * eProp.fScaleOverLifespan.x,
				(eProp.vInitScale.y + RNG.getF(eProp.vExtraScaleRange.y, eProp.vExtraScaleRange.w)) * eProp.fScaleOverLifespan.y
			};

			vScale = vInitScale;

			initColor = eProp.initColorOverLifespan;
			lastColor = eProp.lastColorOverLifespan;
			fLifeSpan = RNG.getF(eProp.fLifespanRange.x, eProp.fLifespanRange.y);
		}
	};
}