#include "AnimationComponent.h"
#include <fstream>

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
	 if(BlendClip(mesh , tick, 0.2f)) // true면 애니메이션 전환중 , 0.5초동안 블렌딩
	{
		return true;
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
			//CurrentClip = nullptr;
			SetClipByName(L"Idle");
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
	int pointer = name.size() - 1;
	if (name[pointer] == NULL)
	{
		name.pop_back();
	}

	auto ret = ClipList.find(name);
	if (ret != ClipList.end())
	{
		PrevClip = CurrentClip;
		CurrentClip = ret->second;
		CurrentClipName = name;
		lastFrame = m_currentAnimationFrame;
		m_currentAnimationFrame = 0.f;
	}
	else
	{
		return false;
	}


	return true;
}


bool AnimationComponent::IsInAction()
{
	if ((CurrentClip == nullptr) || IsDead)
	{
		return false;
	}
	else if (CurrentClip->FileName == L"Idle")
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

void AnimationComponent::Notify(ECS::World* world)
{
	if (CurrentClip != nullptr && !CurrentClip->NotifierList.empty())
	{
		for (auto data : CurrentClip->NotifierList)
		{
			if (data->StartFrame <= (unsigned int)m_currentAnimationFrame && (unsigned int)m_currentAnimationFrame < (data->StartFrame + data->Lifespan))
			{
				if (!data->IsOn)
				{
					data->IsOn = true;
					world->emit<Notifier>(*data);
				}
			}
			else if ((data->StartFrame + data->Lifespan) <= (unsigned int)m_currentAnimationFrame)
			{
				data->IsOn = false;
			}
		}
	}
}



///////////////////////////////// 애니메이션 전환 블렌딩 ////////////////////////
//	update anim 상단에서 애니메이션 전환이 먼저 호출 
// bool ChangeAnim(tick) . static time
// if(prev != current) -> ChangeAnim
// 프레임 값 들고 있으니까 그거로 prev matrix 구하기
// 전환될 프레임은 0 고정이니까 current clip mat[0]
//
bool AnimationComponent::BlendClip(SkeletalMeshComponent* mesh, float tick, float time)
{
	if (PrevClip == nullptr || PrevClip == CurrentClip)
	{
		return false;
	}
	//static float elapsedT= 0.f;
	elapsedT += tick;
	float interpolateT = elapsedT / time;
	if (interpolateT >= 1.0f)
	{
		PrevClip = CurrentClip;
		elapsedT = 0.f;
		m_currentAnimationFrame = 0.f;
		return false;
	}

	size_t BoneIdx = 0;
	int k = m_currentAnimationFrame;
	

	for (auto& it : mesh->BindPoseMap)
	{
		auto AnimationTrackA = PrevClip->LerpFrameMatrixList.find(it.first);
		auto AnimationTrackB = CurrentClip->LerpFrameMatrixList.find(it.first);
		
		Matrix A = AnimationTrackA->second[lastFrame * 100];
		Matrix B = AnimationTrackB->second[CurrentClip->StartFrame];
		Matrix InterpolatedMat;
		Vector3 A_s, A_t, B_s, B_t;
		Quaternion A_r, B_r;

		A.Decompose(A_s, A_r, A_t);
		B.Decompose(B_s, B_r, B_t);

		//Vector3 I_s = DirectX::XMVectorLerp(A_s, B_s, interpolateT);
		Vector3 I_s = B_s;
		Quaternion I_r = DirectX::XMQuaternionSlerp(A_r, B_r, interpolateT);
		Vector3 I_t = DirectX::XMVectorLerp(A_t, B_t, interpolateT);

		//InterpolatedMat = DirectX::XMMatrixTransformation(Vector3(0.0f, 0.0f, 0.0f),DirectX::XMQuaternionIdentity(), I_s, Vector3(0.0f, 0.0f, 0.0f), I_r, I_t);

		Matrix matScale;
		matScale._11 = I_s.x;
		matScale._22 = I_s.y;
		matScale._33 = I_s.z;
		Matrix matRotation = DirectX::XMMatrixRotationQuaternion(I_r);
		InterpolatedMat = matScale * matRotation;
		InterpolatedMat._41 = I_t.x;
		InterpolatedMat._42 = I_t.y;
		InterpolatedMat._43 = I_t.z;



		//Matrix MergedMatrix;
		//using namespace DirectX::SimpleMath;							// * 연산자 모호성 이슈로 네임스페이스 사용해봄
		//{
		//	Matrix temp = it.second * InterpolatedMat;
		//	MergedMatrix = temp;
		//}
		Matrix MergedMatrix = it.second * InterpolatedMat;
		mesh->BPAData.Bone[BoneIdx++] = MergedMatrix.Transpose();

		
	}
	return true;
}

// 
//for (size_t NodeIdx = 0; NodeIdx < NodeNum; NodeIdx++)
// for(auto size : CurrentClip->lerpframematlist)
//{
//	
//	data->AnimationTrackMap.insert(std::make_pair(NodeName, TrackList[NodeIdx]));
//
//	// Generate Matrix List Map of Interpolation Animation.
//	size_t TrackSize = TrackList[NodeIdx].size();
//	std::vector<Matrix> InterpolationMatrixList;
//	//InterpolationMatrixList.resize(TrackSize * _data->InterpolationSampling);
//	for (size_t FrameIdx = 0; FrameIdx < TrackSize; FrameIdx++)
//	{
//		FBXAnimationTrack A, B;
//		UINT FrameA = max(FrameIdx + 0, data->AnimationSceneInfo.StartFrame);
//		UINT FrameB = min(FrameIdx + 1, data->AnimationSceneInfo.EndFrame);
//		A = TrackList[NodeIdx][FrameA];
//		B = TrackList[NodeIdx][FrameB];
//		if (A.Frame == B.Frame) // End Frame
//		{
//			//InterpolationMatrixList[FrameIdx] = TrackList[NodeIdx][FrameIdx].matAnimation;
//			InterpolationMatrixList.push_back(TrackList[NodeIdx][FrameIdx].AnimationMatrix);
//			continue;
//		}
//
//		//float t = (FrameIdx - A.frame) / (B.frame - A.frame);
//		float tick = 1.0f / sampling; // / _data->InterpolationSampling; // Sampling 수 만큼 보간.
//		for (float t = 0.0f; t < 1.0f; t += tick)
//		{
//			Vector3 translation = LinearInterpolation(A.Translation, B.Translation, t);
//			Vector3 scale = LinearInterpolation(A.Scale, B.Scale, t);
//			Matrix matScale;
//			//matScale = matScale.Identity();
//			//matScale.Identity();
//			matScale._11 = scale.x;
//			matScale._22 = scale.y;
//			matScale._33 = scale.z;
//			Vector4 qRotation = SphereLinearInterpolation(A.Rotation, B.Rotation, t);
//			Matrix matRotation = QuaternionToMatrix4x4(qRotation);
//			Matrix rst = matScale * matRotation;
//			rst._41 = translation.x;
//			rst._42 = translation.y;
//			rst._43 = translation.z;
//
//			//InterpolationMatrixList[FrameIdx] = rst;
//			InterpolationMatrixList.push_back(rst);
//		}
//	}
//	data->InterpolationFrameMatrixList.insert(std::make_pair(NodeName, InterpolationMatrixList));
//}

bool AnimationComponent::WriteXML(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	TiXmlElement* pAnimationComponent = new TiXmlElement("AnimationComponent");
	parent->LinkEndChild(pAnimationComponent);

	for (auto& it : ClipList)
	{
		TiXmlElement* pClip = new TiXmlElement("Clip");
		pAnimationComponent->LinkEndChild(pClip);

		if (!WriteString(pClip, it.second->FilePath, "Path"))
		{

		}

		if (!WriteBoolean(pClip, it.second->LoopState, "Loop"))
		{

		}
	}

	return true;
}

bool AnimationComponent::ReadXML(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	TiXmlElement* root = parent->FirstChildElement("AnimationComponent");
	TiXmlElement* pClip = root->FirstChildElement("Clip");
	while (pClip != nullptr)
	{
		std::string filepath;
		if (!ReadString(root, "Path", filepath))
		{
			
		}

		bool loop;
		if (!ReadBoolean(root, "Loop", loop))
		{
			
		}

		if (!Load(filepath, loop))
		{
			
		}

		pClip = pClip->NextSiblingElement();
	}
}

bool AnimationComponent::Load(std::string filename, bool loop)
{
	std::ifstream CLP_in(filename, std::ios::binary);
	if (CLP_in.is_open())
	{
		AnimationClip* newClip = new AnimationClip;
		newClip->FilePath = filename;

		unsigned int uint;
		CLP_in.read(reinterpret_cast<char*>(&uint), sizeof(unsigned int));
		newClip->StartFrame = uint;
		CLP_in.read(reinterpret_cast<char*>(&uint), sizeof(unsigned int));
		newClip->EndFrame = uint;
		float f;
		CLP_in.read(reinterpret_cast<char*>(&f), sizeof(float));
		newClip->TickPerFrame = f;
		CLP_in.read(reinterpret_cast<char*>(&f), sizeof(float));
		newClip->FrameSpeed = f;
		bool b;
		CLP_in.read(reinterpret_cast<char*>(&b), sizeof(bool));
		newClip->LoopState = b;

		int map_size;
		CLP_in.read(reinterpret_cast<char*>(&map_size), sizeof(int));
		for (int i = 0; i < map_size; i++)
		{
			int bonename_size;
			std::string bone;
			std::vector<Matrix> mat_list;

			CLP_in.read(reinterpret_cast<char*>(&bonename_size), sizeof(int));
			bone.resize(bonename_size);
			CLP_in.read(reinterpret_cast<char*>(&bone[0]), bonename_size);
			int list_size;
			CLP_in.read(reinterpret_cast<char*>(&list_size), sizeof(int));
			for (int i = 0; i < list_size; i++)
			{
				Matrix mat;
				CLP_in.read(reinterpret_cast<char*>(&mat), sizeof(Matrix));
				mat_list.push_back(mat);
			}
			int pointer = bone.size() - 1;
			if (bone[pointer] == NULL)
			{
				bone.pop_back();
			}

			newClip->LerpFrameMatrixList.insert(std::make_pair(bone, mat_list));
		}

		int filename_size;
		std::string temp;
		CLP_in.read(reinterpret_cast<char*>(&filename_size), sizeof(int));
		temp.resize(filename_size);
		CLP_in.read(reinterpret_cast<char*>(&temp[0]), filename_size);

		///add-
		newClip->FileName = to_mw(temp);
		int pointer = newClip->FileName.size() - 1;
		if (newClip->FileName[pointer] == NULL)
		{
			newClip->FileName.pop_back();
		}
		///-add

		//newClip->FileName = to_mw(temp);

		newClip->LoopState = loop;	// Default = true

		CLP_in.close();

		AddClip(newClip->FileName, newClip);

	}
	return true;
}