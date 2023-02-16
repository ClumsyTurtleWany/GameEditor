#include "pch.h"
#include "FBXObject.hpp"

bool FBXObject::frame(float _dt)
{
	//curPos = Vector3(0.0f, 0.0f, 0.0f) * data.matWorld;

	m_currentAnimationFrame = m_currentAnimationFrame + (_dt * FileData->AnimationSceneInfo.FrameSpeed * m_AnimationInverse);
	if ((m_currentAnimationFrame > FileData->AnimationSceneInfo.EndFrame ) ||
		(m_currentAnimationFrame < FileData->AnimationSceneInfo.StartFrame))
	{
		m_currentAnimationFrame = min(m_currentAnimationFrame, FileData->AnimationSceneInfo.EndFrame);
		m_currentAnimationFrame = max(m_currentAnimationFrame, FileData->AnimationSceneInfo.StartFrame);
		m_AnimationInverse *= -1.0f;
	}

	UINT InterpolationIdx = m_currentAnimationFrame * FileData->InterpolationSampling;
	
	if (BindPoseMap.empty())
	{
		auto it = FileData->InterpolationFrameMatrixList.find(m_strNodeName);
		if (it != FileData->InterpolationFrameMatrixList.end())
		{
			Matrix matTranspose = it->second[InterpolationIdx].Transpose();
			//ConstantBufferData_Bone CBData_Bone;
			BPAnimData.Bone[0] = matTranspose;
		}
	}
	else
	{
		//for (auto Node : FileData->NodeDataList)
		//{
		//	ConstantBufferData_Bone CBData_Bone;
		//	if (Node.AttributeType != FbxNodeAttribute::EType::eMesh)
		//	{
		//		continue;
		//	}
		//	else
		//	{
		//		size_t NodeIdx = 0;
		//		for (auto it : Node.BindPoseMap)
		//		{
		//			auto AnimationTrack = FileData->InterpolationFrameMatrixList.find(it.first);
		//			if (AnimationTrack == FileData->InterpolationFrameMatrixList.end())
		//			{
		//				CBData_Bone.matBone[NodeIdx++] = it.second;
		//			}
		//			else
		//			{
		//				Matrix4x4 MergedMatrix = it.second * AnimationTrack->second[InterpolationIdx];
		//				CBData_Bone.matBone[NodeIdx++] = MergedMatrix.Transpose();
		//			}
		//		}
		//	}

		//	// pAnimationShader->updateConstantBuffer_Bone(&CBData_Bone);
		//}

		size_t BoneIdx = 0;
		for (auto &it : BindPoseMap)
		{
			auto AnimationTrack = FileData->InterpolationFrameMatrixList.find(it.first);
			if (AnimationTrack == FileData->InterpolationFrameMatrixList.end())
			{
				BPAnimData.Bone[BoneIdx++] = it.second;
			}
			else
			{
				Matrix MergedMatrix = it.second * AnimationTrack->second[InterpolationIdx];
				BPAnimData.Bone[BoneIdx++] = MergedMatrix.Transpose();
			}
		}
		
	}
	
	//BoneData.matBone[0] = matTranspose;
	//for (auto it : Materials)
	//{
	//	if (it.Shader == nullptr)
	//	{
	//		continue;
	//	}

	//	DXAnimationShader* pAnimationShader = reinterpret_cast<DXAnimationShader*>(it.Shader);
	//	//pAnimationShader->updateConstantBuffer_Bone(&BoneData);
	//	//pAnimationShader->updateConstantBuffer_Bone(&CBData_Bone);
	//	pAnimationShader->updateConstantBuffer_Bone(&BPAnimData);
	//}

	//for (auto it : child)
	//{
	//	it->frame(_dt);

	//	/*Matrix4x4 matInterpolationAnimation = it->interpolation(m_currentAnimationFrame);
	//	Matrix4x4 matTranspose = matInterpolationAnimation.Transpose();
	//	ConstantBufferData_Bone BoneData;
	//	BoneData.matBone[0] = matTranspose;
	//	for (auto it : Materials)
	//	{
	//		if (it.Shader == nullptr)
	//		{
	//			continue;
	//		}

	//		DXAnimationShader* pAnimationShader = reinterpret_cast<DXAnimationShader*>(it.Shader);
	//		pAnimationShader->updateConstantBuffer_Bone(&BoneData);
	//	}*/
	//}

	return true;
}

//bool FBXObject::setMatrix(Matrix4x4* _world, Matrix4x4* _view, Matrix4x4* _proj)
//{
//	bool isChange = false;
//	if (_world != nullptr)
//	{
//		data.matWorld = (*_world);
//		isChange = true;
//	}
//
//	if (_view != nullptr)
//	{
//		data.matView = (*_view);
//		isChange = true;
//	}
//
//	if (_proj != nullptr)
//	{
//		data.matProj = (*_proj);
//		isChange = true;
//	}
//
//	if (isChange)
//	{
//		//pShader->updateConstantData(&data);
//		for (auto it : Materials)
//		{
//			if (it.Shader == nullptr)
//			{
//				continue;
//			}
//			it.Shader->updateConstantData(&data);
//		}
//
//		Matrix4x4 matWorld = data.matWorld;
//		for (auto it : child)
//		{
//			//if (it->m_wstrNodeType != L"Mesh")
//			//{
//			//	continue;
//			//}
//			
//			it->setMatrix(&matWorld, &data.matView, &data.matProj);
//			//matWorld._41 += 2.0f;
//		}
//
//		return true;
//	}
//	else
//	{
//		return true;
//	}
//}

//bool FBXObject::rotationYawPitchRoll(float _yaw, float _pitch, float _roll)
//{
//	Vector3 translate = Vector3(data.matWorld._41, data.matWorld._42, data.matWorld._43);
//	DirectX::XMMATRIX matWorld = DirectX::XMMatrixRotationRollPitchYaw(_pitch, _yaw, _roll);
//	data.matWorld = *((Matrix*)&matWorld);
//	data.matWorld._41 += translate.x;
//	data.matWorld._42 += translate.y;
//	data.matWorld._43 += translate.z;
//	return true;
//}

Matrix FBXObject::interpolation(float _frame)
{
	/*FBXAnimationTrack A, B;
	UINT StartFrame = m_animationSceneInfo.StartFrame;
	UINT EndFrame = m_animationSceneInfo.EndFrame;
	UINT FrameA = max(_frame + 0, StartFrame);
	UINT FrameB = min(_frame + 1, EndFrame);
	A = m_animationTrackList[FrameA];
	B = m_animationTrackList[FrameB];
	if (A.frame == B.frame)
	{
		return m_animationTrackList[_frame].matAnimation;
	}

	float t = (_frame - A.frame) / (B.frame - A.frame);
	Vector3f translation = LinearInterpolation(A.translation, B.translation, t);
	Vector3f scale = LinearInterpolation(A.scale, B.scale, t);
	Matrix4x4 matScale;
	matScale.Identity();
	matScale._11 = scale.x;
	matScale._22 = scale.y;
	matScale._33 = scale.z;
	Vector4f qRotation = SphereLinearInterpolation(A.rotation, B.rotation, t);
	Matrix4x4 matRotation = QuaternionToMatrix4x4(qRotation);
	Matrix4x4 rst = matScale * matRotation;
	rst._41 = translation.x;
	rst._42 = translation.y;
	rst._43 = translation.z;

	return rst;*/

	return Matrix();
}
