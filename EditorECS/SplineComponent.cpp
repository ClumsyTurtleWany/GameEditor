#include "SplineComponent.h"

SplineComponent::SplineComponent()
{
	m_loopOpt = SPLINE_LOOP_OPT::SPLOPT_NOLOOP_ONEWAY;
	m_fTension = 0.0f;
	m_fTime = 0.0f;
	m_bPaused = true;
}

SplineComponent::SplineComponent(SPLINE_LOOP_OPT SPLOpt, TRANSFORM_KEY* keyList, int iNumCP, bool bPaused, float fTension)
{
	m_loopOpt = SPLOpt;
	m_fTension = fTension;
	m_bPaused = bPaused;

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
			if (m_fTime < 0.0f) { m_fTime = 0.0f; }
			else if (m_fTime > m_CPList[len - 1].fTime)
			{
				switch (m_loopOpt)
				{
				case SPLINE_LOOP_OPT::SPLOPT_NOLOOP_ONEWAY:
				{
					m_curKey = m_CPList[len - 1];
				} break;

				/*case SPLINE_LOOP_OPT::SPLOPT_NOLOOP_ROUNDTRIP:
				{

				}break;*/

				case SPLINE_LOOP_OPT::SPLOPT_LOOP_ONEWAY:
				{
					m_fTime = 0.0f;
					interp(m_fTime);
				}break;

				/*case SPLINE_LOOP_OPT::SPLOPT_LOOP_ROUNDTRIP:
				{

				}break;*/
				}
			}
			else
			{
				interp(m_fTime);
			}
		}
	}

	return true;
}

bool SplineComponent::interp(float time)
{
	UINT p[4] = { 0 };

	Vector3			vPos;
	Quaternion		qRot;
	Vector3			vScale;
	float			t = 0.0f;

	//예외 처리
	size_t len = m_CPList.size();
	if (!len) { return false; }

	//1. 시간에 해당하는 구간 찾기
	for (UINT i = 0; i < len; i++)
	{
		if (time < m_CPList[i].fTime)
		{
			if (i == 0) 
			{
				m_curKey = m_CPList[i];
				return true; 
			}
			else 
			{  
				p[2] = i;
				if (p[1] == 0) { p[0] = 0; }
				if (p[2] == len - 1) { p[3] = p[2]; }
				else
				{
					p[3] = i + 1;
				}
				break;
			}
		}
		else
		{
			p[1] = i;

			if (i == len - 1) 
			{
				m_curKey = m_CPList[i];
				return true; 
			}

		}
	}

	t = (time - m_CPList[p[1]].fTime) / (m_CPList[p[2]].fTime - m_CPList[p[1]].fTime);

	m_curKey.fTime = t;

	if (m_fTension < 0.0f || m_fTension > 1.0f)
	{
		//텐션이 기본값(0.5)으로 적용된 캣-멀롬 스플라인 위치 반환함수
		//p[1]~p[2]에서만 곡선이 정의된다.
		Vector3::CatmullRom(m_CPList[p[0]].vPos,
			m_CPList[p[1]].vPos,
			m_CPList[p[2]].vPos,
			m_CPList[p[3]].vPos,
			t, m_curKey.vPos);
	}
	else
	{
		Catmull_RomSpline(t, m_fTension, m_CPList[p[0]].vPos,
			m_CPList[p[1]].vPos,
			m_CPList[p[2]].vPos,
			m_CPList[p[3]].vPos,
			m_curKey.vPos);
	}
	
	m_curKey.qRot = Quaternion::Slerp(m_CPList[p[1]].qRot, m_CPList[p[2]].qRot, t);
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

void SplineComponent::Catmull_RomSpline(float t, float tension, Vector3& p0, Vector3& p1, Vector3& p2, Vector3& p3, Vector3& pOut)
{
	float fc, ft, ft2, ft3;
	fc = 1.0f;
	ft = t;
	ft2 = t * t;
	ft3 = t * t * t;

	//p(t) = C3 * t^3 + C2 * t^2 + C1 * t + C0
	Vector3 C0, C1, C2, C3;

	C3 = (-tension * p0) + ((2.0f - tension) * p1) + ((tension - 2.0f) * p2) + (tension * p3);
	C2 = (2.0f * tension * p0) + ((tension - 3.0f) * p1) + ((3.0f - 2 * tension) * p2) + (-tension * p3);
	C1 = (-tension * p0) /*+(0.0f * p1)*/ + (tension * p2);// +(0.0f * p3);
	C0 = /*(1.0f * p0);*/ (1.0f * p1); // +(0.0f * p2) + (0.0f * p3);

	pOut = C3 * ft3 + C2 * ft2 + C1 * ft + C0;
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
