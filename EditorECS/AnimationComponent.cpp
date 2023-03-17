#include "AnimationComponent.h"

/**
	 * @brief ���� FbxObj�� Frame() �Լ��� ������� ����
	 * @param[in] mesh SkeletalMeshComponent ������
	 * @param[in] tick ƽ
	*/
bool AnimationComponent::UpdateAnim(SkeletalMeshComponent* mesh, float tick)
{
	m_currentAnimationFrame += (tick * FrameSpeed * m_AnimationInverse);
	if ((m_currentAnimationFrame > EndFrame ) ||
		(m_currentAnimationFrame < StartFrame))
	{
		m_currentAnimationFrame = min(m_currentAnimationFrame, EndFrame);
		m_currentAnimationFrame = max(m_currentAnimationFrame, StartFrame);
		m_AnimationInverse *= -1.0f;
	}

	UINT InterpolationIdx = m_currentAnimationFrame * 100; // InterpolationSampling �ϴ� 100 ����
	
	
	if (mesh->BindPoseMap.empty())
	{
		auto it = LerpFrameMatrixList.find(mesh->Name);
		if (it != LerpFrameMatrixList.end())
		{
			Matrix matTranspose = it->second[InterpolationIdx].Transpose();
			//ConstantBufferData_Bone CBData_Bone;
			mesh->BPAData.Bone[0] = matTranspose;
		}
	}
	else
	{
		size_t BoneIdx = 0;
		for (auto &it : mesh->BindPoseMap)
		{
			auto AnimationTrack = LerpFrameMatrixList.find(it.first);
			if (AnimationTrack == LerpFrameMatrixList.end())
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