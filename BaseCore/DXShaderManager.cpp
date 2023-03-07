#include "DXShaderManager.h"


void DXShaderManager::SetDevice(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_pd3dDevice = device;
	m_pImmediateContext = context;
}

bool DXShaderManager::LoadVSCode(std::wstring filename, std::wstring key)
{
	ID3DBlob* pVSCode = GetVSCode(key);
	if (pVSCode == nullptr)
	{
		// 정점 레이아웃은 정점 쉐이더와 밀접한 관련이 있다.
		// 정점 레이아웃 생성 시 사전에 정점 쉐이더 생성이 필요함. VertexShader.txt 참고.
		// 따라서 정점 쉐이더 컴파일이 먼저 필요함. 쉐이더 안에서는 유니코드가 작동 안함.
		// D3DCompileFromFile()
		// LPCWSTR pFileName : 파일 이름
		// D3D_SHADER_MACRO* pDefines
		// ID3DInclude* pInclude
		// LPCSTR pEntrypoint : 사용할 함수(VertexShader.txt에 있는 함수) 이름
		// LPCSTR pTarget : 컴파일러 버전(쉐이더 버전 별로 다름)
		// UINT Flags1
		// UINT Flags2
		// ID3DBlob** ppCode : 결과(.obj 파일의 시작 주소와, 크기를 반환 함.)
		// _Always_(_Outptr_opt_result_maybenull_ : Error Code
		HRESULT result;
		ID3DBlob* pErrorCode = nullptr;
		result = D3DCompileFromFile(
			filename.c_str(), //"../../resource/shader/ShapeShader.txt",  //"VertexShader.txt",
			NULL, NULL,
			"VS",
			"vs_5_0",
			0, 0,
			&pVSCode,
			&pErrorCode);

		if (FAILED(result))
		{
			if (pErrorCode != nullptr) // 쉐이더 파일에서는 디버깅이 불가능 해서 Error Code 받아와서 처리하는게 좋음.
			{
				OutputDebugStringA((char*)pErrorCode->GetBufferPointer());
				pErrorCode->Release();
			}
			return false;
		}
		else
		{
			VertexShaderCodeMap.insert(std::make_pair(key, pVSCode));
			return true;
		}
	}
	else
	{
		return true;
	}
}

bool DXShaderManager::LoadHSCode(std::wstring filename, std::wstring key)
{
	ID3DBlob* pHSCode = GetHSCode(key);
	if (pHSCode == nullptr)
	{
		// 정점 레이아웃은 정점 쉐이더와 밀접한 관련이 있다.
		// 정점 레이아웃 생성 시 사전에 정점 쉐이더 생성이 필요함. VertexShader.txt 참고.
		// 따라서 정점 쉐이더 컴파일이 먼저 필요함. 쉐이더 안에서는 유니코드가 작동 안함.
		// D3DCompileFromFile()
		// LPCWSTR pFileName : 파일 이름
		// D3D_SHADER_MACRO* pDefines
		// ID3DInclude* pInclude
		// LPCSTR pEntrypoint : 사용할 함수(VertexShader.txt에 있는 함수) 이름
		// LPCSTR pTarget : 컴파일러 버전(쉐이더 버전 별로 다름)
		// UINT Flags1
		// UINT Flags2
		// ID3DBlob** ppCode : 결과(.obj 파일의 시작 주소와, 크기를 반환 함.)
		// _Always_(_Outptr_opt_result_maybenull_ : Error Code
		HRESULT result;
		ID3DBlob* pErrorCode = nullptr;
		result = D3DCompileFromFile(
			filename.c_str(), //"../../resource/shader/ShapeShader.txt",  //"VertexShader.txt",
			NULL, NULL,
			"HS",
			"hs_5_0",
			0, 0,
			&pHSCode,
			&pErrorCode);

		if (FAILED(result))
		{
			if (pErrorCode != nullptr) // 쉐이더 파일에서는 디버깅이 불가능 해서 Error Code 받아와서 처리하는게 좋음.
			{
				OutputDebugStringA((char*)pErrorCode->GetBufferPointer());
				pErrorCode->Release();
			}
			return false;
		}
		else
		{
			HullShaderCodeMap.insert(std::make_pair(key, pHSCode));
			return true;
		}
	}
	else
	{
		return true;
	}
}

bool DXShaderManager::LoadDSCode(std::wstring filename, std::wstring key)
{
	ID3DBlob* pDSCode = GetDSCode(key);
	if (pDSCode == nullptr)
	{
		// 정점 레이아웃은 정점 쉐이더와 밀접한 관련이 있다.
		// 정점 레이아웃 생성 시 사전에 정점 쉐이더 생성이 필요함. VertexShader.txt 참고.
		// 따라서 정점 쉐이더 컴파일이 먼저 필요함. 쉐이더 안에서는 유니코드가 작동 안함.
		// D3DCompileFromFile()
		// LPCWSTR pFileName : 파일 이름
		// D3D_SHADER_MACRO* pDefines
		// ID3DInclude* pInclude
		// LPCSTR pEntrypoint : 사용할 함수(VertexShader.txt에 있는 함수) 이름
		// LPCSTR pTarget : 컴파일러 버전(쉐이더 버전 별로 다름)
		// UINT Flags1
		// UINT Flags2
		// ID3DBlob** ppCode : 결과(.obj 파일의 시작 주소와, 크기를 반환 함.)
		// _Always_(_Outptr_opt_result_maybenull_ : Error Code
		HRESULT result;
		ID3DBlob* pErrorCode = nullptr;
		result = D3DCompileFromFile(
			filename.c_str(), //"../../resource/shader/ShapeShader.txt",  //"VertexShader.txt",
			NULL, NULL,
			"DS",
			"ds_5_0",
			0, 0,
			&pDSCode,
			&pErrorCode);

		if (FAILED(result))
		{
			if (pErrorCode != nullptr) // 쉐이더 파일에서는 디버깅이 불가능 해서 Error Code 받아와서 처리하는게 좋음.
			{
				OutputDebugStringA((char*)pErrorCode->GetBufferPointer());
				pErrorCode->Release();
			}
			return false;
		}
		else
		{
			DomainShaderCodeMap.insert(std::make_pair(key, pDSCode));
			return true;
		}
	}
	else
	{
		return true;
	}
}

bool DXShaderManager::LoadGSCode(std::wstring filename, std::wstring key)
{
	ID3DBlob* pGSCode = GetGSCode(key);
	if (pGSCode == nullptr)
	{
		// 정점 레이아웃은 정점 쉐이더와 밀접한 관련이 있다.
		// 정점 레이아웃 생성 시 사전에 정점 쉐이더 생성이 필요함. VertexShader.txt 참고.
		// 따라서 정점 쉐이더 컴파일이 먼저 필요함. 쉐이더 안에서는 유니코드가 작동 안함.
		// D3DCompileFromFile()
		// LPCWSTR pFileName : 파일 이름
		// D3D_SHADER_MACRO* pDefines
		// ID3DInclude* pInclude
		// LPCSTR pEntrypoint : 사용할 함수(VertexShader.txt에 있는 함수) 이름
		// LPCSTR pTarget : 컴파일러 버전(쉐이더 버전 별로 다름)
		// UINT Flags1
		// UINT Flags2
		// ID3DBlob** ppCode : 결과(.obj 파일의 시작 주소와, 크기를 반환 함.)
		// _Always_(_Outptr_opt_result_maybenull_ : Error Code
		HRESULT result;
		ID3DBlob* pErrorCode = nullptr;
		result = D3DCompileFromFile(
			filename.c_str(), //"../../resource/shader/ShapeShader.txt",  //"VertexShader.txt",
			NULL, NULL,
			"GS",
			"gs_5_0",
			0, 0,
			&pGSCode,
			&pErrorCode);

		if (FAILED(result))
		{
			if (pErrorCode != nullptr) // 쉐이더 파일에서는 디버깅이 불가능 해서 Error Code 받아와서 처리하는게 좋음.
			{
				OutputDebugStringA((char*)pErrorCode->GetBufferPointer());
				pErrorCode->Release();
			}
			return false;
		}
		else
		{
			DomainShaderCodeMap.insert(std::make_pair(key, pGSCode));
			return true;
		}
	}
	else
	{
		return true;
	}
}

bool DXShaderManager::LoadPSCode(std::wstring filename, std::wstring key)
{
	ID3DBlob* pPSCode = GetPSCode(key);
	if (pPSCode == nullptr)
	{
		// Pixel Shader Create
		HRESULT result;
		ID3DBlob* pErrorCode = nullptr; // 
		result = D3DCompileFromFile(
			filename.c_str(), //"../../resource/shader/ShapeShader.txt", //"PixelShader.txt",
			NULL, NULL,
			"PS",
			"ps_5_0",
			0, 0,
			&pPSCode,
			&pErrorCode);

		if (FAILED(result))
		{
			if (pErrorCode != nullptr) // 쉐이더 파일에서는 디버깅이 불가능 해서 Error Code 받아와서 처리하는게 좋음.
			{
				OutputDebugStringA((char*)pErrorCode->GetBufferPointer());
				pErrorCode->Release();
			}
			return false;
		}
		else
		{
			PixelShaderCodeMap.insert(std::make_pair(key, pPSCode));
			return true;
		}
	}
	else
	{
		return true;
	}
}

bool DXShaderManager::LoadCSCode(std::wstring filename, std::wstring key)
{
	ID3DBlob* pCSCode = GetPSCode(key);
	if (pCSCode == nullptr)
	{
		HRESULT result;
		ID3DBlob* pErrorCode = nullptr; // 
		result = D3DCompileFromFile(
			filename.c_str(), //"../../resource/shader/ShapeShader.txt", //"PixelShader.txt",
			NULL, NULL,
			"CS",
			"cs_5_0",
			0, 0,
			&pCSCode,
			&pErrorCode);

		if (FAILED(result))
		{
			if (pErrorCode != nullptr) // 쉐이더 파일에서는 디버깅이 불가능 해서 Error Code 받아와서 처리하는게 좋음.
			{
				OutputDebugStringA((char*)pErrorCode->GetBufferPointer());
				pErrorCode->Release();
			}
			return false;
		}
		else
		{
			ComputeShaderCodeMap.insert(std::make_pair(key, pCSCode));
			return true;
		}
	}
	else
	{
		return true;
	}
}

bool DXShaderManager::CreateVertexShader(ID3DBlob* vsCode, std::wstring key)
{
	auto vs = GetVertexShader(key);
	if (vs != nullptr)
	{
		return true;
	}
	else
	{
		if (vsCode == nullptr)
		{
			return false;
		}
		
		ID3D11VertexShader* pVertexShader;
		HRESULT result = m_pd3dDevice->CreateVertexShader(vsCode->GetBufferPointer(), vsCode->GetBufferSize(), NULL, &pVertexShader);
		if (FAILED(result))
		{
			OutputDebugStringA("DXShaderManager::CreateVertexShader::Failed Create Vertex Shader.\n");
			return false;
		}
		else
		{
			VertexShaderMap.insert(std::make_pair(key, pVertexShader));
		}
		return true;
	}
}

bool DXShaderManager::CreatePixelShader(ID3DBlob* psCode, std::wstring key)
{
	auto ps = GetPixelShader(key);
	if (ps != nullptr)
	{
		return true;
	}
	else
	{
		if (psCode == nullptr)
		{
			return false;
		}

		ID3D11PixelShader* pPixelShader;
		HRESULT result = m_pd3dDevice->CreatePixelShader(psCode->GetBufferPointer(), psCode->GetBufferSize(), NULL, &pPixelShader);

		if (FAILED(result))
		{
			OutputDebugStringA("DXShaderManager::CreatePixelShader::Failed Create Pixel Shader.\n");
			return false;
		}
		else
		{
			PixelShaderMap.insert(std::make_pair(key, pPixelShader));
		}
		return true;		
	}
}

bool DXShaderManager::CreateHullShader(ID3DBlob* hsCode, std::wstring key)
{
	return true;
}

bool DXShaderManager::CreateDomainShader(ID3DBlob* dsCode, std::wstring key)
{
	return true;
}

bool DXShaderManager::CreateGeometryShader(ID3DBlob* gsCode, D3D11_SO_DECLARATION_ENTRY* decl, UINT elementsNum, std::wstring key)
{
	auto gs = GetGeometryShader(key);
	if (gs != nullptr)
	{
		return true;
	}
	else
	{
		if (gsCode == nullptr || decl == nullptr)
		{
			return false;
		}

		ID3D11GeometryShader* pGeometryShader;
		UINT entrySize = sizeof(decl);
		HRESULT result = m_pd3dDevice->CreateGeometryShaderWithStreamOutput(gsCode->GetBufferPointer(), gsCode->GetBufferSize(), decl, elementsNum, NULL, 0, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &pGeometryShader);

		if (FAILED(result))
		{
			OutputDebugStringA("DXShaderManager::CreateGeometryShader::Failed Create Geometry Shader.\n");
			return false;
		}
		else
		{
			GeometryShaderMap.insert(std::make_pair(key, pGeometryShader));
		}
		return true;		
	}
}

bool DXShaderManager::CreateComputeShader(ID3DBlob* csCode, std::wstring key)
{
	auto cs = GetComputeShader(key);
	if (cs != nullptr)
	{
		return true;
	}
	else
	{
		if (csCode == nullptr)
		{
			return false;
		}

		ID3D11ComputeShader* pComputeShader;
		HRESULT result = m_pd3dDevice->CreateComputeShader(csCode->GetBufferPointer(), csCode->GetBufferSize(), NULL, &pComputeShader);

		if (FAILED(result))
		{
			OutputDebugStringA("DXShaderManager::CreateComputeShader::Failed Create Compute Shader.\n");
			return false;
		}
		else
		{
			ComputeShaderMap.insert(std::make_pair(key, pComputeShader));
		}
		return true;
	}
}

bool DXShaderManager::CreateVertexShader(std::wstring filename, std::wstring key)
{
	auto vsCode = VertexShaderCodeMap.find(key);
	if (vsCode != VertexShaderCodeMap.end())
	{
		auto vs = VertexShaderMap.find(key);
		if (vs != VertexShaderMap.end())
		{
			return true;
		}
		else
		{
			if (!CreateVertexShader(vsCode->second, key))
			{
				OutputDebugStringA("DXShaderManager::CreateVertexShader::Failed Create Vertex Shader.");
				return false;
			}
		}
	}
	else
	{
		if (!LoadVSCode(filename, key))
		{
			OutputDebugStringA("DXShaderManager::CreateVertexShader::Failed Load Vertex Shader Code.");
			return false;
		}

		ID3DBlob* blob = GetVSCode(key);
		if (!CreateVertexShader(blob, key))
		{
			OutputDebugStringA("DXShaderManager::CreateVertexShader::Failed Create Vertex Shader.");
			return false;
		}
	}
	return true;
}

bool DXShaderManager::CreatePixelShader(std::wstring filename, std::wstring key)
{
	auto psCode = PixelShaderCodeMap.find(key);
	if (psCode != PixelShaderCodeMap.end())
	{
		auto ps = PixelShaderMap.find(key);
		if (ps != PixelShaderMap.end())
		{
			return true;
		}
		else
		{
			if (!CreatePixelShader(psCode->second, key))
			{
				OutputDebugStringA("DXShaderManager::CreatePixelShader::Failed Create Pixel Shader.");
				return false;
			}
		}
	}
	else
	{
		if (!LoadPSCode(filename, key))
		{
			OutputDebugStringA("DXShaderManager::CreatePixelShader::Failed Load Pixel Shader Code.");
			return false;
		}

		ID3DBlob* blob = GetPSCode(key);
		if (!CreatePixelShader(blob, key))
		{
			OutputDebugStringA("DXShaderManager::CreatePixelShader::Failed Create Pixel Shader.");
			return false;
		}
	}
	return true;
}

bool DXShaderManager::CreateHullShader(std::wstring filename, std::wstring key)
{
	return true;
}

bool DXShaderManager::CreateDomainShader(std::wstring filename, std::wstring key)
{
	return true;
}

bool DXShaderManager::CreateGeometryShader(std::wstring filename, D3D11_SO_DECLARATION_ENTRY* decl, UINT elementsNum, std::wstring key)
{
	return true;
}

bool DXShaderManager::CreateComputeShader(std::wstring filename, std::wstring key)
{
	auto csCode = ComputeShaderCodeMap.find(key);
	if (csCode != ComputeShaderCodeMap.end())
	{
		auto cs = ComputeShaderMap.find(key);
		if (cs != ComputeShaderMap.end())
		{
			return true;
		}
		else
		{
			if (!CreateComputeShader(csCode->second, key))
			{
				OutputDebugStringA("DXShaderManager::CreateComputeShader::Failed Create Compute Shader.");
				return false;
			}
		}
	}
	else
	{
		if (!LoadCSCode(filename, key))
		{
			OutputDebugStringA("DXShaderManager::CreateComputeShader::Failed Load Compute Shader Code.");
			return false;
		}

		ID3DBlob* blob = GetCSCode(key);
		if (!CreateComputeShader(blob, key))
		{
			OutputDebugStringA("DXShaderManager::CreateComputeShader::Failed Create Compute Shader.");
			return false;
		}
	}
	return true;
}

bool DXShaderManager::CreateInputLayout(ID3DBlob* vsCode, D3D11_INPUT_ELEMENT_DESC* desc, UINT elementsNum, std::wstring key)
{
	if (vsCode == nullptr || desc == nullptr)
	{
		return false;
	}

	ID3D11InputLayout* pInputLayout = nullptr;
	UINT NumElements = sizeof(desc) / sizeof(desc[0]);
	HRESULT result = m_pd3dDevice->CreateInputLayout(desc, elementsNum, vsCode->GetBufferPointer(), vsCode->GetBufferSize(), &pInputLayout);
	if (FAILED(result))
	{
		OutputDebugStringA("DXShaderManager::CreateInputLayout::Failed Create Input Layout.\n");
		return false;
	}
	else
	{
		InputLayoutMap.insert(std::make_pair(key, pInputLayout));
	}

	return true;
}

ID3D11Buffer* DXShaderManager::CreateIndexBuffer(const std::vector<DWORD>& indices)
{
	if (indices.empty())
	{
		return nullptr;
	}

	// CreateBuffer() Param
	// D3D11_BUFFER_DESC* pDesc,
	// D3D11_SUBRESOURCE_DATA* pInitialData,
	// ID3D11Buffer** ppBuffer
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = sizeof(DWORD) * static_cast<UINT>(indices.size()); // 바이트 용량
	desc.Usage = D3D11_USAGE_DEFAULT; // 버퍼의 할당 장소 내지는 버퍼 용도, D3D11_USAGE_DEFAULT == GPU 메모리에 할당.
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER; // 인덱스 버퍼를 사용하겠다는 플래그
	// desc.CPUAccessFlags = 0; // CPU에서 접근 하게 할 것이냐, 0으로 하면 처음 할당 이외에는 CPU가 읽고 쓰기 불가.
	// desc.MiscFlags = 0; //
	// desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initialData;
	ZeroMemory(&initialData, sizeof(initialData));
	initialData.pSysMem = &indices.at(0); // 배열이 아니면 시스템 메모리에 들어 갈 수 없음. 그래서 그냥 배열보다 편한 vector 사용.

	//ID3D11Buffer* pIndexBuffer;
	//HRESULT result = m_pd3dDevice->CreateBuffer(
	//	&desc, // 버퍼 할당 
	//	&initialData, // 초기 할당된 버퍼를 체우는 CPU 메모리, NULL로 넣으면 생성만 해 놓는 것.
	//	&pIndexBuffer); // desc cpu flag를 0으로 해서 이 버퍼에 CPU는 접근 할 수 없음.

	ID3D11Buffer* pIndexBuffer;
	HRESULT result = m_pd3dDevice->CreateBuffer(
		&desc, // 버퍼 할당 
		&initialData, // 초기 할당된 버퍼를 체우는 CPU 메모리, NULL로 넣으면 생성만 해 놓는 것.
		&pIndexBuffer); // desc cpu flag를 0으로 해서 이 버퍼에 CPU는 접근 할 수 없음.

	if (FAILED(result))
	{
		return nullptr;
	}
	else
	{
		BufferList.push_back(pIndexBuffer);
		return pIndexBuffer;
	}
}

ID3D11Buffer* DXShaderManager::CreateStreamOutputBuffer(UINT size)
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = size;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_STREAM_OUTPUT;
	ZeroMemory(&desc, sizeof(desc));

	ID3D11Buffer* pStreamOutputBuffer;
	HRESULT result = m_pd3dDevice->CreateBuffer(&desc, NULL, &pStreamOutputBuffer);
	if (FAILED(result))
	{
		return nullptr;
	}
	else
	{
		BufferList.push_back(pStreamOutputBuffer);
		return pStreamOutputBuffer;
	}

	return nullptr;
}

ID3D11Buffer* DXShaderManager::CreateMappedBuffer(UINT size)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = size;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_STREAM_OUTPUT;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	
	
	return nullptr;
}

ID3D11InputLayout* DXShaderManager::GetInputLayout(std::wstring key)
{
	auto it = InputLayoutMap.find(key);
	if (it != InputLayoutMap.end())
	{
		return it->second;
	}

	return nullptr;
}

ID3DBlob* DXShaderManager::GetVSCode(std::wstring key)
{
	auto it = VertexShaderCodeMap.find(key);
	if (it != VertexShaderCodeMap.end())
	{
		return it->second;
	}
	else
	{
		OutputDebugStringA("DXShaderManager::GetVSCode::Failed Get Vertex Shader Code.\n");
		return nullptr;
	}
}

ID3DBlob* DXShaderManager::GetHSCode(std::wstring key)
{
	auto it = HullShaderCodeMap.find(key);
	if (it != HullShaderCodeMap.end())
	{
		return it->second;
	}
	else
	{
		OutputDebugStringA("DXShaderManager::GetHSCode::Failed Get Hull Shader Code.\n");
		return nullptr;
	}
}

ID3DBlob* DXShaderManager::GetDSCode(std::wstring key)
{
	auto it = DomainShaderCodeMap.find(key);
	if (it != DomainShaderCodeMap.end())
	{
		return it->second;
	}
	else
	{
		OutputDebugStringA("DXShaderManager::GetDSCode::Failed Get Domain Shader Code.\n");
		return nullptr;
	}
}

ID3DBlob* DXShaderManager::GetGSCode(std::wstring key)
{
	auto it = GeometryShaderCodeMap.find(key);
	if (it != GeometryShaderCodeMap.end())
	{
		return it->second;
	}
	else
	{
		OutputDebugStringA("DXShaderManager::GetGSCode::Failed Get Geometry Shader Code.\n");
		return nullptr;
	}
}

ID3DBlob* DXShaderManager::GetPSCode(std::wstring key)
{
	auto it = PixelShaderCodeMap.find(key);
	if (it != PixelShaderCodeMap.end())
	{
		return it->second;
	}
	else
	{
		OutputDebugStringA("DXShaderManager::GetPSCode::Failed Get Pixel Shader Code.\n");
		return nullptr;
	}
}

ID3DBlob* DXShaderManager::GetCSCode(std::wstring key)
{
	auto it = ComputeShaderCodeMap.find(key);
	if (it != ComputeShaderCodeMap.end())
	{
		return it->second;
	}
	else
	{
		OutputDebugStringA("DXShaderManager::GetCSCode::Failed Get Compute Shader Code.\n");
		return nullptr;
	}
}

ID3D11VertexShader* DXShaderManager::GetVertexShader(std::wstring key)
{
	auto it = VertexShaderMap.find(key);
	if (it != VertexShaderMap.end())
	{
		return it->second;
	}
	else
	{
		OutputDebugStringA("DXShaderManager::GetVertexShader::Failed Get Vertex Shader.\n");
		return nullptr;
	}
}

ID3D11HullShader* DXShaderManager::GetHullShader(std::wstring key)
{
	auto it = HullShaderMap.find(key);
	if (it != HullShaderMap.end())
	{
		return it->second;
	}
	else
	{
		OutputDebugStringA("DXShaderManager::GetHullShader::Failed Get Hull Shader.\n");
		return nullptr;
	}
}

ID3D11DomainShader* DXShaderManager::GetDomainShader(std::wstring key)
{
	auto it = DomainShaderMap.find(key);
	if (it != DomainShaderMap.end())
	{
		return it->second;
	}
	else
	{
		OutputDebugStringA("DXShaderManager::GetDomainShader::Failed Get Domain Shader.\n");
		return nullptr;
	}
}

ID3D11GeometryShader* DXShaderManager::GetGeometryShader(std::wstring key)
{
	auto it = GeometryShaderMap.find(key);
	if (it != GeometryShaderMap.end())
	{
		return it->second;
	}
	else
	{
		OutputDebugStringA("DXShaderManager::GetGeometryShader::Failed Get Geometry Shader.\n");
		return nullptr;
	}
}

ID3D11PixelShader* DXShaderManager::GetPixelShader(std::wstring key)
{
	auto it = PixelShaderMap.find(key);
	if (it != PixelShaderMap.end())
	{
		return it->second;
	}
	else
	{
		OutputDebugStringA("DXShaderManager::GetPixelShader::Failed Get Pixel Shader.\n");
		return nullptr;
	}
}


ID3D11ComputeShader* DXShaderManager::GetComputeShader(std::wstring key)
{
	auto it = ComputeShaderMap.find(key);
	if (it != ComputeShaderMap.end())
	{
		return it->second;
	}
	else
	{
		OutputDebugStringA("DXShaderManager::GetComputeShader::Failed Get Compute Shader.\n");
		return nullptr;
	}
}

bool DXShaderManager::Initialize()
{
	return true;
}

bool DXShaderManager::Release()
{
	for (auto& it : VertexShaderCodeMap)
	{
		ID3DBlob* pVSCode = it.second;
		pVSCode->Release();
		pVSCode = nullptr;
	}
	VertexShaderCodeMap.clear();

	for (auto& it : PixelShaderCodeMap)
	{
		ID3DBlob* pPSCode = it.second;
		pPSCode->Release();
		pPSCode = nullptr;
	}
	PixelShaderCodeMap.clear();

	for (auto& it : HullShaderCodeMap)
	{
		ID3DBlob* pHSCode = it.second;
		pHSCode->Release();
		pHSCode = nullptr;
	}
	HullShaderCodeMap.clear();

	for (auto& it : DomainShaderCodeMap)
	{
		ID3DBlob* pDSCode = it.second;
		pDSCode->Release();
		pDSCode = nullptr;
	}
	DomainShaderCodeMap.clear();

	for (auto& it : GeometryShaderCodeMap)
	{
		ID3DBlob* pGSCode = it.second;
		pGSCode->Release();
		pGSCode = nullptr;
	}
	GeometryShaderCodeMap.clear();

	for (auto& it : ComputeShaderCodeMap)
	{
		ID3DBlob* pCSCode = it.second;
		pCSCode->Release();
		pCSCode = nullptr;
	}
	ComputeShaderCodeMap.clear();

	for (auto& it : VertexShaderMap)
	{
		ID3D11VertexShader* pVS = it.second;
		pVS->Release();
		pVS = nullptr;
	}
	VertexShaderMap.clear();

	for (auto& it : PixelShaderMap)
	{
		ID3D11PixelShader* pPS = it.second;
		pPS->Release();
		pPS = nullptr;
	}
	PixelShaderMap.clear();

	for (auto& it : HullShaderMap)
	{
		ID3D11HullShader* pHS = it.second;
		pHS->Release();
		pHS = nullptr;
	}
	HullShaderMap.clear();

	for (auto& it : DomainShaderMap)
	{
		ID3D11DomainShader* pDS = it.second;
		pDS->Release();
		pDS = nullptr;
	}
	DomainShaderMap.clear();

	for (auto& it : GeometryShaderMap)
	{
		ID3D11GeometryShader* pGS = it.second;
		pGS->Release();
		pGS = nullptr;
	}
	GeometryShaderMap.clear();

	for (auto& it : ComputeShaderMap)
	{
		ID3D11ComputeShader* pCS = it.second;
		pCS->Release();
		pCS = nullptr;
	}
	ComputeShaderMap.clear();

	for (auto& it : BufferList)
	{
		it->Release();
	}
	BufferList.clear();

	for (auto& it : InputLayoutMap)
	{
		ID3D11InputLayout* pLayout = it.second;
		if (pLayout == nullptr)
		{
			continue;
		}
		pLayout->Release();
		pLayout = nullptr;
	}
	InputLayoutMap.clear();

	return true;
}