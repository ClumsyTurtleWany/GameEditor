#pragma once
#include "Define.h"

enum class VSCodeType
{
	StaticMesh,
	SkeletalMesh
};

class DXShaderManager : public Singleton<DXShaderManager>
{
private:
	friend class Singleton<DXShaderManager>;

private:
	ID3D11Device* m_pd3dDevice = nullptr;
	ID3D11DeviceContext* m_pImmediateContext = nullptr;

	std::map<unsigned int, DXShader*> m_ShaderList;

	std::map<VSCodeType, ID3DBlob*> m_pVertexShaderCodeMap;
	std::map<PSCodeType, ID3DBlob*> m_pPixelShaderCodeMap;

	std::map<VSCodeType, ID3D11VertexShader*> m_pVertexShaderMap;
	std::map<PSCodeType, ID3D11PixelShader*> m_pPixelShaderMap;

	unsigned int ShaderCnt = 0;

private:
	DXShaderManager() {};
	~DXShaderManager() { release(); };

private:
	bool LoadVSCode(VSCodeType _key, std::wstring _filename);
	bool LoadPSCode(PSCodeType _key, std::wstring _filename);

	bool CreateVertexShader();
	bool CreatePixelShader();

public:
	void setDevice(ID3D11Device* _device, ID3D11DeviceContext* _context);
	bool Load(int _key, ShaderType _type = ShaderType::Texture);
	DXShader* getShader(int _key);
	bool DeleteShader(int _key);
	ID3DBlob* getVSCode(VSCodeType _key);
	ID3DBlob* getPSCode(PSCodeType _key);
	ID3D11VertexShader* getVertexShader(VSCodeType _key);
	ID3D11PixelShader* getPixelShader(PSCodeType _key);
	unsigned int getShaderCount();

public:
	bool initialize();
	bool frame();
	bool render();
	bool release();
};
