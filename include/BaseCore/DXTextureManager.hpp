#pragma once
#include "DXTexture.hpp"

class DXTextureManager : public Singleton<DXTextureManager>
{
private:
	friend class Singleton<DXTextureManager>;

	ID3D11Device* m_pd3dDevice = nullptr;
	ID3D11DeviceContext* m_pImmediateContext = nullptr;

	std::map<std::wstring, DXTexture*> m_TextureList;
	std::vector<DXTexture*>				m_AlphaTextureList;
	std::vector<ID3D11ShaderResourceView*>			SRVList;

private:
	DXTextureManager() {};
	~DXTextureManager() { Release(); };

public:
	void SetDevice(ID3D11Device* _device, ID3D11DeviceContext* _context);
	bool Load(std::wstring _filename);
	bool Load(std::string _filename);
	bool Load(const char* _filename);

	DXTexture* GetTexture(std::wstring _filename);
	DXTexture* GetTexture(std::string _filename);
	DXTexture* GetTexture(const char* _filename);

	DXTexture* CreateAlphaTexture(int width, int height);
	DXTexture* CreateTexture(DXTexture* texture);

	ID3D11ShaderResourceView* CreateShaderResourceViewBuffer(ID3D11Buffer* buffer);
	
public:
	bool Initialize();
	bool Frame();
	bool Render();
	bool Release();
};
