#include "SocketComponent.h"

void SocketComponent::Update(const SkeletalMeshComponent& mesh, const TransformComponent& transform)
{
	//if (anim.CurrentClip != nullptr) 
	//{
	//	if (anim.m_currentAnimationFrame <= anim.CurrentClip->EndFrame)
	//	{
	//		AnimMat = anim.CurrentClip->LerpFrameMatrixList.find(BoneName)->second[anim.m_currentAnimationFrame * 100];
	//
	//	}
	//	else
	//	{
	//		AnimMat = anim.PrevClip->LerpFrameMatrixList.find(BoneName)->second[anim.lastFrame * 100];
	//	}
	//}


	AnimMat = mesh.BPAData.Bone[BoneIdx];
	Matrix invBind = mesh.BindPoseMap.find(BoneName)->second.Invert();
	AnimMat = AnimMat * invBind.Transpose();
	AnimMat = AnimMat.Transpose();
	


	Matrix World;
	T = transform.Translation;
	Vector3 R = transform.Rotation;
	S = transform.Scale;
	Vector3 rotation = R / 180.0f * PI;
	DirectX::FXMVECTOR q = DirectX::XMQuaternionRotationRollPitchYawFromVector(rotation);
	World = DirectX::XMMatrixAffineTransformation(S, Vector3(0,0,0), q, T);


	TransformData.Mat = OffsetMat * AnimMat * World ;
	TransformData.InversedMat = DirectX::XMMatrixInverse(0, TransformData.Mat);
	TransformData.Mat.Decompose(S, Q, T);

	TransformData.Mat = TransformData.Mat.Transpose();
}


bool SocketComponent::Attach(const SkeletalMeshComponent& mesh, std::string bone)
{
	auto it = mesh.BindPoseKeyToIndexMap.find(bone);
	if (it != mesh.BindPoseKeyToIndexMap.end())
	{
		BoneName = bone;
		BoneIdx = it->second;
		return true;
	}
	BoneName = "NoBoneFound";
	return false;
}


bool SocketComponent::SetOffset(Vector3 T, Vector3 R, Vector3 S)
{
	Vector3 rotation = R / 180.0f * PI;
	DirectX::FXMVECTOR q = DirectX::XMQuaternionRotationRollPitchYawFromVector(rotation);
	OffsetMat = DirectX::XMMatrixAffineTransformation(S, Vector3(0.0f, 0.0f, 0.0f), q, T);
	return true;
}

TransformMatrix SocketComponent::GetTransformMatrix()
{
	return TransformData;
}