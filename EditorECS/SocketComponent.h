#pragma once
#include "Define.h"
#include "SkeletalMeshComponent.h"
#include "AnimationComponent.h"
#include "TransformComponent.h"
/**
* @class SocketComponent
* @brief 소켓 컴포넌트
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
	// Bone Idx
	unsigned int BoneIdx;

public:
	/**
	 * @brief 소켓의 위치를 업데이트한다
	*/
	void Update(const SkeletalMeshComponent& mesh, const TransformComponent& transform);

	/**
	 * @brief skeletalMeshComponent에 소켓을 부착한다
	 * @param[in] mesh 메시의 이름
	 * @param[in] bone 뼈 노드의 이름
	 * @return 정상적으로 뼈 노드를 찾으면 true
	*/
	bool Attach(const SkeletalMeshComponent& mesh, std::string bone);

	/**
	 * @brief 오브젝트와의 상대위치를 설정한다
	 * @param[in] T 이동
	 * @param[in] R 회전
	 * @param[in] S 스케일
	 * @return true
	*/
	bool SetOffset(Vector3 T, Vector3 R, Vector3 S);

	/**
	 * @brief 소켓의 최종 월드 행렬을 얻는다
	 * @return TransformMatrix를 리턴한다
	*/
	TransformMatrix GetTransformMatrix();

	/**
	 * @brief 소켓의 위치를 얻는다
	 * @return Translation을 리턴한다
	*/
	Vector3 GetTranslation() 
	{
		return T;
	}
	/**
	 * @brief 소켓의 회전 정보를 얻는다
	 * @return Rotation을 리턴한다
	*/
	Vector3 GetRotation()
	{
		Vector3 R = Q.ToEuler();
		return R;
	}
	/**
	 * @brief 소켓의 스케일 값을 얻는다
	 * @return Scale을 리턴한다
	*/
	Vector3 GetScale()
	{
		return S;
	}
};

