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

	// WIC (Windows Image Component) : DX10 ���� .tga ������ �������� ����(ǥ��ȭ ���� �ʾƼ�). DirectXTex ������ ������. ����� DirectXTK(Tool Kit) ��� ��.
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
		OutputDebugStringA("WanyCore::DXTexture::Failed Create WIC Texture From File.\n");
		rst = DirectX::CreateDDSTextureFromFile(m_pd3dDevice, _filename.c_str(), (ID3D11Resource**)&m_pTextureResource, &m_pTextureResourceView);
		if (FAILED(rst))
		{
			OutputDebugStringA("WanyCore::DXTexture::Failed Create DDS Texture From File.\n");
			//OutputDebugStringA("WanyCore::DXTexture::Failed Load.\n");
			/*std::wstring debugString = L"WanyCore::DXTexture::Failed Load(";
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
	// �ؽ�ó�� �����ϱ� ���ؼ��� CPU�� �����ؾ� �ϰ�,
	// CPU�� �����ϴ� ����� ������¡ �ۿ� ����.
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

	// CPU ���� ���. ���ؽ�Ʈ�� ���� ����.
	D3D11_MAPPED_SUBRESOURCE MappedFaceDest; // �ٽ� ���ҽ�
	//void* pData; // �迭�� ���� �ּ�
	//UINT RowPitch; // ������ ����, DX�� �⺻������ ������ ������ �־ Texture Width * Pixel Byte ���� ū ���� ���� �� �ִ�. 
	//UINT DepthPitch; // 
	rst = m_pImmediateContext->Map(pTexture2D, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_READ, 0, &MappedFaceDest); // CPU�� ����
	if (SUCCEEDED(rst))
	{
		//std::vector<UINT> resultList;
		//resultList.resize(m_Desc.Height * m_Desc.Width);
		MappedResourceData.resize(m_Desc.Height * m_Desc.Width);
		UCHAR* pTexels = (UCHAR*)MappedFaceDest.pData;
		for (UINT row = 0; row < m_Desc.Height; row++)
		{
			UINT rowStart = row * MappedFaceDest.RowPitch; // �ٴ����� �Ѿ����.
			for (UINT col = 0; col < m_Desc.Width; col++)
			{
				UINT colStart = col * 4; // 4Byte�� ������.
				UINT Red = pTexels[rowStart + colStart + 0]; // ��� ��.
				//resultList[row * m_Desc.Width + col] = Red; //static_cast<float>(Red) / 255.0f;
				MappedResourceData[row * m_Desc.Width + col] = Red; //static_cast<float>(Red) / 255.0f;
			}
		}

		m_pImmediateContext->Unmap(pTexture2D, D3D11CalcSubresource(0, 0, 1)); // Unmap ������ ��� �ؽ�ó���� ���� �Ұ�.
		return rst;
	}

	pTexture2D->Release();
	return rst;
}

ID3D11Resource* DXTexture::getResource()
{
	return m_pTextureResource;
}

ID3D11ShaderResourceView* DXTexture::getResourceView()
{
	return m_pTextureResourceView;
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
	// TODO: ���⿡ return ���� �����մϴ�.
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
		OutputDebugStringA("WanyCore::DXTexture::CreateRenderTarget::Failed Create Render Target Texture.\n");
		return false;
	}

	rst = m_pd3dDevice->CreateShaderResourceView(m_pTextureResource, NULL, &m_pTextureResourceView);
	if (FAILED(rst))
	{
		OutputDebugStringA("WanyCore::DXTexture::CreateRenderTarget::Failed Create Render Target Texture Resource View.\n");
		return false;
	}

	return true;
}
