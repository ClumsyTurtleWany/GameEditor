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

private:
	DXTextureManager() {};
	~DXTextureManager() { release(); };

public:
	void setDevice(ID3D11Device* _device, ID3D11DeviceContext* _context);
	bool Load(std::wstring _filename);
	bool Load(std::string _filename);
	bool Load(const char* _filename);

	DXTexture* getTexture(std::wstring _filename);
	DXTexture* getTexture(std::string _filename);
	DXTexture* getTexture(const char* _filename);
	DXTexture* CreateAlphaTexture(int width, int height);
	DXTexture* CreateTexture(DXTexture* texture);
	
public:
	bool initialize();
	bool frame();
	bool render();
	bool release();
};
