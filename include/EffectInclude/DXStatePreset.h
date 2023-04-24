#pragma once
#include "EffectDefine.h"

namespace EFFECTUTIL
{
	enum SAMPLERSTATE_TYPE
	{
		SS_DEFAULT = 0,
		SS_LINEAR_WRAP,
		SS_POINT_WRAP,
		SS_NUMBER_OF_TYPE
	};

	enum RASTERIZERSTATE_TYPE
	{
		RS_SOLID = 0,
		RS_WIREFRAME,
		RS_NUMBER_OF_TYPE
	};

	enum BLENDSTATE_TYPE
	{
		BS_DEFAULT = 0,
		BS_MSALPHATEST,
		BS_ALPHABLEND,
		BS_DUALSOURCEBLEND,
		//BS_ADDITIVEBLEND,
		BS_NUMBER_OF_TYPE
	};

	enum DEPTHSTENCILSTATE_TYPE
	{
		DSS_DEFAULT = 0,
		DSS_COMPGREATER,
		DSS_COMPLESS_NOWRITE,
		DSS_DISABLE,
		DSS_NUMBER_OF_TYPE
	};

	class CPGCDXStateManager
	{
	private:
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDContext;

		std::map<std::wstring, ID3D11SamplerState*>			m_SStateMap;
		std::map<std::wstring, ID3D11RasterizerState*>		m_RStateMap;
		std::map<std::wstring, ID3D11BlendState*>			m_BStateMap;
		std::map<std::wstring, ID3D11DepthStencilState*>	m_DSStateMap;

		CPGCDXStateManager();
		~CPGCDXStateManager();

		HRESULT												createDefaultStates();

	public:
		CPGCDXStateManager(const CPGCDXStateManager& other) = delete;
		CPGCDXStateManager& operator=(const CPGCDXStateManager& other) = delete;

	public:
		bool init(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext);
		bool release();

		HRESULT						createSState(std::wstring wszSStateName, D3D11_SAMPLER_DESC samplerDesc);
		HRESULT						createRState(std::wstring wszRStateName, D3D11_RASTERIZER_DESC rasterizerDesc);
		HRESULT						createBState(std::wstring wszBStateName, D3D11_BLEND_DESC blendDesc);
		HRESULT						createDSState(std::wstring wszDSStateName, D3D11_DEPTH_STENCIL_DESC depthStencilDesc);

		ID3D11SamplerState* getSState(std::wstring wszSStateName);
		ID3D11RasterizerState* getRState(std::wstring wszRStateName);
		ID3D11BlendState* getBState(std::wstring wszBStateName);
		ID3D11DepthStencilState* getDSState(std::wstring wszDSStateName);

		static CPGCDXStateManager& getInstance()
		{
			static CPGCDXStateManager singleInst;
			return singleInst;
		}
	};

	#define DXSTATE_MGR CPGCDXStateManager::getInstance()
}