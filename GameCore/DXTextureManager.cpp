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
		OutputDebugString(L"WanyCore::DXTextureManager::Failed Load File.\n");
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
		OutputDebugString(L"WanyCore::DXTextureManager::Failed Load File.\n");
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
		OutputDebugString(L"WanyCore::DXTextureManager::Failed Load File.\n");
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
				pTexture->release();
				delete pTexture;
			}
		}
		m_TextureList.clear();
	}

	return true;
}