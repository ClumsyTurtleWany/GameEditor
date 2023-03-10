#include "FBXObject.hpp"

//bool FBXObject::frame(float _dt)
//{
//	//curPos = Vector3(0.0f, 0.0f, 0.0f) * data.matWorld;
//
//	m_currentAnimationFrame = m_currentAnimationFrame + (_dt * FileData->AnimationSceneInfo.FrameSpeed * m_AnimationInverse);
//	if ((m_currentAnimationFrame > FileData->AnimationSceneInfo.EndFrame ) ||
//		(m_currentAnimationFrame < FileData->AnimationSceneInfo.StartFrame))
//	{
//		m_currentAnimationFrame = min(m_currentAnimationFrame, FileData->AnimationSceneInfo.EndFrame);
//		m_currentAnimationFrame = max(m_currentAnimationFrame, FileData->AnimationSceneInfo.StartFrame);
//		m_AnimationInverse *= -1.0f;
//	}
//
//	UINT InterpolationIdx = m_currentAnimationFrame * FileData->InterpolationSampling;
//	
//	if (BindPoseMap.empty())
//	{
//		auto it = FileData->InterpolationFrameMatrixList.find(m_strNodeName);
//		if (it != FileData->InterpolationFrameMatrixList.end())
//		{
//			Matrix matTranspose = it->second[InterpolationIdx].Transpose();
//			//ConstantBufferData_Bone CBData_Bone;
//			BPAnimData.Bone[0] = matTranspose;
//		}
//	}
//	else
//	{
//		size_t BoneIdx = 0;
//		for (auto &it : BindPoseMap)
//		{
//			auto AnimationTrack = FileData->InterpolationFrameMatrixList.find(it.first);
//			if (AnimationTrack == FileData->InterpolationFrameMatrixList.end())
//			{
//				BPAnimData.Bone[BoneIdx++] = it.second;
//			}
//			else
//			{
//				Matrix MergedMatrix = it.second * AnimationTrack->second[InterpolationIdx];
//				BPAnimData.Bone[BoneIdx++] = MergedMatrix.Transpose();
//			}
//		}
//		
//	}
//
//	return true;
//}
//
