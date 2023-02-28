#include "FMODSoundManager.hpp"

bool FMODSoundManager::Load(std::wstring _filename, SoundType _type)
{
	std::filesystem::path path(_filename);
	std::wstring key = path.filename();
	auto it = m_SoundList.find(key);
	if (it != m_SoundList.end())
	{
		return true;
	}

	FMODSound* newSound = new FMODSound;
	newSound->SetSystem(m_pFmodSystem);
	newSound->Initialize();
	std::string filename;
	filename.assign(_filename.begin(), _filename.end());
	if (!newSound->Load(filename))
	{
		delete newSound;

		/*std::wstring debugStr = "FMODSoundManager::Load::Failed Load FMODSound(";
		debugStr += _filename + ").\n";
		OutputDebugStringA(debugStr.c_str());*/
		return false;
	}

	if (_type == SoundType::Effect)
	{
		newSound->SetLoop(false);
	}
	else
	{
		newSound->SetLoop(true);
	}
	
	m_SoundList.insert(std::make_pair(key, newSound));

	return true;
}

bool FMODSoundManager::LoadDir(std::wstring _path)
{
	// recursive_directory_iterator 사용 시 하위 폴더까지 모두 탐색.
	/*std::filesystem::path path(_path);
	for (auto& file : std::filesystem::recursive_directory_iterator(path))
	{
		std::wstring filename = file.path().filename();
		std::wstring str = file.path();
		Load(str);
	}*/

	// directory_iterator 사용 시 현재 폴더만 탐색. 하위 폴더 탐색 시 재귀적으로 폴더 호출 필요.
	// 위의 코드와 아래 코드가 동일하다고 보면 됨.
	std::filesystem::path path(_path);
	for (auto& file : std::filesystem::directory_iterator(path))
	{
		std::wstring filename = file.path().filename();
		std::wstring str = file.path();
		if (file.path().extension() == "")
		{
			std::wstring dir = str + L"/";
			LoadDir(dir);
		}
		else
		{
			Load(str);
		}
	}

	return true;
}

FMODSound* FMODSoundManager::GetSound(std::wstring _filename)
{
	auto it = m_SoundList.find(_filename);
	if (it != m_SoundList.end())
	{
		return it->second;
	}
	return nullptr;
}

bool FMODSoundManager::Initialize()
{
	FMOD_RESULT rst = FMOD::System_Create(&m_pFmodSystem);
	if (rst != FMOD_OK)
	{
		OutputDebugStringA("FMODSoundManager::Initialize::Failed Create System.");
		return false;
	}

	rst = m_pFmodSystem->init(32, FMOD_INIT_NORMAL, 0); // 32: 최대 32개 까지 동시 출력 할 수 있다. 더 많이 사용 할 수 있으나 현 수준에선 32개 이상 불필요.
	if (rst != FMOD_OK)
	{
		OutputDebugStringA("FMODSoundManager::Initialize::Failed Initialize System.");
		return false;
	}

	return true;
}

bool FMODSoundManager::Frame()
{
	FMOD_RESULT rst = m_pFmodSystem->update();
	if (rst != FMOD_OK)
	{
		OutputDebugStringA("FMODSound::frame::Failed Update FMODSound System.");
		return false;
	}

	return true;
}

bool FMODSoundManager::Render()
{
	return false;
}

bool FMODSoundManager::Release()
{
	for (auto it : m_SoundList)
	{
		FMODSound* pSound = it.second;
		pSound->Release();
		delete pSound;
		pSound = nullptr;
	}

	if (m_pFmodSystem != nullptr)
	{
		// System은 Close 먼저 필요함.
		m_pFmodSystem->close();
		m_pFmodSystem->release();
		m_pFmodSystem = nullptr;
	}

	return false;
}
