#pragma once
#include "Define.h"
#include "SkeletalMeshComponent.h"
#include "AnimationComponent.h"
#include "TransformComponent.h"
/**
* @class SocketComponent
* @brief ���� ������Ʈ
*/
class SocketComponent
{
public:
	// Bone Anim Matrix
	Matrix AnimMat;
	// Offset
	Matrix OffsetMat;
	// FinalPos
	TransformMatrix TransformData;
	// Bone Name
	std::string BoneName;

public:
	/**
	 * @brief ������ ��ġ�� ������Ʈ�Ѵ�
	*/
	void Update(const AnimationComponent& anim, const TransformComponent& transform);

	/**
	 * @brief skeletalMeshComponent�� ������ �����Ѵ�
	 * @param[in] mesh �޽��� �̸�
	 * @param[in] bone �� ����� �̸�
	 * @return ���������� �� ��带 ã���� true
	*/
	bool Attach(const SkeletalMeshComponent& mesh, std::string bone);

	/**
	 * @brief ������Ʈ���� �����ġ�� �����Ѵ�
	 * @param[in] T �̵�
	 * @param[in] R ȸ��
	 * @param[in] S ������
	 * @return true
	*/
	bool SetOffset(Vector3 T, Vector3 R, Vector3 S);

	/**
	 * @brief ������ ���� ���� ����� ��´�
	 * @param[in] T �̵�
	 * @param[in] R ȸ��
	 * @param[in] S ������
	 * @return true
	*/
	TransformMatrix GetPosition();
};

