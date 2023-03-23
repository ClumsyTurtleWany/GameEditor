#include "AnimationComponent.h"


bool AnimationComponent::UpdateAnim(SkeletalMeshComponent* mesh, float tick)
{
	if (IsDead)
	{
		return true;
	}
	if (CurrentClip == nullptr)
	{
		SetClipByName(L"Idle");
		return false;
	}
	m_currentAnimationFrame += (tick * CurrentClip->FrameSpeed * m_AnimationInverse);
	if ((m_currentAnimationFrame > CurrentClip->EndFrame) ||
		(m_currentAnimationFrame < CurrentClip->StartFrame))
	{
		if (CurrentClipName == L"Dying")
		{
			IsDead = true;
			return true;
		}
		m_currentAnimationFrame = min(m_currentAnimationFrame, CurrentClip->EndFrame);
		m_currentAnimationFrame = max(m_currentAnimationFrame, CurrentClip->StartFrame);
		//m_AnimationInverse *= -1.0f;
		if (CurrentClip->LoopState)
		{
			m_currentAnimationFrame = 0.f;
		}
		else
		{
			CurrentClip = nullptr;
			m_currentAnimationFrame = 0.f;
			return true;
		}
	}

	UINT InterpolationIdx = m_currentAnimationFrame * 100; // InterpolationSampling 일단 100 고정


	if (mesh->BindPoseMap.empty())
	{
		auto it = CurrentClip->LerpFrameMatrixList.find(mesh->Name);
		if (it != CurrentClip->LerpFrameMatrixList.end())
		{
			Matrix matTranspose = it->second[InterpolationIdx].Transpose();
			//ConstantBufferData_Bone CBData_Bone;
			mesh->BPAData.Bone[0] = matTranspose;
		}
	}
	else
	{
		size_t BoneIdx = 0;
		for (auto& it : mesh->BindPoseMap)
		{
			auto AnimationTrack = CurrentClip->LerpFrameMatrixList.find(it.first);
			if (AnimationTrack == CurrentClip->LerpFrameMatrixList.end())
			{
				mesh->BPAData.Bone[BoneIdx++] = it.second;
			}
			else
			{
				Matrix MergedMatrix = it.second * AnimationTrack->second[InterpolationIdx];
				mesh->BPAData.Bone[BoneIdx++] = MergedMatrix.Transpose();
			}
		}

	}

	return true;
}

bool AnimationComponent::AddClip(std::wstring name, AnimationClip* clip)
{

	auto ret = ClipList.insert(std::make_pair(name, clip));

	return ret.second;
}

bool AnimationComponent::SetClipByName(std::wstring name)
{
	auto ret = ClipList.find(name);
	if (ret != ClipList.end())
	{
		CurrentClip = ret->second;
		CurrentClipName = name;
		m_currentAnimationFrame = 0.f;
	}
	else
	{
		return false;
	}


	return true;
}

AnimationComponent::~AnimationComponent()
{
	for (auto it = ClipList.begin(); it != ClipList.end(); ++it) {
		delete it->second;
	}

	ClipList.clear();
}