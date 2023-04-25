#pragma once
#include <atlconv.h>
#include <set>
#include <fstream>
#include <unordered_set>

#include "Define.h"
#include "RNGenerator.h"

namespace EFFECTUTIL
{
	//디버그 창에 DX리소스의 생성 오류 코드 출력 함수
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
	#define EFFECT_PROP_PATH_LIMIT			256

	//wide 문자열(유니코드) / 멀티바이트 문자열 변환 함수
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

	//외부 버퍼 생성시 사용
	inline HRESULT createDXSimpleBuf(ID3D11Device* pDevice,
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
		bd.Usage = D3D11_USAGE_DYNAMIC;						//GPU는 읽기전용, CPU는 쓰기 전용으로 액세스 지정
		bd.BindFlags = bindType;							//정점 버퍼로 사용 - DX Map함수 사용을 위한 설정
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;			//CPU 쓰기 가능 - DX Map함수 사용을 위한 설정

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
		sd.pSysMem = pResource;

		hr = pDevice->CreateBuffer(&bd, &sd, ppOut);

		return hr;
	}

	inline HRESULT createDXBuf(ID3D11Device* pDevice,
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
		Matrix	rot;
		Vector3 scale;
	};

	struct PointParticleEmitterProperty
	{
		//파티클 초기 트랜스폼
		Vector3			vInitPos;
		Vector3			vMinPosOffset;
		Vector3			vMaxPosOffset;

		Vector3			fInitPYR;

		Vector3			fExtraPYRMinRange;
		Vector3			fExtraPYRMaxRange;

		Vector2			vInitScale;
		Vector4			vExtraScaleRange;

		//파티클 스폰
		int				iMaxParticleNum;
		float			fSpawnRate;
		Vector2			fLifespanRange;

		//파티클 상태 변화
		Vector3			vInitMinVelocity;
		Vector3			vInitMaxVelocity;

		Vector3			fMinPYRVelocity;
		Vector3			fMaxPYRVelocity;

		Vector4			fScaleOverLifespan;

		Vector3			vInitMinAcceleration;
		Vector3			vInitMaxAcceleration;

		Color			initColorOverLifespan;
		Color			lastColorOverLifespan;

		//사용할 빌보드 행렬 
		int					iUseBillBoard;

		//파티클 가시 여부
		bool				bShow;

		//루프 여부
		bool				bLoop;

		PointParticleEmitterProperty()
		{
			bShow = true;
			bLoop = false;
			iUseBillBoard = 0;
			vInitScale = { 1.0f, 1.0f };
			iMaxParticleNum = 0;
			fSpawnRate = 0.0f;
		}
	};

	struct PointParticle
	{
		bool			bEnable;
		bool			bLoop;

		Vector3			vPos;
		Vector3			vVelocity;
		Vector3			vAccelelation;
		Vector2			vInitScale;
		Vector2			vScale;

		Vector3			fPYR;
		Vector3			fPYRVelocity;

		Color			initColor;
		Color			lastColor;

		float			fLifeSpan;
		float			fElapsedTime;

		PointParticle()
		{
			bEnable = false;
			bLoop = false;
			fLifeSpan = 0.0f;
			fElapsedTime = 0.0f;

			initColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			lastColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		}

		void setParticle(PointParticleEmitterProperty& eProp)
		{
			bEnable = true;
			bLoop = eProp.bLoop;
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

			fPYR =
			{
				DEG2RAD(eProp.fInitPYR.x + RNG.getF(eProp.fExtraPYRMinRange.x, eProp.fExtraPYRMaxRange.x)),
				DEG2RAD(eProp.fInitPYR.y + RNG.getF(eProp.fExtraPYRMinRange.y, eProp.fExtraPYRMaxRange.y)),
				DEG2RAD(eProp.fInitPYR.z + RNG.getF(eProp.fExtraPYRMinRange.z, eProp.fExtraPYRMaxRange.z))
			};

			fPYRVelocity =
			{
				DEG2RAD(RNG.getF(eProp.fMinPYRVelocity.x, eProp.fMaxPYRVelocity.x)),
				DEG2RAD(RNG.getF(eProp.fMinPYRVelocity.y, eProp.fMaxPYRVelocity.y)),
				DEG2RAD(RNG.getF(eProp.fMinPYRVelocity.z, eProp.fMaxPYRVelocity.z))
			};

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

	struct POINTPARTICLEEMITTER_FILE
	{
		WCHAR							wszEmitterName[EFFECT_PROP_PATH_LIMIT];

		WCHAR							wszSpriteName[EFFECT_PROP_PATH_LIMIT];
		WCHAR							wszSpritePath[EFFECT_PROP_PATH_LIMIT];

		WCHAR							wszTextureName[EFFECT_PROP_PATH_LIMIT];
		WCHAR							wszTexturePath[EFFECT_PROP_PATH_LIMIT];

		Vector3							vEmitterPos;

		UINT							iSpriteType;
		UINT							spriteRC[2];

		int								iBlendStateOption;
		bool							bEnableDepth;
		bool							bEnableDepthWrite;
		bool							bBackCull;

		PointParticleEmitterProperty			eProp;
	};

	struct PARTICLESYSTEM_FILE
	{
		WCHAR						wszPSystemName[256];
		POINTPARTICLEEMITTER_FILE	emitters[64];
		int							iEmiiterCnt;
	};
}