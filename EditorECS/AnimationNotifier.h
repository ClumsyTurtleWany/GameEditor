#pragma once
#include "Define.h"
#include "AnimationComponent.h"

/**
* @class AnimationNotifier
* @brief [C] �ִϸ��̼� ��Ƽ���̾ �����ϴ� ������Ʈ
*/
class NotifierMgrComponent
{
public:
	std::map<std::wstring, Notifier*>	 NotifierList;							///< ��Ƽ���̾� ����Ʈ

public:
	NotifierMgrComponent() {};

	virtual ~NotifierMgrComponent();

public:

	/**
	 * @brief ��Ƽ���̾ �����Ѵ�
	 * @param[in] lable	��Ƽ���̾� �̸�
	 * @param[in] frame	���� ������
	 * @param[in] lifespan	��Ƽ���̾� ����(�⺻ 1 ������)
	 * @return �����Ҵ�� Notifier ����
	*/
	Notifier*	CreateNotifier(std::wstring lable, unsigned int frame, unsigned int lifespan = 1);

	/**
	 * @brief ��Ƽ���̾ ���� ��� ������ �޾��ش�
	 * @param[in] lable		��Ƽ���̾��� �̸�
	 * @param[in] soundname	����� ���� ������ �̸�
	 * @param[in] volume	���� ���� ���� �ʱⰪ
	 * @param[in] loop		���� ����
	 * @return �����Ҵ�� SoundDesc ����
	*/
	SoundDesc*	MakeSound(std::wstring lable, std::wstring soundname, float volume, bool loop);

	// FmodSound �ٷ� �Ѱ��� ��
	bool	MakeFmodSound(std::wstring lable, FMODSound* FSound);

	/**
	 * @brief ��Ƽ���̾ ����Ʈ ��� ������ �޾��ش�
	 * @param[in] lable	��Ƽ���̾��� �̸�
	 * @param[in] key	����Ʈ Ű �̸�
	 * @param[in] pos	��ġ ��
	 * @param[in] tc	TransformComponent ����
	 * @param[in] prop	��ƼŬ �ý��� �Ӽ� ����
	 * @return �����Ҵ�� EffectDesc ����
	*/
	EffectDesc* MakeEffect(std::wstring lable, std::wstring key, TransformComponent tc, EFFECTUTIL::ParticlesystemProperty prop );

	// �����ε�
	EffectDesc* MakeEffect(std::wstring lable, std::wstring key, Vector3 pos,  EFFECTUTIL::ParticlesystemProperty prop );

	/**
	 * @brief Ư�� Ŭ���� ��Ƽ���̾ �޾��ش�
	 * @param[in] animcomp	Ŭ���� ��� �ִ� �ִϸ��̼� ������Ʈ
	 * @param[in] clipname	�ִϸ��̼� Ŭ���� �̸�
	 * @param[in] notifier	��Ƽ���̾� ������
	 * @return �ִϸ��̼� Ŭ���� �������� �ʴ´ٸ� false ����
	*/
	bool AddNotifier(const AnimationComponent& animcomp, std::wstring clipname, Notifier* notifier);

};

