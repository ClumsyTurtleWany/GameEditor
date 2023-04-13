#include "SplineComponent.h"

void QuaternionToEulerRad(Quaternion Q, Vector3 vOut)
{
	//좌표계 축에 대한 회전 연산순서에 따라 달리 결정된다.
	//DX의 PYR연산순서는 YXZ순서를 따르므로 이를 반영한다.
	//각 축의 180도 이상의 회전에 대해 조건처리를 하지 않았으므로 
	//회전의 방향이 항상 반시계 방향이 아닐 수 있음에 유의

	//Yaw - Pitch - Roll
	float x = Q.y;
	float y = Q.x;
	float z = Q.z;
	float w = Q.w;

	float sqX = x * x;
	float sqY = y * y;
	float sqZ = z * z;
	float sqW = w * w;

	float C0 = 2.0f * (y * z + x * w);
	float C1 = -2.0f * (x * z - y * w);
	float C2 = 2.0f * (x * y + z * w);

	float C3 = -sqX - sqY + sqZ + sqW;
	float C4 = sqX - sqY - sqZ + sqW;

	vOut.x = atan2f(C0, C3);
	vOut.y = asinf(C1);
	vOut.z = atan2f(C2, C4);
}

SplineComponent::SplineComponent()
{
	m_loopOpt = SPLINE_LOOP_OPT::SPLOPT_NOLOOP_ONEWAY;
	m_fTension = 0.0f;
	m_fTime = 0.0f;
	m_bPaused = true;
	m_bReverse = false;
}

SplineComponent::SplineComponent(SPLINE_LOOP_OPT SPLOpt, TRANSFORM_KEY* keyList, int iNumCP, bool bPaused, float fTension)
{
	m_loopOpt = SPLOpt;
	m_fTension = fTension;
	m_bPaused = bPaused;
	m_bReverse = false;

	m_fTime = 0.0f;

	for (int i = 0; i < iNumCP; i++) { m_CPList.push_back(*keyList++); }

	m_curKey = m_CPList[0];
}

SplineComponent::~SplineComponent()
{
}

bool SplineComponent::update(float dt)
{
	if (!m_bPaused)
	{
		m_fTime += dt;

		size_t len = m_CPList.size();

		if (len)
		{
			if (len == 1) { m_curKey = m_CPList[0]; return true; }

			if (m_fTime < 0.0f) { m_fTime = 0.0f; }
			else if (m_fTime > m_CPList[len - 1].fTime)
			{
				switch (m_loopOpt)
				{
				case SPLINE_LOOP_OPT::SPLOPT_NOLOOP_ONEWAY:
				{
					m_curKey = m_CPList[len - 1];
					pause();
				} return true;

				case SPLINE_LOOP_OPT::SPLOPT_NOLOOP_ROUNDTRIP:
				{
					if (!m_bReverse)
					{
						m_fTime = 0.0f;
						m_bReverse = true;
					}
					else
					{
						m_bReverse = false;
						pause();
						return true;
					}
				}break;

				case SPLINE_LOOP_OPT::SPLOPT_LOOP_ONEWAY:
				{
					m_fTime = 0.0f;
				}break;

				case SPLINE_LOOP_OPT::SPLOPT_LOOP_ROUNDTRIP:
				{
					if (!m_bReverse)
					{
						m_fTime = 0.0f;
						m_bReverse = true;
					}
					else
					{
						m_bReverse = false;
						m_fTime = 0.0f;
					}
				}break;
				}
			}

			if (len == 2) 
			{
				if (m_bReverse)
				{
					Lerp(m_CPList[1], m_CPList[0], m_fTime);
				}
				else
				{
					Lerp(m_CPList[0], m_CPList[1], m_fTime);
				}
				
			}
			else { SplineInterp(m_fTime); }
		}
	}

	return true;
}

bool SplineComponent::Lerp(TRANSFORM_KEY& p0, TRANSFORM_KEY& p1, float time)
{
	UINT p[4] = { 0 };

	Vector3			vPos;
	Quaternion		qRot;
	Vector3			vScale;

	float tSt = min(p0.fTime, p1.fTime);
	float tEd = max(p0.fTime, p1.fTime);

	float	t = (time - tSt) / (tEd - tSt);
	m_curKey.fTime = t;

	m_curKey.vPos = Vector3::Lerp(p0.vScale, p1.vScale, t);
	m_curKey.qRot = Quaternion::Slerp(p0.qRot, p1.qRot, t);
	m_curKey.vScale = Vector3::Lerp(p0.vScale, p1.vScale, t);

	return true;
}

bool SplineComponent::SplineInterp(float time)
{
	int p[4] = { 0 };

	Vector3			vPos;
	Quaternion		qRot;
	Vector3			vScale;

	//예외 처리
	size_t len = m_CPList.size();
	if (!len) { return false; }

	//1. 시간에 해당하는 보간 구간의 제어점 리스트 구성하기
	if (!m_bReverse)
	{
		for (int i = 0; i < len; i++)
		{
			if (time < m_CPList[i].fTime)
			{
				p[2] = i;

				//왼쪽 제어점 2개
				if (p[1] == 0) { p[0] = 0; }
				else { p[0] = p[1] - 1; }

				//오른쪽 제어점 2개
				if (p[2] == len - 1) { p[3] = p[2]; }
				else { p[3] = p[2] + 1; }
				break;
			}

			p[1] = i;
		}

	}
	else
	{
		p[2] = len - 1;

		for (int i = len - 1; i >= 0; i--)
		{
			if (m_CPList[len - 1].fTime - time > m_CPList[i].fTime)
			{
				p[2] = i;

				//왼쪽 제어점 2개
				if (p[2] == 0) { p[3] = 0; }
				else { p[3] = p[2] - 1; }

				//오른쪽 제어점 2개
				if (p[1] == len - 1) { p[0] = p[1]; }
				else { p[0] = p[1] + 1; }
				break;
			}

			p[1] = i;
		}
	}

	float tSt = min(m_CPList[p[1]].fTime, m_CPList[p[2]].fTime);
	float tEd = max(m_CPList[p[1]].fTime, m_CPList[p[2]].fTime);
	float	t = 0.0f;

	if (!m_bReverse)
	{
		t = (time - tSt) / (tEd - tSt);
	}
	else
	{
		t = (tEd - m_CPList[len - 1].fTime + time) / (tEd - tSt);
	}

	m_curKey.fTime = t;

	if (m_fTension <= 0.0f || m_fTension >= 1.0f)
	{
		//텐션이 기본값(0.5f)으로 적용된 캣-멀롬 스플라인 위치 반환함수
		//p[1]~p[2]에서만 곡선이 정의된다.
		Vector3::CatmullRom(m_CPList[p[0]].vPos,
			m_CPList[p[1]].vPos,
			m_CPList[p[2]].vPos,
			m_CPList[p[3]].vPos,
			t, m_curKey.vPos);

		//회전 문제 해결방법 1 : 각축에 대한 회전 각도를 벡터로 취급한다.
		//회전 변화량을 스플라인으로 보간한다. 같은 시간 구간으로 구성된다면 
		//구간별 회전각도를 다르게 주면 구간마다 애니메이션 속도가 달라진다.
#ifdef PYR_INTERP
		Vector3::CatmullRom(m_CPList[p[0]].vRot,
			m_CPList[p[1]].vRot,
			m_CPList[p[2]].vRot,
			m_CPList[p[3]].vRot,
			t, m_curKey.vRot);
#else 
		//회전 문제 해결방법 2 : 쿼터니언을 직접 보간한다.
		//항상 최단거리로 보간하므로 축 벡터의 PI Rad.이상의 회전각도 보간은
		//가장 가까운 거리로 우선 보간 하므로 이를 유의할 것
		m_curKey.qRot = DirectX::XMVectorCatmullRom(m_CPList[p[0]].qRot,
			m_CPList[p[1]].qRot,
			m_CPList[p[2]].qRot,
			m_CPList[p[3]].qRot,
			t);
#endif //PYR_INTERP
	}
	else
	{
		Catmull_RomSpline(t, m_fTension, m_CPList[p[0]].vPos,
			m_CPList[p[1]].vPos,
			m_CPList[p[2]].vPos,
			m_CPList[p[3]].vPos,
			m_curKey.vPos);

#ifdef PYR_INTERP
		Catmull_RomSpline(t, m_fTension, m_CPList[p[0]].vRot,
			m_CPList[p[1]].vRot,
			m_CPList[p[2]].vRot,
			m_CPList[p[3]].vRot,
			m_curKey.vRot);
#else
		Catmull_RomSpline(t, m_fTension, m_CPList[p[0]].qRot,
			m_CPList[p[1]].qRot,
			m_CPList[p[2]].qRot,
			m_CPList[p[3]].qRot,
			m_curKey.qRot);
#endif //PYR_INTERP
	}
	
#ifdef PYR_INTERP
	m_curKey.qRot = DirectX::XMQuaternionRotationRollPitchYawFromVector(m_curKey.vRot);
#elif USE_SLERP
	m_curKey.qRot = Quaternion::Slerp(m_CPList[p[1]].qRot, m_CPList[p[2]].qRot, t);
#endif //PYR_INTERP

	m_curKey.vScale = Vector3::Lerp(m_CPList[p[1]].vScale, m_CPList[p[2]].vScale, t);

	return true;
}

void SplineComponent::start()
{
	m_bPaused = false;
}

void SplineComponent::pause()
{
	m_bPaused = true;
}

void SplineComponent::reset()
{
	m_fTime = 0.0f;
	m_bPaused = true;
}

Vector3 SplineComponent::HornerRule_VPoly(Vector3* vfactorList, int iNumE, int t)
{
	Vector3 ret;

	if (vfactorList && iNumE > 0)
	{
		ret = vfactorList[0];

		//계수 리스트의 순서는 차수의 내림차순
		for (int i = 1; i < iNumE; i++) { ret = (ret * t) + vfactorList[i]; }
	}

	return ret;
}

void SplineComponent::Catmull_RomSpline(float t, float tension, Vector3& p0, Vector3& p1, Vector3& p2, Vector3& p3, Vector3& pOut)
{
	//p(t) = F[0] * t^3 + F[1] * t^2 + F[2] * t + F[3]
	Vector3 F[4];

	F[0] = (-tension * p0) + ((2.0f - tension) * p1) + ((tension - 2.0f) * p2) + (tension * p3);
	F[1] = (2.0f * tension * p0) + ((tension - 3.0f) * p1) + ((3.0f - 2 * tension) * p2) + (-tension * p3);
	F[2] = (-tension * p0) /*+(0.0f * p1)*/ + (tension * p2);// +(0.0f * p3);
	F[3] = /*(1.0f * p0);*/ (1.0f * p1); // +(0.0f * p2) + (0.0f * p3);

	//pOut = HornerRule_VPoly(F, sizeof(F) / sizeof(F[0]), t);
	pOut = F[0];
	for (int i = 1; i < 4; i++) { pOut = (pOut * t) + F[i]; }
}

void SplineComponent::Catmull_RomSpline(float t, float tension, Quaternion& p0, Quaternion& p1, Quaternion& p2, Quaternion& p3, Quaternion& pOut)
{
	//p(t) = F[0] * t^3 + F[1] * t^2 + F[2] * t + F[3]
	Quaternion F[4];

	F[0] = (-tension * p0) + ((2.0f - tension) * p1) + ((tension - 2.0f) * p2) + (tension * p3);
	F[1] = (2.0f * tension * p0) + ((tension - 3.0f) * p1) + ((3.0f - 2 * tension) * p2) + (-tension * p3);
	F[2] = (-tension * p0) /*+(0.0f * p1)*/ + (tension * p2);// +(0.0f * p3);
	F[3] = /*(1.0f * p0);*/ (1.0f * p1); // +(0.0f * p2) + (0.0f * p3);

	//pOut = HornerRule_VPoly(F, sizeof(F) / sizeof(F[0]), t);
	pOut = F[0];
	for (int i = 1; i < 4; i++) { pOut = (pOut * t) + F[i]; }
}

Matrix TRANSFORM_KEY::getT()
{
	return Matrix::CreateTranslation(vPos);
}

Matrix TRANSFORM_KEY::getR()
{
	return Matrix::CreateFromQuaternion(qRot);
}

Matrix TRANSFORM_KEY::getS()
{
	return Matrix::CreateScale(vScale);
}

Matrix TRANSFORM_KEY::getWorld()
{
	return Matrix::CreateScale(vScale) * Matrix::CreateFromQuaternion(qRot) * Matrix::CreateTranslation(vPos);
}
