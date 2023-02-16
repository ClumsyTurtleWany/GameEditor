#pragma once
#include "Define.h"

class DXShader
{
	// Device
	ID3D11Device*				m_pd3dDevice = nullptr;
	ID3D11DeviceContext*		m_pImmediateContext = nullptr;

public:
	ID3D11InputLayout*			InputLayout = nullptr;

	ID3D11VertexShader*			VertexShader = nullptr;
	ID3D11HullShader*			HullShader = nullptr;
	ID3D11DomainShader*			DomainShader = nullptr;
	ID3D11GeometryShader*		GeometryShader = nullptr;
	ID3D11PixelShader*			PixelShader = nullptr;

	ID3D11SamplerState*			SamplerState = nullptr;
	ID3D11RasterizerState*		RSState = nullptr;
	ID3D11BlendState*			BlendState = nullptr;
	ID3D11DepthStencilState*	DepthStencilState = nullptr;

	ID3D11Buffer*				VertexBuffer = nullptr;
	ID3D11Buffer*				IndexBuffer = nullptr;
	ID3D11Buffer*				TransformBuffer = nullptr;

	std::vector<Vertex>			VertexList;
	std::vector<DWORD>			IndexList;
	//VertexTransform				VertexTransformData;

public:
	bool SetInputLayout();
	bool SetShader();
	bool SetState();
};