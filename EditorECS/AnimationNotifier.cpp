#include "AnimationNotifier.h"


NotifierMgrComponent::~NotifierMgrComponent()
{
	for (auto iter = NotifierList.begin() ; iter != NotifierList.end() ; iter++)
	{
		if (iter->second->Sound != nullptr) 
		{
			delete iter->second->Sound;
			iter->second->Sound = nullptr;
		}
		if (iter->second->Effect != nullptr)
		{
			delete iter->second->Effect;
			iter->second->Effect = nullptr;
		}
		delete iter->second;
	}
	NotifierList.clear();
}

Notifier* NotifierMgrComponent::CreateNotifier(std::wstring lable, unsigned int frame, unsigned int lifespan)
{
	auto iter = NotifierList.find(lable);
	if (iter != NotifierList.end())
	{
		return iter->second;
	}

	Notifier* newNot = new Notifier;

	newNot->Lable = lable;
	newNot->StartFrame = frame;
	newNot->Lifespan = lifespan;

	NotifierList.insert(std::make_pair(lable, newNot));

	return newNot;
}

SoundDesc* NotifierMgrComponent::MakeSound(std::wstring lable, std::wstring soundname, float volume, bool loop)
{
	auto iter = NotifierList.find(lable);
	if (iter != NotifierList.end())
	{
		Notifier* noti = iter->second;
		SoundDesc* newSound = new SoundDesc;
		newSound->SoundName = soundname;
		newSound->Volume = volume;
		newSound->Loop = loop;

		noti->Sound = newSound;
		return newSound;
	}
	else
		return nullptr;
}

bool NotifierMgrComponent::MakeFmodSound(std::wstring lable, FMODSound* FSound)
{
	auto iter = NotifierList.find(lable);
	if (iter != NotifierList.end() && FSound != nullptr)
	{
		Notifier* noti = iter->second;

		noti->FModSound = FSound;
		return true;
	}
	else
		return false;
}

EffectDesc* NotifierMgrComponent::MakeEffect(std::wstring lable, std::wstring key, TransformComponent tc, EFFECTUTIL::ParticlesystemProperty prop)
{
	auto iter = NotifierList.find(lable);
	if (iter != NotifierList.end())
	{
		Notifier* noti = iter->second;
		EffectDesc* newEffect = new EffectDesc;
		
		newEffect->Key = key;
		newEffect->TransformComp = tc;
		newEffect->PSProp = prop;


		noti->Effect = newEffect;
		return newEffect;
	}
	else
		return nullptr;
}

EffectDesc* NotifierMgrComponent::MakeEffect(std::wstring lable, std::wstring key, Vector3 pos, EFFECTUTIL::ParticlesystemProperty prop)
{
	auto iter = NotifierList.find(lable);
	if (iter != NotifierList.end())
	{
		Notifier* noti = iter->second;
		EffectDesc* newEffect = new EffectDesc;

		newEffect->Key = key;
		newEffect->TransformComp = TransformComponent(pos, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
		newEffect->PSProp = prop;


		noti->Effect = newEffect;
		return newEffect;
	}
	else
		return nullptr;
}


bool NotifierMgrComponent::AddNotifier(const AnimationComponent& animcomp, std::wstring clipname, Notifier* notifier)
{
	auto clp = animcomp.ClipList.find(clipname);
	if (clp != animcomp.ClipList.end())
	{
		notifier->ClipName = clipname;
		clp->second->NotifierList.push_back(notifier);

		return true;
	}
	else
		return false;
}