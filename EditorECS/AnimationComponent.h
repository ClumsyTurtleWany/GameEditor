#pragma once
#include "Define.h"
#include "SkeletalMeshComponent.h"

/**
* @class AnimationComponent
* @brief [C] �޽��� �ִϸ��̼� ����� ������Ʈ ���ֱ����� ������ ���� ������Ʈ
*/
class AnimationComponent
{
public:
	
	unsigned int StartFrame; 
	unsigned int EndFrame;
	float TickPerFrame;
	float FrameSpeed;

	
	std::map<std::string, std::vector<Matrix>> LerpFrameMatrixList; ///< FBXLoader::PreProcess()�� GenerateAnimationTrack()���� ������ ���� ���

///////// add
	float m_currentAnimationFrame = 0.f;							///< ƽ���� ������Ʈ�Ǵ� ���� ������
	float m_AnimationInverse = 1.f;									///< ������ ����� ������ǵ��� ���ִ� ����

	// Animation File Name
	std::wstring	FileName;										///< GenerateAnimationFromFileData()���� ���� filename(Fbx ������ �̸�)

public:
	AnimationComponent() {};
	virtual ~AnimationComponent() {};

public:

	virtual bool UpdateAnim(SkeletalMeshComponent* mesh, float tick);	///< ƽ���� m_currentAnimationFrame�� ������Ʈ�ϰ� SkeletalMeshComponent�� BPAData�� ������Ʈ ���ش�.
};