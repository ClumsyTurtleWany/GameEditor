#pragma once
#include "FMODSound.hpp"

enum class SoundType
{
	BGM,
	Effect,
};

class FMODSoundManager : public Singleton<FMODSoundManager>
{
private:
	friend class Singleton<FMODSoundManager>;

	FMOD::System* m_pFmodSystem = nullptr;
	std::map<std::wstring, FMODSound*> m_SoundList;

private:
	FMODSoundManager() {};
	~FMODSoundManager() { Release(); };

public:
	bool Load(std::wstring _filename, SoundType _type = SoundType::Effect);
	bool LoadDir(std::wstring _path);
	FMODSound* GetSound(std::wstring _filename);

public:
	bool Initialize();
	bool Frame();
	bool Render();
	bool Release();

};