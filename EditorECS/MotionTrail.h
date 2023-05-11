#pragma once
#include "Camera.h"
#include "SkeletalMeshComponent.h"
#include "AnimationComponent.h"
#include "MovementComponent.h"

struct CBUF_MOTION_TRAIL_DATA
{
	Vector3			pos;
	float			timer;

	Vector3			direction;
	float			hitColorDuration;
};

void updateConstBuf(void* pResource, size_t iByteSize, ID3D11Buffer* pBuf, ID3D11DeviceContext* pDContext);

class MotionTrail
{
public:
	ID3D11Device*										m_pDevice;
	ID3D11DeviceContext*								m_pDContext;

	ECS::World*											m_pMainWorld;

	Camera*												m_pMainCam;

	ECS::Entity*										m_pTargetEntity;
	SkeletalMeshComponent*								m_pTargetMesh;
	AnimationComponent*									m_pTargetAnim;

	SkeletalMeshComponent								m_Copy;

	Vector3												m_vForward;

	float												m_spawnTimer;
	float												m_iInitTargetTrailIdx;

	int													m_iTrailNum;
	float												m_fTrailDuration;
	float												m_fTrailSpawnTime;

	std::vector<CBUF_MOTION_TRAIL_DATA>					m_trailData;
	std::vector<AnimationComponent*>					m_pAnimArray;
	std::vector<int>									m_validate;

	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>	m_pCBuf_TRAIL;

	bool												m_bEnable;

public:
	MotionTrail()
	{
		m_pDevice = nullptr;
		m_pDContext = nullptr;

		m_pMainWorld = nullptr;
		m_pMainCam = nullptr;

		m_pTargetEntity = nullptr;
		m_pTargetMesh = nullptr;
		m_pTargetAnim = nullptr;

		m_spawnTimer = 0.0f;
		m_iInitTargetTrailIdx = 0;

		m_iTrailNum = 1;
		m_fTrailDuration = 1.0f;
		m_fTrailSpawnTime = 1.0f;

		m_bEnable = false;
	};

	virtual ~MotionTrail() {};

	HRESULT create(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDContext,
		ECS::World* pMainWorld,
		ECS::Entity* pTarget,
		Camera* pMainCam,
		int iTrailNum,
		float fTrailDuration,
		float fTrailSpawnTime);

	bool targetChange(ECS::Entity* pTarget);

	void play() { m_bEnable = true; m_spawnTimer = 0.0f; };
	void Pause() { m_bEnable = false; };

	bool init() {};
	bool render(float dt);
	bool release();
};