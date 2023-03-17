#pragma once
#include "Define.h"
#include "SkeletalMeshComponent.h"

/**
* @class AnimationComponent
* @brief [C] 메시의 애니메이션 행렬을 업데이트 해주기위한 정보를 담은 컴포넌트
*/
class AnimationComponent
{
public:
	
	unsigned int StartFrame; 
	unsigned int EndFrame;
	float TickPerFrame;
	float FrameSpeed;

	
	std::map<std::string, std::vector<Matrix>> LerpFrameMatrixList; ///< FBXLoader::PreProcess()의 GenerateAnimationTrack()에서 생성된 보간 행렬

///////// add
	float m_currentAnimationFrame = 0.f;							///< 틱마다 업데이트되는 현재 프레임
	float m_AnimationInverse = 1.f;									///< 프레임 연산시 역재생되도록 해주는 변수

	// Animation File Name
	std::wstring	FileName;										///< GenerateAnimationFromFileData()에서 사용된 filename(Fbx 파일의 이름)

public:
	AnimationComponent() {};
	virtual ~AnimationComponent() {};

public:

	virtual bool UpdateAnim(SkeletalMeshComponent* mesh, float tick);	///< 틱마다 m_currentAnimationFrame을 업데이트하고 SkeletalMeshComponent의 BPAData를 업데이트 해준다.
};