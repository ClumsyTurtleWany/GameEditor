#pragma once
#include "Define.h"
#include "SkeletalMeshComponent.h"

/**
* @struct AnimationClip
* @brief �ִϸ��̼� ������ �����ϴ� ����ü
*/
struct AnimationClip
{
	unsigned int StartFrame;
	unsigned int EndFrame;
	float TickPerFrame;
	float FrameSpeed;
	bool	LoopState = true;


	std::map<std::string, std::vector<Matrix>> LerpFrameMatrixList; ///< FBXLoader::PreProcess()�� GenerateAnimationTrack()���� ������ ���� ���
	


	std::wstring	FileName;										///< Fbx ������ �̸�
};

/**
* @class AnimationComponent
* @brief [C] �޽��� �ִϸ��̼� ����� ������Ʈ ���ֱ����� ������ ���� ������Ʈ
*/
class AnimationComponent
{
public:
	bool									IsDead = false;								///< ����������� Ȯ��
	std::wstring							CurrentClipName = L"";						///< ���� �ִϸ��̼� Ŭ���� �̸�
	AnimationClip*							CurrentClip = nullptr;						///< ���� �ִϸ��̼� Ŭ���� ������
	std::map<std::wstring, AnimationClip*>	ClipList;									///< �ִϸ��̼� Ŭ�� �����̳�
	float									m_currentAnimationFrame = 0.f;				///< ƽ���� ������Ʈ�Ǵ� ���� ������
	float									m_AnimationInverse = 1.f;					///< ������ ����� ������ǵ��� ���ִ� ����
	

public:
	AnimationComponent() {};

	/**
	 * @brief ClipList�� ��ȸ�ϸ� �����Ҵ�� AnimationClip�� Delete �� �� Clear()
	*/
	virtual ~AnimationComponent();

public:
	/**
	 * @brief ������Ʈ�� AnimationClip�� �߰��Ѵ�
	 * @param[in] name Ŭ�� ������ �̸�
	 * @param[in] clip AnimationClip ������
	 * @return name�� �ߺ����̶�� Ŭ�� �߰� ���� & false ����
	*/
	bool		 AddClip(std::wstring name, AnimationClip* clip);

	/**
	 * @brief �Է��� Ŭ�� �̸����� �ִϸ��̼� ����
	 * @param[in] name Ŭ�� ������ �̸�
	*/
	bool		 SetClipByName(std::wstring name);

	/**
	 * @brief ĳ������ �ִϸ��̼��� ��������� Ȯ��(Idle ����)
	 * @return �ִϸ��̼� ������̸� true ����
	*/
	bool		 IsInAction();

	/**
	 * @brief ���� FbxObj�� Frame() �Լ��� ������� ����
	 * @detail ƽ���� m_currentAnimationFrame�� ������Ʈ�ϰ� SkeletalMeshComponent�� BPAData�� ������Ʈ ���ش�.
	 * @param[in] mesh SkeletalMeshComponent ������
	 * @param[in] tick ƽ
	*/
	virtual bool UpdateAnim(SkeletalMeshComponent* mesh, float tick);

};