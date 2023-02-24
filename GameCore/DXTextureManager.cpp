#include "DXTextureManager.hpp"


void DXTextureManager::setDevice(ID3D11Device* _device, ID3D11DeviceContext* _context)
{
	m_pd3dDevice = _device;
	m_pImmediateContext = _context;
}

bool DXTextureManager::Load(std::wstring _filename)
{
	auto it = m_TextureList.find(_filename);
	if (it != m_TextureList.end())
	{
		return true;
	}

	DXTexture* newTexture = new DXTexture;
	newTexture->setDevice(m_pd3dDevice, m_pImmediateContext);
	HRESULT rst = newTexture->Load(_filename);
	if (FAILED(rst))
	{
		delete newTexture;
		OutputDebugStringA("WanyCore::DXTextureManager::Failed Load File.\n");
		return false;
	}

	m_TextureList.insert(std::make_pair(_filename, newTexture));

	return true;
}

bool DXTextureManager::Load(std::string _filename)
{
	std::wstring filename;
	filename.assign(_filename.begin(), _filename.end());

	auto it = m_TextureList.find(filename);
	if (it != m_TextureList.end())
	{
		return true;
	}

	DXTexture* newTexture = new DXTexture;
	newTexture->setDevice(m_pd3dDevice, m_pImmediateContext);
	HRESULT rst = newTexture->Load(filename);
	if (FAILED(rst))
	{
		delete newTexture;
		OutputDebugStringA("WanyCore::DXTextureManager::Failed Load File.\n");
		return false;
	}

	m_TextureList.insert(std::make_pair(filename, newTexture));

	return true;
}

bool DXTextureManager::Load(const char* _filename)
{
	std::wstring filename(_filename, _filename + std::strlen(_filename));

	auto it = m_TextureList.find(filename);
	if (it != m_TextureList.end())
	{
		return true;
	}

	DXTexture* newTexture = new DXTexture;
	newTexture->setDevice(m_pd3dDevice, m_pImmediateContext);
	HRESULT rst = newTexture->Load(filename);
	if (FAILED(rst))
	{
		delete newTexture;
		OutputDebugStringA("WanyCore::DXTextureManager::Failed Load File.\n");
		return false;
	}

	m_TextureList.insert(std::make_pair(filename, newTexture));

	return true;
}

DXTexture* DXTextureManager::getTexture(std::wstring _filename)
{
	auto it = m_TextureList.find(_filename);
	if (it != m_TextureList.end())
	{
		return it->second;
	}
	return nullptr;
}

DXTexture* DXTextureManager::getTexture(std::string _filename)
{
	std::wstring filename;
	filename.assign(_filename.begin(), _filename.end());
	
	auto it = m_TextureList.find(filename);
	if (it != m_TextureList.end())
	{
		return it->second;
	}
	return nullptr;
}

DXTexture* DXTextureManager::getTexture(const char* _filename)
{
	std::wstring filename(_filename, _filename + std::strlen(_filename));

	auto it = m_TextureList.find(filename);
	if (it != m_TextureList.end())
	{
		return it->second;
	}
	return nullptr;
}

ID3D11Texture2D* DXTextureManager::CreateAlphaTexture(int width, int height)
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	float* alphaData = new float[width * height * 4];
	for (size_t row = 0; row < height; row++)
	{
		for (size_t col = 0; col < width; col++)
		{
			float* pixel = &alphaData[width * row * 4 + col * 4];
			pixel[0] = 0.0f;
			pixel[1] = 0.0f;
			pixel[2] = 0.0f;
			pixel[3] = 0.0f;
		}
	}

	ID3D11Texture2D* pTexture;
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = alphaData;
	initData.SysMemPitch = sizeof(float) * 4 * width;
	initData.SysMemSlicePitch = 0;
	HRESULT result = m_pd3dDevice->CreateTexture2D(&desc, &initData, &pTexture);
	if (FAILED(result))
	{
		return nullptr;
	}
	else
	{
		
		return pTexture;
	}

	ID3D11ShaderResourceView* pResourceView;
	result = m_pd3dDevice->CreateShaderResourceView(pTexture, NULL, &pResourceView);
	

	return nullptr;
}

bool DXTextureManager::initialize()
{
	return true;
}

bool DXTextureManager::frame()
{
	return true;
}

bool DXTextureManager::render()
{
	return true;
}

bool DXTextureManager::release()
{
	if (!m_TextureList.empty())
	{
		for (auto it : m_TextureList)
		{
			DXTexture* pTexture = it.second;
			if (pTexture != nullptr)
			{
				pTexture->Release();
				delete pTexture;
			}
		}
		m_TextureList.clear();
	}

	return true;
}