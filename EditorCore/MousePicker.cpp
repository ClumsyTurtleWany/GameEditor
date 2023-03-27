#include "MousePicker.h"

MousePicker::MousePicker()
{
	optPickingMode = PMOD_DEFAULT;
	pTarget = nullptr;
	pMainInput = Input::GetInstance();
}

MousePicker::~MousePicker()
{
}

void MousePicker::Update()
{
	IntersectionDistance = MAX_PICK_DIST;
	pickingTargetDistance = MAX_PICK_DIST;
	pTarget = nullptr;

	ptCursor = pMainInput->m_ptPos;

	ClientWidth = DXDevice::g_ViewPort.Width;
	ClientHeight = DXDevice::g_ViewPort.Height;

	Vector3 v;
	v.x = (((2.0f * ptCursor.x) / ClientWidth) - 1) / Projection._11;
	v.y = -(((2.0f * ptCursor.y) / ClientHeight) - 1) / Projection._22;
	v.z = 1.0f;

	Matrix inversedView = DirectX::XMMatrixInverse(NULL, View);

	PickingRay.position = Vector3(0.0f, 0.0f, 0.0f);
	PickingRay.direction = Vector3(v.x, v.y, v.z);
	PickingRay.position = DirectX::XMVector3TransformCoord(PickingRay.position, inversedView);
	PickingRay.direction = DirectX::XMVector3TransformNormal(PickingRay.direction, inversedView);
	PickingRay.direction.Normalize();
}

void MousePicker::setMatrix(Matrix* pWorld, Matrix* pView, Matrix* pProj)
{
	if (pWorld) { World = *pWorld; }
	if (pView) { View = *pView; }
	if (pProj) { Projection = *pProj; }
}

bool MousePicker::RayToOBB(const DirectX::BoundingOrientedBox& OBB)
{
	//단순 충돌 체크
	//분리축 이론을 이용한다.
	//OBB의 중점과 레이의 시점을 이은 벡터(R = O - C)를 이용해 
	//레이의 방향벡터와 OBB의 각 모서리를 외적해 나온 벡터 축의 검증을 수행하고
	//OBB의 법선 벡터 축의 검증은 반직선의 시점과 방향성 검사로 대신한다.

	//1. OBB의 법선 벡터 축 검사 : 반직선의 시점과 방향성 검사를 함께 수행한다.
	//시점 검사는 OBB의 연장축의 길이와 R을 OBB의 축과 내적해 나온 길이를 비교한다.
	//(OBB의 연장축의 길이) < abs(DOT(R, OBB의 축)) => 외부
	//방향 검사는 레이의 방향벡터와 R과의 내적을 통해 나온 부호로 검증한다.
	// R의 방향이 센터에서 레이의 원점이므로 두 내적의 결과가 양수면 박스를 벗어난 방향으로
	// 레이가 진행한다는 의미가된다.
	//DOT(PickingRay.direction, OBB의 축) * DOT(R, OBB의 축) >= 0.0f => 외부

	//최적화 이후 : OD => Origin Dot / DD => Direction Dot

	float OD[3], ODAbs[3], DD[3], DDAbs[3];
	Vector3 centerToOrigin = PickingRay.position - OBB.Center;

	Vector3 Axis[3] =
	{
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f }
	};

	Matrix Rot = Matrix::CreateFromQuaternion(OBB.Orientation);
	for (int i = 0; i < 3; i++)
	{
		Vector3::Transform(Axis[i], Rot, Axis[i]);
	}
	
	DD[0] = PickingRay.direction.Dot(Axis[0]);
	OD[0] = centerToOrigin.Dot(Axis[0]);
	DDAbs[0] = fabs(DD[0]);
	ODAbs[0] = fabs(OD[0]);
	if (ODAbs[0] > OBB.Extents.x && OD[0] * DD[0] >= 0.0f) { return false; }

	DD[1] = PickingRay.direction.Dot(Axis[1]);
	OD[1] = centerToOrigin.Dot(Axis[1]);
	DDAbs[1] = fabs(DD[1]);
	ODAbs[1] = fabs(OD[1]);
	if (ODAbs[1] > OBB.Extents.y && OD[1] * DD[1] >= 0.0f) { return false; }

	DD[2] = PickingRay.direction.Dot(Axis[2]);
	OD[2] = centerToOrigin.Dot(Axis[2]);
	DDAbs[2] = fabs(DD[2]);
	ODAbs[2] = fabs(OD[2]);
	if (ODAbs[2] > OBB.Extents.x && OD[2] * DD[2] >= 0.0f) { return false; }

	//2. 레이의 방향벡터와 모서리의 외적벡터 축 집합의 검사
	//스칼라 삼중적의 특성을 이용해 식을 변경해 
	//외적을 각 축마다 수행하는 것이 아닌 레이의 방향벡터와 centerToOrigin과의 한번의 외적을
	//반복하여 사용한다.
	//OBB의 투영부도 OBB의 각축의 외적이 다른 OBB의 축이 된다는 점과
	//스칼라 삼중적의 특성, 절댓값이 적용된 외적의 특성을 이용해 구조를 변경한다.
	//DEC => Direction Edge Cross

	float DECAbs[3], OBBProjLen;
	Vector3 vDirXCTO;
	vDirXCTO = PickingRay.direction.Cross(centerToOrigin);
	DECAbs[0] = fabs(vDirXCTO.Dot(Axis[0]));
	OBBProjLen = OBB.Extents.y * DDAbs[2] + OBB.Extents.z * DDAbs[1];
	if (DECAbs[0] > OBBProjLen) { return false; }

	DECAbs[1] = fabs(vDirXCTO.Dot(Axis[1]));
	OBBProjLen = OBB.Extents.x * DDAbs[2] + OBB.Extents.z * DDAbs[0];
	if (DECAbs[1] > OBBProjLen) { return false; }

	DECAbs[2] = fabs(vDirXCTO.Dot(Axis[2]));
	OBBProjLen = OBB.Extents.x * DDAbs[1] + OBB.Extents.y * DDAbs[0];
	if (DECAbs[2] > OBBProjLen) { return false; }

	//교차지점 구하는 부분
	OBB.Intersects(PickingRay.position, PickingRay.direction, IntersectionDistance);

	Intersection = PickingRay.position + (IntersectionDistance * PickingRay.direction);

	return true;
}

bool MousePicker::RayToAABB(const DirectX::BoundingBox& AABB)
{
	//교차지점 구하는 부분
	if (AABB.Intersects(PickingRay.position, PickingRay.direction, IntersectionDistance))
	{
		Intersection = PickingRay.position + (IntersectionDistance * PickingRay.direction);
		return true;
	}

	return false;
}

bool MousePicker::RayToSphere(const DirectX::BoundingSphere& sphere)
{
	float rayDistLimit = MAX_PICK_DIST;

	// (o-c)
	Vector3 vDir = PickingRay.position - sphere.Center;
	// 레이와의 길이 비교를 위한 내적 : d(o-c)
	float  fProj = PickingRay.direction.Dot(vDir);
	// 길이 상한 처리
	if (rayDistLimit < fabs(fProj)) return false;

	//구에 대한 2차 방정식
	//f(p) = |p-c|^2 = (p.x - c.x)^2 + (p.y - c.y)^2 + (p.z - c.z)^2 = r^2
	//교점이 생긴다면 p는 Ray위의 점이 되기 때문에 
	//ray = r(t) = o + td (o : origin, d : direction)라고 하면 2차 방정식을 다음과 같이 나타낼 수 있다.
	//f(p) = |r(t)-c|^2 = |o + td - c|^2
	// => t^2(d*d) + 2td(o-c) + (o-c)(o-c)-r^2 = 0
	// t^2 + 2(d(o-c))t + (o-c)(o-c)-r^2 = 0 (d*d = 1, d가 정규화 되어있다면)
	// 방정식 계수 a = 1 , b = 2d(o-c), c = (o-c)(o-c)-r^2

	//판별식 계산을 위한 계수
	float b = 2.0f * fProj;
	float c = vDir.Dot(vDir) - (sphere.Radius * sphere.Radius);

	//이차 방정식의 판별식의 값
	float fDiscriminant = (b * b) - (4.0f * c);
	if (fDiscriminant < 0.0f)
	{
		return false;
	}

	//방정식의 해
	fDiscriminant = sqrtf(fDiscriminant);
	float t0 = (-b + fDiscriminant) / 2.0f;
	float t1 = (-b - fDiscriminant) / 2.0f;

	if (t0 >= 0.0f)
	{
		IntersectionDistance = t0;
		Intersection = PickingRay.position + PickingRay.direction * IntersectionDistance;
		return true;
	}
	if (t1 >= 0.0f)
	{
		IntersectionDistance = t1;
		Intersection = PickingRay.position + PickingRay.direction * IntersectionDistance;
		return true;
	}

	return false;
}

bool MousePicker::CheckPick(const Vector3& v0, const Vector3& v1, const Vector3& v2)
{
	if (IntersectTriangle(PickingRay.position, PickingRay.direction, v0, v1, v2, &IntersectionDistance))
	{
		Intersection = PickingRay.position + PickingRay.direction * IntersectionDistance;
		return true;
	}

	return false;
}

bool MousePicker::IntersectTriangle(const Vector3& origin, const Vector3& direction, const Vector3& v0, const Vector3& v1, const Vector3& v2, float* distance)
{
	Vector3 edge1 = v1 - v0;
	Vector3 edge2 = v2 - v0;

	Vector3 pvec = direction.Cross(edge2);
	float det = edge1.Dot(pvec);
	Vector3 tvec;

	if (det > 0)
	{
		tvec = origin - v0;
	}
	else
	{
		tvec = v0 - origin;
		det = -det;
	}

	if (det < 0.0001f)
	{
		return false;
	}

	float u = tvec.Dot(pvec);
	if (u < 0.0f || u > det)
	{
		return false;
	}

	Vector3 qvec = tvec.Cross(edge1);

	float v = direction.Dot(qvec);
	if (v < 0.0f || u + v > det)
	{
		return false;
	}

	*distance = edge2.Dot(qvec);
	float invDet = 1.0f / det;
	*distance *= invDet;
	u *= invDet;
	v *= invDet;

	return true;
}
