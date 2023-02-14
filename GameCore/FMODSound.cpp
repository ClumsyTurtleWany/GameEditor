#include "FMODSound.hpp"

bool FMODSound::Initialize()
{
	//FMOD_RESULT rst = FMOD::System_Create(&m_pFmodSystem);
	//if (rst != FMOD_OK)
	//{
	//	OutputDebugStringA("WanyCore::FMODSound::Initialize::Failed Create System.");
	//	return false;
	//}

	//rst = m_pFmodSystem->init(32, FMOD_INIT_NORMAL, 0); // 32: �ִ� 32�� ���� ���� ��� �� �� �ִ�. �� ���� ��� �� �� ������ �� ���ؿ��� 32�� �̻� ���ʿ�.
	//if (rst != FMOD_OK)
	//{
	//	OutputDebugStringA("WanyCore::FMODSound::Initialize::Failed Initialize System.");
	//	return false;
	//}

	return true;
}

bool FMODSound::SetSystem(FMOD::System* _system)
{
	m_pFmodSystem = _system;
	return true;
}

bool FMODSound::Load(std::string _filename)
{
	// ���� ���� �ε�
	FMOD_RESULT rst = m_pFmodSystem->createSound(_filename.c_str(), FMOD_DEFAULT, nullptr, &m_pSound);
	if (rst != FMOD_OK)
	{
		OutputDebugStringA("WanyCore::FMODSound::load::Failed Load FMODSound File.");
		return false;
	}

	m_pSound->getLength(&TotalTime, FMOD_TIMEUNIT_MS);

	return true;
}

bool FMODSound::Play()
{
	bool isPlaying = false;
	if (m_pChannel != nullptr)
	{
		m_pChannel->isPlaying(&isPlaying);
	}
	
	if (!isPlaying)
	{
		// ���� ����. ����Ǿ�� ä�ο� ��ȯ��.
		// ���� �̹� ��� ���̸�, �ٸ� ä�ο��� ����� ��.
		FMOD_RESULT rst = m_pFmodSystem->playSound(m_pSound, nullptr, false, &m_pChannel);
		if (rst != FMOD_OK)
		{
			OutputDebugStringA("WanyCore::FMODSound::play::Failed Play FMODSound.");
			return false;
		}

	}

	return true;
}

bool FMODSound::PlayEffect()
{
	// Effect ����� ���߷� ����� �����ؾ� �ϱ� ������ 
	// �÷��� �������� ������� ������ ä���� ��ȯ�� ��� �Ѵ�.
	FMOD_RESULT rst = m_pFmodSystem->playSound(m_pSound, nullptr, false, &m_pChannel);
	if (rst != FMOD_OK)
	{
		OutputDebugStringA("WanyCore::FMODSound::play::Failed Play FMODSound.");
		return false;
	}

	return true;
}

bool FMODSound::Pause()
{
	if (m_pChannel == nullptr)
	{
		return false;
	}

	bool paused;
	FMOD_RESULT rst = m_pChannel->getPaused(&paused);
	if (rst != FMOD_OK)
	{
		OutputDebugStringA("WanyCore::FMODSound::pause::Failed Get Paused Flag.");
		return false;
	}

	rst = m_pChannel->setPaused(!paused);
	if (rst != FMOD_OK)
	{
		OutputDebugStringA("WanyCore::FMODSound::pause::Failed Set Paused Flag.");
		return false;
	}

	return true;
}

bool FMODSound::Stop()
{
	FMOD_RESULT rst = m_pChannel->stop();
	if (rst != FMOD_OK)
	{
		OutputDebugStringA("WanyCore::FMODSound::stop::Failed Stop FMODSound.");
		return false;
	}

	return true;
}

bool FMODSound::SetVolume(float _volume)
{
	// ���� ���� 0 ~ 1 �� ���̷� �־�� ��.
	FMOD_RESULT rst = m_pChannel->setVolume(_volume);
	if (rst != FMOD_OK)
	{
		OutputDebugStringA("WanyCore::FMODSound::setVolume::Failed Set Volume.");
		return false;
	}

	return true;
}

bool FMODSound::VolumeUp(float _volume)
{
	// ���� ���� 0 ~ 1 �� ���̷� �־�� ��.
	float val = 0.0f;
	FMOD_RESULT rst = m_pChannel->getVolume(&val);
	if (rst != FMOD_OK)
	{
		OutputDebugStringA("WanyCore::FMODSound::volumeUp::Failed Get Volume.");
		return false;
	}

	val += _volume;
	val = val > 1.0f ? 1.0f : val;
	val = val < 0.0f ? 0.0f : val;

	rst = m_pChannel->setVolume(val);
	if (rst != FMOD_OK)
	{
		OutputDebugStringA("WanyCore::FMODSound::volumeUp::Failed Set Volume.");
		return false;
	}

	return true;
}

bool FMODSound::VolumeDown(float _volume)
{
	float val = 0.0f;
	FMOD_RESULT rst = m_pChannel->getVolume(&val);
	if (rst != FMOD_OK)
	{
		OutputDebugStringA("WanyCore::FMODSound::volumeDown::Failed Get Volume.");
		return false;
	}

	val -= _volume;
	val = val > 1.0f ? 1.0f : val;
	val = val < 0.0f ? 0.0f : val;

	rst = m_pChannel->setVolume(val);
	if (rst != FMOD_OK)
	{
		OutputDebugStringA("WanyCore::FMODSound::volumeDown::Failed Set Volume.");
		return false;
	}
	return false;
}

bool FMODSound::SetLoop(bool _loop)
{
	FMOD_RESULT rst;
	if (_loop)
	{
		rst = m_pSound->setMode(FMOD_LOOP_NORMAL); // ���� ���
		if (rst != FMOD_OK)
		{
			OutputDebugStringA("WanyCore::FMODSound::setLoop::Failed Set Mode.");
			return false;
		}
	}
	else
	{
		rst = m_pSound->setMode(FMOD_LOOP_OFF); // ���� ��� ����
		if (rst != FMOD_OK)
		{
			OutputDebugStringA("WanyCore::FMODSound::setLoop::Failed Set Mode.");
			return false;
		}
	}

	return false;
}

bool FMODSound::Frame()
{
	// Frame�� �� �־�� ��. FMODSound file ũ�⿡ ���� ������ ���� �о���� ��찡 �ְ�, �κи� �о���� ��찡 �־�
	// frame ���� �ֱ������� update�� �̿��Ͽ� ������ �߰������� �о��.
	

	if (m_pChannel != nullptr)
	{
		unsigned int ms = 0;
		FMOD_RESULT rst = m_pChannel->getPosition(&ms, FMOD_TIMEUNIT_MS);
		if (rst != FMOD_OK)
		{
			OutputDebugStringA("WanyCore::FMODSound::frame::Failed Get FMODSound Position.");
			return false;
		}

		TCHAR szBuffer[256] = { 0, };
		/*_stprintf_s(szBuffer, _T("��� �ð�[%02d:%02d:%02d], ��ü �ð�[%02d:%02d:%02d]"),
			(ms / 1000) / 60, (ms / 1000) % 60, (ms / 10) % 60,
			(totalTime / 1000) / 60, (totalTime / 1000) % 60, (totalTime / 10) % 60);*/
	}

	return true;
}

bool FMODSound::Render()
{
	// ���� ����. ����Ǿ�� ä�ο� ��ȯ��.
	//FMOD_RESULT rst = m_pFmodSystem->playSound(m_pSound, nullptr, false, &m_pChannel);
	return true;
}

bool FMODSound::Release()
{
	//if (m_pSound != nullptr)
	//{
	//	m_pSound->release();
	//	m_pSound = nullptr;
	//}

	//if (m_pFmodSystem != nullptr)
	//{
	//	// System�� Close ���� �ʿ���.
	//	m_pFmodSystem->close();
	//	m_pFmodSystem->release();
	//	m_pFmodSystem = nullptr;
	//}
	return true;
}
