#pragma once
#include "Define.h"

/**
* @enum TimelineState
* @brief Ÿ�Ӷ����� ��� ���� ǥ�ø� ���� ������ �ڷ�
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
* @brief [C] Ÿ�Ӷ��� ������ ���� ����ü
*/
class TimelineComponent
{
	float			From;						///< ���� ��
	float			To;							///< �� ��
	float*			Result = nullptr;			///< �����Ǵ� ��(�����ͷ� �Ҵ��ؼ� �ǽð� ������Ʈ ����)
	float			elapsedTime = 0.f;			///< ��� �ð�
	float			endTime = 0.f;				///< Ÿ�Ӷ����� ����
	TimelineState	State = Start;				///< Ÿ�Ӷ����� ��� ���� ǥ��
	float			Speed = 1.f;				///< ��� �ӵ� ����
	float			Inverse = 1.f;				///< ����� ����

public:
	TimelineComponent() {};

	/**
	* @brief Ÿ�Ӷ��� ������
	* @param[in] start ������
	* @param[in] goal ����
	* @param[in] time Ÿ�Ӷ��� ����
	*/
	TimelineComponent(float start, float goal, float time);
	~TimelineComponent() {};

	/**
	* @brief ��� ���� Ÿ�Ӷ����� ������ ƽ���� ������Ʈ
	* @param[in] tick ƽ
	*/
	void Update(float tick);

	/**
	* @brief Ÿ�Ӷ��� ����
	* @param[in] start ������
	* @param[in] goal ����
	* @param[in] time Ÿ�Ӷ��� ����
	*/
	void CreateTimeline(float start, float goal, float time);
	
	/**
	* @brief ������Ʈ �Ǵ� ���� ����
	* @param[in] result ������Ʈ�Ǵ� ��� ���� ������
	*/
	void SetTarget(float* result);

	/**
	* @brief Ÿ�Ӷ����� �ӵ� ����
	* @param[in] speed �⺻���� 1.0f
	*/
	void SetSpeed(float speed);

	/**
	* @brief ����� ����
	* @param[in] setter true -> �����
	*/
	void SetInverse(bool setter);

	/**
	* @brief Ÿ�Ӷ��� ���
	*/
	void Play();

	/**
	* @brief Ÿ�Ӷ��� ó������ ���
	*/
	void PlayFromStart();

	/**
	* @brief Ÿ�Ӷ��� ���߱�
	*/
	void Stop();

	/**
	* @brief Ÿ�Ӷ��� �����
	*/
	void Reverse();

	/**
	* @brief ���������� �����
	*/
	void PlayFromEnd();

	/**
	* @brief ���� Result�� �� �޾ƿ���
	*/
	float GetUpdate();

	/**
	* @brief ���� Ÿ�Ӷ����� State �޾ƿ���
	*/
	TimelineState GetState();

};

