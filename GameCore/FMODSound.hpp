#pragma once

//www.fmod.com -> Download -> Windows
//C:\Program Files(x86)\FMOD SoundSystem\FMOD Studio API Windows\api\core
// inc, lib
#include "fmod/fmod.h"
#include "fmod/fmod.hpp"
#include "fmod/fmod_errors.h"
#ifdef _DEBUG
#pragma comment(lib, "fmod_vc.lib")
#else
#pragma comment(lib, "fmodL_vc.lib")
#endif

#include "Define.h"

class FMODSound
{
public:
	FMOD::System*	m_pFmodSystem = nullptr;
	FMOD::Sound*	m_pSound; // ���� ���� ex).mp3, .wav, ...
	FMOD::Channel*	m_pChannel; // �ϳ��� ���尡 ���� ����Ŀ�� �л�Ǿ ������ ��. ������ ����ϴ� ��.

public:
	unsigned int TotalTime;

public:
	bool Initialize();
	bool Frame();
	bool Render();
	bool Release();

public:
	bool Load(std::string _filename);
	bool Play();
	bool PlayEffect();
	bool Pause();
	bool Stop();

public:
	bool SetLoop(bool _loop);
	bool SetSystem(FMOD::System* _system);
	bool SetVolume(float _volume);
	bool VolumeUp(float _volume);
	bool VolumeDown(float _volume);
};