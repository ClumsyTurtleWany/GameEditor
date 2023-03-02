#pragma once
#include "Define.h"

// Direct X -> Texture Tool Kit
#include "DXTK/WICTextureLoader.h"
#include "DXTK/DDSTextureLoader.h"

#ifdef _DEBUG
#pragma comment(lib, "DirectXTK_d.lib")
#else
#pragma comment(lib, "DirectXTK_r.lib")
#endif

class DXTexture
{
private:
	ID3D11Device*				m_pd3dDevice = nullptr;
	ID3D11DeviceContext*		m_pImmediateContext = nullptr;
	
private:
	// Texture
	ID3D11ShaderResourceView*	m_pTextureResourceView;
	//ID3D11Resource*			m_pTextureResource;
	ID3D11Texture2D*			m_pTextureResource;
	D3D11_TEXTURE2D_DESC		m_Desc;
	ID3D11UnorderedAccessView*	m_pTextureUAV;

private:
	std::wstring				m_wstrFileName;

private:
	std::vector<UINT>			MappedResourceData;

public:
	void						setDevice(ID3D11Device* _device, ID3D11DeviceContext* _context);

public:
	HRESULT						Load(std::wstring _filename);
	HRESULT						LoadEX(std::wstring _filename);
	
public:
	ID3D11Texture2D*			GetTexture2D();
	ID3D11Resource*				getResource();
	ID3D11ShaderResourceView*	getResourceView();
	ID3D11UnorderedAccessView*	GetUAV();

public:
	float						getWidth();
	float						getHeight();
	std::wstring				getFileName();
	std::vector<UINT>&			getMappedResource();

public:
	void SetTexture2D(ID3D11Texture2D* texture);
	void SetSRV(ID3D11ShaderResourceView* srv);
	void SetUAV(ID3D11UnorderedAccessView* uav);

public:
	bool						Release();
	
public:
	bool						CreateRenderTarget(float _width, float _height);
};

