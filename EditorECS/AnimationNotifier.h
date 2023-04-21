#pragma once
#include "Define.h"
#include "AnimationComponent.h"

/**
* @class AnimationNotifier
* @brief [C] 애니메이션 노티파이어를 관리하는 컴포넌트
*/
class NotifierMgrComponent
{
public:
	std::map<std::wstring, Notifier*>	 NotifierList;							///< 노티파이어 리스트

public:
	NotifierMgrComponent() {};

	virtual ~NotifierMgrComponent();

public:

	/**
	 * @brief 노티파이어를 생성한다
	 * @param[in] lable	노티파이어 이름
	 * @param[in] frame	시작 프레임
	 * @param[in] lifespan	노티파이어 수명(기본 1 프레임)
	 * @return 동적할당된 Notifier 리턴
	*/
	Notifier*	CreateNotifier(std::wstring lable, unsigned int frame, unsigned int lifespan = 1);

	/**
	 * @brief 노티파이어에 사운드 재생 정보를 달아준다
	 * @param[in] lable		노티파이어의 이름
	 * @param[in] soundname	재생할 사운드 파일의 이름
	 * @param[in] volume	사운드 파일 볼륨 초기값
	 * @param[in] loop		루프 여부
	 * @return 동적할당된 SoundDesc 리턴
	*/
	SoundDesc*	MakeSound(std::wstring lable, std::wstring soundname, float volume, bool loop);

	// FmodSound 바로 넘겨줄 때
	bool	MakeFmodSound(std::wstring lable, FMODSound* FSound);

	/**
	 * @brief 노티파이어에 이펙트 재생 정보를 달아준다
	 * @param[in] lable	노티파이어의 이름
	 * @param[in] key	이펙트 키 이름
	 * @param[in] pos	위치 값
	 * @param[in] tc	TransformComponent 정보
	 * @param[in] prop	파티클 시스템 속성 정보
	 * @return 동적할당된 EffectDesc 리턴
	*/
	EffectDesc* MakeEffect(std::wstring lable, std::wstring key, TransformComponent tc, EFFECTUTIL::ParticlesystemProperty prop );

	// 오버로드
	EffectDesc* MakeEffect(std::wstring lable, std::wstring key, Vector3 pos,  EFFECTUTIL::ParticlesystemProperty prop );

	/**
	 * @brief 특정 클립에 노티파이어를 달아준다
	 * @param[in] animcomp	클립을 들고 있는 애니메이션 컴포넌트
	 * @param[in] clipname	애니메이션 클립의 이름
	 * @param[in] notifier	노티파이어 포인터
	 * @return 애니메이션 클립이 존재하지 않는다면 false 리턴
	*/
	bool AddNotifier(const AnimationComponent& animcomp, std::wstring clipname, Notifier* notifier);

};

