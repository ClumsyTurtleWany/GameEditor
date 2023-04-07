#pragma once
#include "Define.h"


#define Deg2Rad(angle) ((angle * (float)PI) / 180.0f)

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
	Quaternion		qRot;
	Vector3			vScale;

	float			fTime;

	TRANSFORM_KEY() { fTime = -1.0f; }
	TRANSFORM_KEY(float time, Vector3 Pos, Vector3 PitchYawRoll = { 0.0f, 0.0f, 0.0f }, Vector3 Scale = {1.0f, 1.0f, 1.0f})
	{
		fTime = time;

		vPos = Pos;

		Vector3 PYR = { Deg2Rad(PitchYawRoll.x), Deg2Rad(PitchYawRoll.y), Deg2Rad(PitchYawRoll.z) };

		qRot = DirectX::XMQuaternionRotationRollPitchYawFromVector(PitchYawRoll);
		vScale = Scale;
	}

	TRANSFORM_KEY(float time, Vector3 Pos, Quaternion Rot, Vector3 Scale)
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

//Catmull-Rom스플라인을 사용하는 컴포넌트
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
	bool interp(float time);

	void start();
	void pause();
	void reset();

	void Catmull_RomSpline(float t, float tension, Vector3& p0, Vector3& p1, Vector3& p2, Vector3& p3, Vector3& pOut);
};

