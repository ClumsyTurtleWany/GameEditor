#pragma once
#include "Define.h"
#include "SkeletalMeshComponent.h"
#include "EffectInclude/ParticleEmitter.h"

/**
* @struct SoundDesc
* @brief  ���� ��Ƽ���̾��� ������ ���� ����ü
*/
struct SoundDesc
{
	std::wstring	SoundName;										///< ������ �̸�
	float			Volume;											///< ���� ����
	bool			Loop;											///< ���� ����
};

/**
* @struct EffectDesc
* @brief  ����Ʈ ��Ƽ���̾��� ������ ���� ����ü
*/
struct EffectDesc
{
	std::wstring						Key;						///< ����Ʈ�� �̸�
	TransformComponent					TransformComp;				///< ����Ʈ ��ġ ����
	EFFECTUTIL::ParticlesystemProperty	PSProp;						///< ��ƼŬ �Ӽ�
};


/**
* @struct Notifier
* @brief ��Ƽ���̾��� ������ ���� ����ü (���� ������, ���� �ð� ���� ���� ����)
*/
struct Notifier
{
	std::wstring	ClipName;										///< �ִϸ��̼� Ŭ���� �̸�
	std::wstring	Lable;											///< ��Ƽ���̾��� �̸�
	unsigned int	StartFrame;										///< ��Ƽ���� ���� ������
	unsigned int	Lifespan;										///< ��Ƽ���� ���� �ð�(������ ����) , �⺻���� 1������
	bool			IsOn = false;									///< ��Ƽ���̾ Ȱ��ȭ�Ǿ� �ִ��� �����ϱ� ���� bool ���
	bool			Disable = false;								///< ������ ��Ȱ��ȭ, ������ 
	FMODSound* FModSound = nullptr;							///< FmodSound* 
	SoundDesc* Sound = nullptr;								///< ���� ����
	EffectDesc* Effect = nullptr;								///< ����Ʈ ����

};




/**
* @struct AnimationClip
* @brief �ִϸ��̼� ������ �����ϴ� ����ü
*/
struct AnimationClip
{
	unsigned int StartFrame;										///< ���� ������
	unsigned int EndFrame;											///< �� ������
	float		 TickPerFrame;										///< ������ �� ƽ
	float		 FrameSpeed;										///< ������ �ӵ�
	bool		 LoopState = true;									///< ���� ����


	std::map<std::string, std::vector<Matrix>>	LerpFrameMatrixList; ///< FBXLoader::PreProcess()�� GenerateAnimationTrack()���� ������ ���� ���
	std::vector<Notifier*>						NotifierList;		 ///< ��Ƽ���̾� ����Ʈ
	std::wstring								FileName;			 ///< Fbx ������ �̸�
	std::string									FilePath;			 ///< Fbx ������ ���
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

	AnimationClip*							PrevClip = nullptr;							///< ���� �ִϸ��̼� Ŭ���� ������
	AnimationClip*							CurrentClip = nullptr;						///< ���� �ִϸ��̼� Ŭ���� ������
	std::map<std::wstring, AnimationClip*>	ClipList;									///< �ִϸ��̼� Ŭ�� �����̳�

	float									m_currentAnimationFrame = 0.f;				///< ƽ���� ������Ʈ�Ǵ� ���� ������
	float									m_AnimationInverse = 1.f;					///< ������ ����� ������ǵ��� ���ִ� ����
	float									elapsedT = 0.f;								///< �ִϸ��̼� ������ ���� ��� �ð� ����
	float									lastFrame = 0.f;							///< �ִϸ��̼� ������ ���� ���� �ִϸ��̼��� ������ ������ ��


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

	/**
	 * @brief Ŭ�� ���� ��Ƽ���̾ �ִٸ� ���� �޼��� ��Ƽ���� �̺�Ʈ�� �߻���Ų��
	 * @param[in] world �̺�Ʈ�� emit ������
	*/
	void Notify(ECS::World* world);

	/**
	 * @brief �ִϸ��̼��� ����� �� ��, �� �ִϸ��̼��� �����Ѵ�
	 * @param[in] mesh ����� ���̷�Ż �޽�
	 * @param[in] tick ƽ
	 * @return �ִϸ��̼��� ��ȯ ���̸� true ����
	*/
	bool BlendClip(SkeletalMeshComponent* mesh, float tick, float time);
	
public:
	bool WriteXML(TiXmlElement* parent);
	bool ReadXML(TiXmlElement* parent);

public:
	bool Load(std::string filename, bool loop = true);
};