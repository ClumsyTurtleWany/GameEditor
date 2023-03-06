#include "DXShaderManager.h"


void DXShaderManager::SetDevice(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_pd3dDevice = device;
	m_pImmediateContext = context;
}

bool DXShaderManager::LoadVSCode(std::wstring filename)
{
	ID3DBlob* pVSCode = GetVSCode(filename);
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
			VertexShaderCodeMap.insert(std::make_pair(filename, pVSCode));
			return true;
		}
	}
	else
	{
		return true;
	}
}

bool DXShaderManager::LoadHSCode(std::wstring filename)
{
	ID3DBlob* pHSCode = GetHSCode(filename);
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
			HullShaderCodeMap.insert(std::make_pair(filename, pHSCode));
			return true;
		}
	}
	else
	{
		return true;
	}
}

bool DXShaderManager::LoadDSCode(std::wstring filename)
{
	ID3DBlob* pDSCode = GetDSCode(filename);
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
			DomainShaderCodeMap.insert(std::make_pair(filename, pDSCode));
			return true;
		}
	}
	else
	{
		return true;
	}
}

bool DXShaderManager::LoadGSCode(std::wstring filename)
{
	ID3DBlob* pGSCode = GetGSCode(filename);
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
			DomainShaderCodeMap.insert(std::make_pair(filename, pGSCode));
			return true;
		}
	}
	else
	{
		return true;
	}
}

bool DXShaderManager::LoadPSCode(std::wstring filename)
{
	ID3DBlob* pPSCode = GetPSCode(filename);
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
			PixelShaderCodeMap.insert(std::make_pair(filename, pPSCode));
			return true;
		}
	}
	else
	{
		return true;
	}
}

bool DXShaderManager::LoadCSCode(std::wstring filename)
{
	ID3DBlob* pCSCode = GetCSCode(filename);
	if (pCSCode == nullptr)
	{
		// Pixel Shader Create
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
			ComputeShaderCodeMap.insert(std::make_pair(filename, pCSCode));
			return true;
		}
	}
	else
	{
		return true;
	}
}

bool DXShaderManager::CreateVertexShader()
{
	for (auto it : VertexShaderCodeMap)
	{
		ID3D11VertexShader* pVertexShader;
		HRESULT result = m_pd3dDevice->CreateVertexShader(it.second->GetBufferPointer(), it.second->GetBufferSize(), NULL, &pVertexShader);

		if (FAILED(result))
		{
			OutputDebugStringA("DXShaderManager::CreateVertexShader::Failed Create Vertex Shader.\n");
			return false;
		}
		else
		{
			VertexShaderMap.insert(std::make_pair(it.first, pVertexShader));
		}
	}
	return true;
}

bool DXShaderManager::CreatePixelShader()
{
	for (auto it : PixelShaderCodeMap)
	{
		ID3D11PixelShader* pPixelShader;
		HRESULT result = m_pd3dDevice->CreatePixelShader(it.second->GetBufferPointer(), it.second->GetBufferSize(), NULL, &pPixelShader);

		if (FAILED(result))
		{
			OutputDebugStringA("DXShaderManager::CreatePixelShader::Failed Create Pixel Shader.\n");
			return false;
		}
		else
		{
			PixelShaderMap.insert(std::make_pair(it.first, pPixelShader));
		}
	}
	return true;
}

bool DXShaderManager::CreateGeometryShader(D3D11_SO_DECLARATION_ENTRY* decl)
{
	for (auto it : GeometryShaderCodeMap)
	{
		ID3D11GeometryShader* pGeometryShader;
		//HRESULT result = m_pd3dDevice->CreateGeometryShader(it.second->GetBufferPointer(), it.second->GetBufferSize(), NULL, &pGeometryShader);
		
		D3D11_SO_DECLARATION_ENTRY pDecl[] =
		{
			{0, "SV_POSITION", 0, 0, 4, 0},
			{0, "POSITION", 0, 0, 3, 0},
			{0, "NORMAL", 0, 0, 3, 0},
			{0, "COLOR", 0, 0, 4, 0},
			{0, "TEXCOORD", 0, 0, 2, 0},
			{0, "TEXCOORD", 1, 0, 4, 0},
		};

		UINT entrySize = sizeof(decl);
		HRESULT result = m_pd3dDevice->CreateGeometryShaderWithStreamOutput(it.second->GetBufferPointer(), it.second->GetBufferSize(), pDecl, sizeof(pDecl), NULL, 0, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &pGeometryShader);

		if (FAILED(result))
		{
			OutputDebugStringA("DXShaderManager::CreateGeometryShader::Failed Create Geometry Shader.\n");
			return false;
		}
		else
		{
			GeometryShaderMap.insert(std::make_pair(it.first, pGeometryShader));
		}
	}
}

bool DXShaderManager::CreateComputeShader()
{
	for (auto it : ComputeShaderCodeMap)
	{
		ID3D11ComputeShader* pComputeShader;
		HRESULT result = m_pd3dDevice->CreateComputeShader(it.second->GetBufferPointer(), it.second->GetBufferSize(), NULL, &pComputeShader);

		if (FAILED(result))
		{
			OutputDebugStringA("DXShaderManager::CreateComputeShader::Failed Create Compute Shader.\n");
			return false;
		}
		else
		{
			ComputeShaderMap.insert(std::make_pair(it.first, pComputeShader));
		}
	}
	return true;
}

bool DXShaderManager::CreateStaticMeshInputLayout()
{
	// Input Layout For Static Mesh
	ID3DBlob* pBlob = GetVSCode(L"../include/Core/HLSL/VS_StaticMesh.hlsl");
	if (pBlob == nullptr)
	{
		return false;
	}

	//ComPtr<ID3D11InputLayout> StaticMeshLayout;
	ID3D11InputLayout* StaticMeshLayout = nullptr;
	D3D11_INPUT_ELEMENT_DESC InputElementDescs[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 12 == float * 3 // Vertex의 Color 시작 바이트.
		{"TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0} // 28 == float * 28 // Vertex의 Texture 시작 바이트.
	};

	UINT NumElements = sizeof(InputElementDescs) / sizeof(InputElementDescs[0]);
	HRESULT result = m_pd3dDevice->CreateInputLayout(InputElementDescs, NumElements, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &StaticMeshLayout);
	if (FAILED(result))
	{
		return false;
	}

	InputLayoutMap.insert(std::make_pair(InputLayoutType::StaticMesh, StaticMeshLayout));

	return true;
}

bool DXShaderManager::CreateSkeletalMeshInputLayout()
{
	// Input Layout For Skeletal Mesh
	ID3DBlob* pBlob = GetVSCode(L"../include/Core/HLSL/VS_SkeletalMesh.hlsl");
	if (pBlob == nullptr)
	{
		return false;
	}

	ID3D11InputLayout* SkeletalMeshLayout;
	D3D11_INPUT_ELEMENT_DESC InputElementDescs[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 12 == float * 3 // Vertex의 Color 시작 바이트.
		{"TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 28 == float * 28 // Vertex의 Texture 시작 바이트.

		{"INDEX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	UINT NumElements = sizeof(InputElementDescs) / sizeof(InputElementDescs[0]);
	HRESULT result = m_pd3dDevice->CreateInputLayout(InputElementDescs, NumElements, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &SkeletalMeshLayout);

	if (FAILED(result))
	{
		return result;
	}

	InputLayoutMap.insert(std::make_pair(InputLayoutType::SkeletalMesh, SkeletalMeshLayout));

	return true;
}

bool DXShaderManager::CreateInputLayout()
{
	if (!CreateStaticMeshInputLayout())
	{
		return false;
	}
	
	if (!CreateSkeletalMeshInputLayout())
	{
		return false;
	}
	
	return true;	
}

ID3D11Buffer* DXShaderManager::CreateVertexBuffer(const std::vector<Vertex>& vertices)
{
	// CreateBuffer() Param
	// D3D11_BUFFER_DESC* pDesc,
	// D3D11_SUBRESOURCE_DATA* pInitialData,
	// ID3D11Buffer** ppBuffer
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = sizeof(Vertex) * static_cast<UINT>(vertices.size()); // 바이트 용량
	desc.Usage = D3D11_USAGE_DEFAULT; // 버퍼의 할당 장소 내지는 버퍼 용도, D3D11_USAGE_DEFAULT == GPU 메모리에 할당.
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	// desc.CPUAccessFlags = 0; // CPU에서 접근 하게 할 것이냐, 0으로 하면 처음 할당 이외에는 CPU가 읽고 쓰기 불가.
	// desc.MiscFlags = 0; //
	// desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initialData;
	ZeroMemory(&initialData, sizeof(initialData));
	initialData.pSysMem = &vertices.at(0); // 배열이 아니면 시스템 메모리에 들어 갈 수 없음. 그래서 그냥 배열보다 편한 vector 사용.

	//ID3D11Buffer* pVertexBuffer;
	//HRESULT result = m_pd3dDevice->CreateBuffer(
	//	&desc, // 버퍼 할당 
	//	&initialData, // 초기 할당된 버퍼를 체우는 CPU 메모리, NULL로 넣으면 생성만 해 놓는 것.
	//	&pVertexBuffer); // desc cpu flag를 0으로 해서 이 버퍼에 CPU는 접근 할 수 없음.

	ID3D11Buffer* pVertexBuffer;
	HRESULT result = m_pd3dDevice->CreateBuffer(
		&desc, // 버퍼 할당 
		&initialData, // 초기 할당된 버퍼를 체우는 CPU 메모리, NULL로 넣으면 생성만 해 놓는 것.
		&pVertexBuffer); // desc cpu flag를 0으로 해서 이 버퍼에 CPU는 접근 할 수 없음.

	if (FAILED(result))
	{
		return nullptr;
	}
	else
	{
		BufferList.push_back(pVertexBuffer);
		return pVertexBuffer;
	}
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

ID3D11ShaderResourceView* DXShaderManager::CreateShaderResourceViewBuffer(ID3D11Buffer* buffer)
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	buffer->GetDesc(&descBuf);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer

		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		desc.BufferEx.NumElements = descBuf.ByteWidth / 4;
	}
	else
	{
		if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			// This is a Structured Buffer	

			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
		}
	}

	ID3D11ShaderResourceView* pSRV = nullptr;
	HRESULT result = m_pd3dDevice->CreateShaderResourceView(buffer, &desc, &pSRV);
	if (FAILED(result))
	{
		return nullptr;
	}
	else
	{
		SRVList.push_back(pSRV);
		return pSRV;
	}
}

ID3D11InputLayout* DXShaderManager::GetInputLayout(InputLayoutType type)
{
	auto it = InputLayoutMap.find(type);
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
		OutputDebugStringA("DXShaderManager::getPixelShader::Failed Get Pixel Shader.\n");
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
	if (!LoadVSCode(L"../include/Core/HLSL/VS_StaticMesh.hlsl"))
	{
		OutputDebugStringA("DXShaderManager::initialize::Failed Load VS Code(VS_StaticMesh.hlsl).\n");
		return false;
	}

	if (!LoadVSCode(L"../include/Core/HLSL/VS_SkeletalMesh.hlsl"))
	{
		OutputDebugStringA("DXShaderManager::initialize::Failed Load VS Code(VS_StaticMesh.hlsl).\n");
		return false;
	}

	if (!LoadPSCode(L"../include/Core/HLSL/PS_Default.hlsl"))
	{
		OutputDebugStringA("DXShaderManager::initialize::Failed Load PS Code(PS_Default.hlsl).\n");
		return false;
	}

	if (!LoadPSCode(L"../include/Core/HLSL/PS_Light.hlsl"))
	{
		OutputDebugStringA("DXShaderManager::initialize::Failed Load PS Code(PS_Light.hlsl).\n");
		return false;
	}

	if (!LoadPSCode(L"../include/Core/HLSL/PS_Landscape.hlsl"))
	{
		OutputDebugStringA("DXShaderManager::initialize::Failed Load PS Code(PS_Landscape.hlsl).\n");
		return false;
	}

	if (!LoadCSCode(L"../include/Core/HLSL/CS_LandscapeSplatting.hlsl"))
	{
		OutputDebugStringA("DXShaderManager::initialize::Failed Load CS Code(CS_LandscapeSplatting.hlsl).\n");
		return false;
	}

	/*if (!LoadGSCode(L"../include/Core/HLSL/GS_Landscape.hlsl"))
	{
		OutputDebugStringA("DXShaderManager::initialize::Failed Load GS Code(GS_Landscape.hlsl).\n");
		return false;
	}*/

	CreateVertexShader();
	CreatePixelShader();
	//CreateGeometryShader();
	CreateInputLayout();
	CreateComputeShader();

	//// Load Vertex Shader Code.
	//if (!LoadVSCode(VSCodeType::Texture, "../include/core/HLSL/VS_Texture.hlsl"))
	//{
	//	OutputDebugStringA("DXShaderManager::initialize::Failed Load VS Code(VS_Texture.hlsl).\n");
	//	return false;
	//}

	//if (!LoadVSCode(VSCodeType::ConstantBuffer, "../include/core/HLSL/VS_ConstantBuffer.hlsl"))
	//{
	//	OutputDebugStringA("DXShaderManager::initialize::Failed Load VS Code(VS_ConstantBuffer.hlsl).\n");
	//	return false;
	//}

	//if (!LoadVSCode(VSCodeType::Light, "../include/core/HLSL/VS_Light.hlsl"))
	//{
	//	OutputDebugStringA("DXShaderManager::initialize::Failed Load VS Code(VS_Light.hlsl).\n");
	//	return false;
	//}

	//if (!LoadVSCode(VSCodeType::Animation, "../include/core/HLSL/VS_Animation.hlsl"))
	//{
	//	OutputDebugStringA("DXShaderManager::initialize::Failed Load VS Code(VS_Animation.hlsl).\n");
	//	return false;
	//}

	//// Load Pixel Shader Code.
	//if (!LoadPSCode(PSCodeType::Normal, "../include/core/HLSL/PS_Default.hlsl"))
	//{
	//	OutputDebugStringA("DXShaderManager::initialize::Failed Load PS Code(PS_Default.hlsl).\n");
	//	return false;
	//}

	//if (!LoadPSCode(PSCodeType::Texture, "../include/core/HLSL/PS_Texture.hlsl"))
	//{
	//	OutputDebugStringA("DXShaderManager::initialize::Failed Load PS Code(PS_Texture.hlsl).\n");
	//	return false;
	//}

	//if (!LoadPSCode(PSCodeType::Mask, "../include/core/HLSL/PS_Mask.hlsl"))
	//{
	//	OutputDebugStringA("DXShaderManager::initialize::Failed Load PS Code(PS_Mask.hlsl).\n");
	//	return false;
	//}

	//if (!LoadPSCode(PSCodeType::Light, "../include/core/HLSL/PS_Light.hlsl"))
	//{
	//	OutputDebugStringA("DXShaderManager::initialize::Failed Load PS Code(PS_Light.hlsl).\n");
	//	return false;
	//}

	//// Create Vertex Shader
	//if (!CreateVertexShader())
	//{
	//	OutputDebugStringA("DXShaderManager::initialize::Failed Create Vertex Shader.\n");
	//	return false;
	//}

	//// Create Pixel Shader
	//if (!CreatePixelShader())
	//{
	//	OutputDebugStringA("DXShaderManager::initialize::Failed Create Pixel Shader.\n");
	//	return false;
	//}

	return true;
}

bool DXShaderManager::Release()
{
	for (auto it : VertexShaderCodeMap)
	{
		ID3DBlob* pVSCode = it.second;
		pVSCode->Release();
		pVSCode = nullptr;
	}
	VertexShaderCodeMap.clear();

	for (auto it : PixelShaderCodeMap)
	{
		ID3DBlob* pPSCode = it.second;
		pPSCode->Release();
		pPSCode = nullptr;
	}
	PixelShaderCodeMap.clear();

	for (auto it : VertexShaderMap)
	{
		ID3D11VertexShader* pVS = it.second;
		pVS->Release();
		pVS = nullptr;
	}
	VertexShaderMap.clear();

	for (auto it : PixelShaderMap)
	{
		ID3D11PixelShader* pPS = it.second;
		pPS->Release();
		pPS = nullptr;
	}
	PixelShaderMap.clear();

	for (auto it : BufferList)
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

ID3D11DeviceContext* DXShaderManager::GetContext()
{
	return m_pImmediateContext;
}
