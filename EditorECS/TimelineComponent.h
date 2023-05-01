#pragma once
#include "Define.h"

/**
* @enum TimelineState
* @brief 타임라인의 재생 상태 표시를 위한 열거형 자료
*/
enum TimelineState
{
	Start,
	Playing,
	Pause,
	End,
};

/**
* @class TimelineComponent
* @brief [C] 타임라인 정보를 담은 구조체
*/
class TimelineComponent
{
	float			From;						///< 시작 값
	float			To;							///< 끝 값
	float*			Result = nullptr;			///< 보간되는 값(포인터로 할당해서 실시간 업데이트 가능)
	float			elapsedTime = 0.f;			///< 경과 시간
	float			endTime = 0.f;				///< 타임라인의 길이
	TimelineState	State = Start;				///< 타임라인의 재생 상태 표시
	float			Speed = 1.f;				///< 재생 속도 조절
	float			Inverse = 1.f;				///< 역재생 조절

public:
	TimelineComponent() {};

	/**
	* @brief 타임라인 생성자
	* @param[in] start 시작점
	* @param[in] goal 끝점
	* @param[in] time 타임라인 길이
	*/
	TimelineComponent(float start, float goal, float time);
	~TimelineComponent() {};

	/**
	* @brief 재생 중인 타임라인의 정보를 틱마다 업데이트
	* @param[in] tick 틱
	*/
	void Update(float tick);

	/**
	* @brief 타임라인 설정
	* @param[in] start 시작점
	* @param[in] goal 끝점
	* @param[in] time 타임라인 길이
	*/
	void CreateTimeline(float start, float goal, float time);
	
	/**
	* @brief 업데이트 되는 변수 설정
	* @param[in] result 업데이트되는 결과 값의 포인터
	*/
	void SetTarget(float* result);

	/**
	* @brief 타임라인의 속도 조절
	* @param[in] speed 기본값은 1.0f
	*/
	void SetSpeed(float speed);

	/**
	* @brief 역재생 설정
	* @param[in] setter true -> 역재생
	*/
	void SetInverse(bool setter);

	/**
	* @brief 타임라인 재생
	*/
	void Play();

	/**
	* @brief 타임라인 처음부터 재생
	*/
	void PlayFromStart();

	/**
	* @brief 타임라인 멈추기
	*/
	void Stop();

	/**
	* @brief 타임라인 역재생
	*/
	void Reverse();

	/**
	* @brief 끝에서부터 역재생
	*/
	void PlayFromEnd();

	/**
	* @brief 현재 Result의 값 받아오기
	*/
	float GetUpdate();

	/**
	* @brief 현재 타임라인의 State 받아오기
	*/
	TimelineState GetState();

};

