#pragma once
#include "Define.h"
#include "SkeletalMeshComponent.h"
#include "EffectInclude/ParticleEmitter.h"

/**
* @struct SoundDesc
* @brief  사운드 노티파이어의 정보를 담은 구조체
*/
struct SoundDesc
{
	std::wstring	SoundName;										///< 사운드의 이름
	float			Volume;											///< 사운드 볼륨
	bool			Loop;											///< 루프 설정
};

/**
* @struct EffectDesc
* @brief  이펙트 노티파이어의 정보를 담은 구조체
*/
struct EffectDesc
{
	std::wstring						Key;						///< 이펙트의 이름
	TransformComponent					TransformComp;				///< 이펙트 위치 정보
	EFFECTUTIL::ParticlesystemProperty	PSProp;						///< 파티클 속성
};


/**
* @struct Notifier
* @brief 노티파이어의 정보를 담은 구조체 (시작 프레임, 유지 시간 등의 정보 제공)
*/
struct Notifier
{
	std::wstring	ClipName;										///< 애니메이션 클립의 이름
	std::wstring	Lable;											///< 노티파이어의 이름
	unsigned int	StartFrame;										///< 노티파이 시작 프레임
	unsigned int	Lifespan;										///< 노티파이 유지 시간(프레임 단위) , 기본값은 1프레임
	bool			IsOn = false;									///< 노티파이어가 활성화되어 있는지 구분하기 위한 bool 멤버
	FMODSound*		FModSound = nullptr;							///< FmodSound* 
	SoundDesc*		Sound = nullptr;								///< 사운드 정보
	EffectDesc*		Effect = nullptr;								///< 이펙트 정보

};




/**
* @struct AnimationClip
* @brief 애니메이션 정보를 저장하는 구조체
*/
struct AnimationClip
{
	unsigned int StartFrame;
	unsigned int EndFrame;
	float TickPerFrame;
	float FrameSpeed;
	bool	LoopState = true;


	std::map<std::string, std::vector<Matrix>> LerpFrameMatrixList; ///< FBXLoader::PreProcess()의 GenerateAnimationTrack()에서 생성된 보간 행렬

	std::vector<Notifier*>	NotifierList;

	std::wstring			FileName;										///< Fbx 파일의 이름
};

/**
* @class AnimationComponent
* @brief [C] 메시의 애니메이션 행렬을 업데이트 해주기위한 정보를 담은 컴포넌트
*/
class AnimationComponent
{
public:
	bool									IsDead = false;								///< 사망상태인지 확인
	std::wstring							CurrentClipName = L"";						///< 현재 애니메이션 클립의 이름
	AnimationClip*							CurrentClip = nullptr;						///< 현재 애니메이션 클립의 포인터
	std::map<std::wstring, AnimationClip*>	ClipList;									///< 애니메이션 클립 컨테이너
	float									m_currentAnimationFrame = 0.f;				///< 틱마다 업데이트되는 현재 프레임
	float									m_AnimationInverse = 1.f;					///< 프레임 연산시 역재생되도록 해주는 변수
	

public:
	AnimationComponent() {};

	/**
	 * @brief ClipList를 순회하며 동적할당된 AnimationClip을 Delete 한 후 Clear()
	*/
	virtual ~AnimationComponent();

public:
	/**
	 * @brief 컴포넌트에 AnimationClip을 추가한다
	 * @param[in] name 클립 파일의 이름
	 * @param[in] clip AnimationClip 포인터
	 * @return name이 중복값이라면 클립 추가 실패 & false 리턴
	*/
	bool		 AddClip(std::wstring name, AnimationClip* clip);

	/**
	 * @brief 입력한 클립 이름으로 애니메이션 설정
	 * @param[in] name 클립 파일의 이름
	*/
	bool		 SetClipByName(std::wstring name);

	/**
	 * @brief 캐릭터의 애니메이션이 재생중인지 확인(Idle 제외)
	 * @return 애니메이션 재생중이면 true 리턴
	*/
	bool		 IsInAction();

	/**
	 * @brief 기존 FbxObj의 Frame() 함수를 기반으로 구성
	 * @detail 틱마다 m_currentAnimationFrame을 업데이트하고 SkeletalMeshComponent의 BPAData를 업데이트 해준다.
	 * @param[in] mesh SkeletalMeshComponent 포인터
	 * @param[in] tick 틱
	*/
	virtual bool UpdateAnim(SkeletalMeshComponent* mesh, float tick);

	/**
	 * @brief 클립 내에 노티파이어가 있다면 조건 달성시 노티파이 이벤트를 발생시킨다
	 * @param[in] world 이벤트를 emit 시켜줌
	*/
	void Notify(ECS::World* world);
};