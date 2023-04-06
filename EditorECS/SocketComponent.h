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
	// Basic Transform
	Vector3 T;
	DirectX::SimpleMath::Quaternion Q;
	Vector3 S;
	// Final TransformData
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
	 * @return TransformMatrix�� �����Ѵ�
	*/
	TransformMatrix GetTransformMatrix();

	/**
	 * @brief ������ ��ġ�� ��´�
	 * @return Translation�� �����Ѵ�
	*/
	Vector3 GetTranslation() 
	{
		return T;
	}
	/**
	 * @brief ������ ȸ�� ������ ��´�
	 * @return Rotation�� �����Ѵ�
	*/
	Vector3 GetRotation()
	{
		Vector3 R = Q.ToEuler();
		return R;
	}
	/**
	 * @brief ������ ������ ���� ��´�
	 * @return Scale�� �����Ѵ�
	*/
	Vector3 GetScale()
	{
		return S;
	}
};

