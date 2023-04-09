#pragma once
#include "Define.h"

#define BOUNDEPSILON	0.0005f
#define DEG2RAD(angle) ((angle * (float)PI) / 180.0f)

enum class SPLINE_LOOP_OPT
{
	SPLOPT_NOLOOP_ONEWAY,
	SPLOPT_NOLOOP_ROUNDTRIP,
	SPLOPT_LOOP_ONEWAY,
	SPLOPT_LOOP_ROUNDTRIP,
	NUMBER_OF_SPLINE_LOOP_OPT
};

struct TRANSFORM_KEY
{
	Vector3			vPos;
	Vector3			vRot;
	Quaternion		qRot;
	Vector3			vScale;

	float			fTime;

	TRANSFORM_KEY() { fTime = -1.0f; }
	TRANSFORM_KEY(float time, Vector3 Pos, Vector3 PitchYawRoll = { 0.0f, 0.0f, 0.0f }, Vector3 Scale = {1.0f, 1.0f, 1.0f})
	{
		fTime = time;

		vPos = Pos;
		vRot = { DEG2RAD(PitchYawRoll.x), DEG2RAD(PitchYawRoll.y), DEG2RAD(PitchYawRoll.z) };
		qRot = DirectX::XMQuaternionRotationRollPitchYawFromVector(vRot);

		vScale = Scale;
	}

	TRANSFORM_KEY(float time, Vector3 Pos, Quaternion Rot, Vector3 Scale = { 1.0f, 1.0f, 1.0f })
	{
		fTime = time;

		vPos = Pos;
		qRot = Rot;
		vScale = Scale;
	}

	Matrix getT();
	Matrix getR();
	Matrix getS();

	Matrix getWorld();
};

//Catmull-Rom���ö����� ����ϴ� ������Ʈ
class SplineComponent
{
public:
	std::wstring					m_wszName;

	bool							m_bPaused;
	float							m_fTime;

	SPLINE_LOOP_OPT					m_loopOpt;
	float							m_fTension;

	std::vector<TRANSFORM_KEY>		m_CPList;
	TRANSFORM_KEY					m_curKey;
	
	SplineComponent();
	SplineComponent(SPLINE_LOOP_OPT SPLOpt, TRANSFORM_KEY* keyList, int iNumCP, bool bPaused = true, float fTension = -1.0f);
	~SplineComponent();

	bool update(float dt);
	bool lerp(TRANSFORM_KEY& p0, TRANSFORM_KEY& p1, float time);
	bool interp(float time);

	void start();
	void pause();
	void reset();

	Vector3 HornerRule_VPoly(Vector3* vfactorList, int iNumE, int t);
	void Catmull_RomSpline(float t, float tension, Vector3& p0, Vector3& p1, Vector3& p2, Vector3& p3, Vector3& pOut);
	void Catmull_RomSpline(float t, float tension, Quaternion& p0, Quaternion& p1, Quaternion& p2, Quaternion& p3, Quaternion& pOut);
};

