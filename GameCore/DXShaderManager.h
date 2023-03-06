#pragma once
#include "Define.h"

enum class InputLayoutType
{
	StaticMesh,
	SkeletalMesh
};

class DXShaderManager : public Singleton<DXShaderManager>
{
private:
	friend class Singleton<DXShaderManager>;

private:
	ID3D11Device*									m_pd3dDevice = nullptr;
	ID3D11DeviceContext*							m_pImmediateContext = nullptr;

	std::map<InputLayoutType, ID3D11InputLayout*>	InputLayoutMap;

	std::map<std::wstring, ID3DBlob*>				VertexShaderCodeMap;
	std::map<std::wstring, ID3DBlob*>				HullShaderCodeMap;
	std::map<std::wstring, ID3DBlob*>				DomainShaderCodeMap;
	std::map<std::wstring, ID3DBlob*>				GeometryShaderCodeMap;
	std::map<std::wstring, ID3DBlob*>				PixelShaderCodeMap;
	std::map<std::wstring, ID3DBlob*>				ComputeShaderCodeMap;

	std::map<std::wstring, ID3D11VertexShader*>		VertexShaderMap;
	std::map<std::wstring, ID3D11HullShader*>		HullShaderMap;
	std::map<std::wstring, ID3D11DomainShader*>		DomainShaderMap;
	std::map<std::wstring, ID3D11GeometryShader*>	GeometryShaderMap;
	std::map<std::wstring, ID3D11PixelShader*>		PixelShaderMap;
	std::map<std::wstring, ID3D11ComputeShader*>	ComputeShaderMap;

	std::vector<ID3D11Buffer*>						BufferList;
	std::vector<ID3D11ShaderResourceView*>			SRVList;

private:
	DXShaderManager() {};
	virtual ~DXShaderManager() { Release(); };

public:
	bool LoadVSCode(std::wstring filename);
	bool LoadHSCode(std::wstring filename);
	bool LoadDSCode(std::wstring filename);
	bool LoadGSCode(std::wstring filename);
	bool LoadPSCode(std::wstring filename);
	bool LoadCSCode(std::wstring filename);

	bool CreateVertexShader();
	bool CreatePixelShader();
	bool CreateGeometryShader(D3D11_SO_DECLARATION_ENTRY* decl = nullptr);
	bool CreateComputeShader();

	bool CreateInputLayout();
	bool CreateStaticMeshInputLayout();
	bool CreateSkeletalMeshInputLayout();

public:
	ID3D11Buffer* CreateVertexBuffer(const std::vector<Vertex>& vertices);
	ID3D11Buffer* CreateIndexBuffer(const std::vector<DWORD>& indices);

	template <typename T>
	ID3D11Buffer* CreateConstantBuffer(const T& constantData);

	template <typename T>
	ID3D11Buffer* CreateStructuredBuffer(const T& structData);

	ID3D11Buffer* CreateStreamOutputBuffer(UINT size);
	ID3D11Buffer* CreateMappedBuffer(UINT size);

	ID3D11ShaderResourceView* CreateShaderResourceViewBuffer(ID3D11Buffer* buffer);

public:
	void SetDevice(ID3D11Device* device, ID3D11DeviceContext* context);

public:
	ID3D11InputLayout* GetInputLayout(InputLayoutType type);

	ID3DBlob* GetVSCode(std::wstring key);
	ID3DBlob* GetHSCode(std::wstring key);
	ID3DBlob* GetDSCode(std::wstring key);
	ID3DBlob* GetGSCode(std::wstring key);
	ID3DBlob* GetPSCode(std::wstring key);
	ID3DBlob* GetCSCode(std::wstring key);

	ID3D11VertexShader*		GetVertexShader(std::wstring key);
	ID3D11HullShader*		GetHullShader(std::wstring key);
	ID3D11DomainShader*		GetDomainShader(std::wstring key);
	ID3D11GeometryShader*	GetGeometryShader(std::wstring key);
	ID3D11PixelShader*		GetPixelShader(std::wstring key);
	ID3D11ComputeShader*	GetComputeShader(std::wstring key);
	
public:
	bool Initialize();
	bool Release();

public:
	ID3D11DeviceContext* GetContext();
};

template<typename T>
inline ID3D11Buffer* DXShaderManager::CreateConstantBuffer(const T& constantData)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = sizeof(T) * 1; // 바이트 용량
	desc.Usage = D3D11_USAGE_DEFAULT; // 버퍼의 할당 장소 내지는 버퍼 용도, D3D11_USAGE_DEFAULT == GPU 메모리에 할당. D3D11_USAGE_DYNAMIC; //
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // Constant 버퍼를 사용하겠다는 플래그
	//desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // Test
	//desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initialData;
	ZeroMemory(&initialData, sizeof(initialData));
	initialData.pSysMem = &constantData; // 배열이 아니면 시스템 메모리에 들어 갈 수 없음. 그래서 그냥 배열보다 편한 vector 사용.

	//ID3D11Buffer* pConstantBuffer;
	//HRESULT result = m_pd3dDevice->CreateBuffer(
	//	&desc, // 버퍼 할당 
	//	&initialData, // 초기 할당된 버퍼를 체우는 CPU 메모리, NULL로 넣으면 생성만 해 놓는 것.
	//	&pConstantBuffer);

	ID3D11Buffer* pConstantBuffer;
	HRESULT result = m_pd3dDevice->CreateBuffer(
		&desc, // 버퍼 할당 
		&initialData, // 초기 할당된 버퍼를 체우는 CPU 메모리, NULL로 넣으면 생성만 해 놓는 것.
		&pConstantBuffer);

	if (FAILED(result))
	{
		return nullptr;
	}
	else
	{
		BufferList.push_back(pConstantBuffer);
		return pConstantBuffer;
	}
}

template<typename T>
inline ID3D11Buffer* DXShaderManager::CreateStructuredBuffer(const T& structData)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE; 
	desc.ByteWidth = sizeof(T) * 1;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = sizeof(T);

	D3D11_SUBRESOURCE_DATA initialData;
	ZeroMemory(&initialData, sizeof(initialData));
	initialData.pSysMem = &structData; 

	ID3D11Buffer* pStructuredBuffer;
	HRESULT result = m_pd3dDevice->CreateBuffer(
		&desc, // 버퍼 할당 
		&initialData, // 초기 할당된 버퍼를 체우는 CPU 메모리, NULL로 넣으면 생성만 해 놓는 것.
		&pStructuredBuffer);

	if (FAILED(result))
	{
		return nullptr;
	}
	else
	{
		BufferList.push_back(pStructuredBuffer);
		return pStructuredBuffer;
	}
}
