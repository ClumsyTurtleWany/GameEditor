#include "DXTexture.hpp"


void DXTexture::setDevice(ID3D11Device* _device, ID3D11DeviceContext* _context)
{
	m_pd3dDevice = _device;
	m_pImmediateContext = _context;
}

HRESULT DXTexture::Load(std::wstring _filename)
{
	// Texture
	// ID3D11Device* d3dDevice
	// const wchar_t* fileName
	// ID3D11Resource** texture
	// ID3D11ShaderResourceView** textureView

	// WIC (Windows Image Component) : DX10 부터 .tga 파일은 지원하지 않음(표준화 되지 않아서). DirectXTex 에서는 지원함. 현재는 DirectXTK(Tool Kit) 사용 중.
	// DDS (Direct Draw Surface)
	if (_filename.empty())
	{
		return S_FALSE;
	}

	size_t idx = _filename.find_last_of(L".");
	std::wstring extension(_filename.begin() + idx, _filename.end());
	if ((extension == L".tga") || (extension == L".TGA"))
	{
		_filename.erase(_filename.begin() + idx, _filename.end());
		_filename += L".dds";
	}

	m_wstrFileName = _filename;
	HRESULT rst = DirectX::CreateWICTextureFromFile(m_pd3dDevice, _filename.c_str(), (ID3D11Resource**)&m_pTextureResource, &m_pTextureResourceView);
	if (FAILED(rst))
	{
		OutputDebugStringA("DXTexture::Failed Create WIC Texture From File.\n");
		rst = DirectX::CreateDDSTextureFromFile(m_pd3dDevice, _filename.c_str(), (ID3D11Resource**)&m_pTextureResource, &m_pTextureResourceView);
		if (FAILED(rst))
		{
			OutputDebugStringA("DXTexture::Failed Create DDS Texture From File.\n");
			//OutputDebugStringA("DXTexture::Failed Load.\n");
			/*std::wstring debugString = L"DXTexture::Failed Load(";
			debugString += _filename;
			debugString += L")\n";
			OutputDebugStringA(debugString.c_str());*/
			return rst;
		}

		return rst;
	}

	m_pTextureResource->GetDesc(&m_Desc);
	return rst;
}

HRESULT DXTexture::LoadEX(std::wstring _filename)
{
	// 텍스처에 접근하기 위해서는 CPU에 접근해야 하고,
	// CPU에 접근하는 방법은 스테이징 밖에 없다.
	HRESULT rst = DirectX::CreateWICTextureFromFileEx(m_pd3dDevice, _filename.c_str(), 0, D3D11_USAGE_STAGING, 0, D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE, 0, DirectX::WIC_LOADER_DEFAULT,  (ID3D11Resource**)&m_pTextureResource, nullptr);
	if (FAILED(rst))
	{
		return rst;
	}

	ID3D11Texture2D* pTexture2D = NULL;
	rst = pTexture2D->QueryInterface(__uuidof(ID3D11Texture2D), (LPVOID*)&pTexture2D);
	if (FAILED(rst))
	{
		return rst;
	}

	pTexture2D->GetDesc(&m_Desc);

	// CPU 접근 방법. 컨텍스트로 접근 가능.
	D3D11_MAPPED_SUBRESOURCE MappedFaceDest; // 핵심 리소스
	//void* pData; // 배열의 시작 주소
	//UINT RowPitch; // 한줄의 길이, DX는 기본적으로 여분을 가지고 있어서 Texture Width * Pixel Byte 보다 큰 값을 가질 수 있다. 
	//UINT DepthPitch; // 
	rst = m_pImmediateContext->Map(pTexture2D, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_READ, 0, &MappedFaceDest); // CPU에 접근
	if (SUCCEEDED(rst))
	{
		//std::vector<UINT> resultList;
		//resultList.resize(m_Desc.Height * m_Desc.Width);
		MappedResourceData.resize(m_Desc.Height * m_Desc.Width);
		UCHAR* pTexels = (UCHAR*)MappedFaceDest.pData;
		for (UINT row = 0; row < m_Desc.Height; row++)
		{
			UINT rowStart = row * MappedFaceDest.RowPitch; // 줄단위로 넘어가야함.
			for (UINT col = 0; col < m_Desc.Width; col++)
			{
				UINT colStart = col * 4; // 4Byte씩 움직임.
				UINT Red = pTexels[rowStart + colStart + 0]; // 결과 값.
				//resultList[row * m_Desc.Width + col] = Red; //static_cast<float>(Red) / 255.0f;
				MappedResourceData[row * m_Desc.Width + col] = Red; //static_cast<float>(Red) / 255.0f;
			}
		}

		m_pImmediateContext->Unmap(pTexture2D, D3D11CalcSubresource(0, 0, 1)); // Unmap 전에는 어느 텍스처에도 접근 불가.
		return rst;
	}

	pTexture2D->Release();
	return rst;
}

ID3D11Texture2D* DXTexture::GetTexture2D()
{
	return m_pTextureResource;
}

ID3D11Resource* DXTexture::getResource()
{
	return m_pTextureResource;
}

ID3D11ShaderResourceView* DXTexture::getResourceView()
{
	return m_pTextureResourceView;
}

ID3D11UnorderedAccessView* DXTexture::GetUAV()
{
	return m_pTextureUAV;
}

float DXTexture::getWidth()
{
	return static_cast<float>(m_Desc.Width);
}

float DXTexture::getHeight()
{
	return static_cast<float>(m_Desc.Height);
}

std::wstring DXTexture::getFileName()
{
	return m_wstrFileName;
}

std::vector<UINT>& DXTexture::getMappedResource()
{
	return MappedResourceData;
	// TODO: 여기에 return 문을 삽입합니다.
}

void DXTexture::SetTexture2D(ID3D11Texture2D* texture)
{
	m_pTextureResource = texture;
	m_pTextureResource->GetDesc(&m_Desc);
}

void DXTexture::SetSRV(ID3D11ShaderResourceView* srv)
{
	m_pTextureResourceView = srv;
}

void DXTexture::SetUAV(ID3D11UnorderedAccessView* uav)
{
	m_pTextureUAV = uav;
}

bool DXTexture::Release()
{
	if (m_pTextureResourceView != nullptr)
	{
		m_pTextureResourceView->Release();
		m_pTextureResourceView = nullptr;
	}

	if (m_pTextureResource != nullptr)
	{
		m_pTextureResource->Release();
		m_pTextureResource = nullptr;
	}

	return true;
}

bool DXTexture::CreateRenderTarget(float _width, float _height)
{
	ZeroMemory(&m_Desc, sizeof(m_Desc));
	m_Desc.Width = _width;
	m_Desc.Height = _height;
	m_Desc.MipLevels = 1;
	m_Desc.ArraySize = 1;
	m_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_Desc.SampleDesc.Count = 1;
	m_Desc.SampleDesc.Quality = 0;
	m_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	m_Desc.CPUAccessFlags = 0;
	m_Desc.MiscFlags = 0;

	HRESULT rst = m_pd3dDevice->CreateTexture2D(&m_Desc, NULL, &m_pTextureResource);
	if (FAILED(rst))
	{
		OutputDebugStringA("DXTexture::CreateRenderTarget::Failed Create Render Target Texture.\n");
		return false;
	}

	rst = m_pd3dDevice->CreateShaderResourceView(m_pTextureResource, NULL, &m_pTextureResourceView);
	if (FAILED(rst))
	{
		OutputDebugStringA("DXTexture::CreateRenderTarget::Failed Create Render Target Texture Resource View.\n");
		return false;
	}

	return true;
}
