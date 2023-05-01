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
	 if(BlendClip(mesh , tick, 0.2f)) // true면 애니메이션 전환중 , 0.2초동안 블렌딩
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
		if (CurrentClip->LoopState)
		{
			m_currentAnimationFrame = 0.f;
		}
		else
		{
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

		for (auto data : CurrentClip->NotifierList) 
		{
			data->Disable = false;
		}
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
			if (data->Disable) 
			{  
				data->IsOn = false;
			}
			else 
			{
				if (data->StartFrame <= (unsigned int)m_currentAnimationFrame && (unsigned int)m_currentAnimationFrame < (data->StartFrame + data->Lifespan))
				{
					if (!data->IsOn)
					{
						data->IsOn = true;
						world->emit<Notifier>(*data);
					}
				}
				else
				{
					data->IsOn = false;
				}
			}
		}
	}
}

bool AnimationComponent::BlendClip(SkeletalMeshComponent* mesh, float tick, float time)
{
	if (PrevClip == nullptr || PrevClip == CurrentClip)
	{
		return false;
	}
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

		Vector3 I_s = B_s;
		Quaternion I_r = DirectX::XMQuaternionSlerp(A_r, B_r, interpolateT);
		Vector3 I_t = DirectX::XMVectorLerp(A_t, B_t, interpolateT);

		Matrix matScale;
		matScale._11 = I_s.x;
		matScale._22 = I_s.y;
		matScale._33 = I_s.z;
		Matrix matRotation = DirectX::XMMatrixRotationQuaternion(I_r);
		InterpolatedMat = matScale * matRotation;
		InterpolatedMat._41 = I_t.x;
		InterpolatedMat._42 = I_t.y;
		InterpolatedMat._43 = I_t.z;

		Matrix MergedMatrix = it.second * InterpolatedMat;
		mesh->BPAData.Bone[BoneIdx++] = MergedMatrix.Transpose();
		
	}
	return true;
}

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

		newClip->FileName = to_mw(temp);
		int pointer = newClip->FileName.size() - 1;
		if (newClip->FileName[pointer] == NULL)
		{
			newClip->FileName.pop_back();
		}

		newClip->LoopState = loop;	// Default = true

		CLP_in.close();

		AddClip(newClip->FileName, newClip);

	}
	return true;
}