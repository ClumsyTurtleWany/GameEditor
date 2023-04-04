#include "MousePicker.h"

MousePicker::MousePicker()
{
	optPickingMode = PMOD_CHARACTER;

	ZeroMemory(&lastSelect, sizeof(SelectState));
	ZeroMemory(&curSelect, sizeof(SelectState));

	bClicked = false;
	dwPickingButton = VK_LBUTTON;

	pMainInput = Input::GetInstance();

	bOneTimeInputBlock = false;
	bPendingClicked = false;
	fLandTraceTimer = 0.0f;
}

MousePicker::~MousePicker()
{
}

void MousePicker::Update()
{
	fIntersectionDistance = MAX_PICK_DIST;

	ptCursor = pMainInput->m_ptPos;

	ClientWidth = DXDevice::g_ViewPort.Width;
	ClientHeight = DXDevice::g_ViewPort.Height;

	if (bOneTimeInputBlock)
	{
		bOneTimeInputBlock = false;
	}
	else
	{
		if (pMainInput->getKey(dwPickingButton) == KeyState::Up)
		{
			bClicked = true;

			if (optPickingMode == PMOD_LAND)
			{
				bPendingClicked = true;
			}
		}
		else { bClicked = false; }
	}

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

void MousePicker::ClearSelectState()
{
	ZeroMemory(&lastSelect, sizeof(SelectState));
}

void MousePicker::setMatrix(Matrix* pWorld, Matrix* pView, Matrix* pProj)
{
	if (pWorld) { World = *pWorld; }
	if (pView) { View = *pView; }
	if (pProj) { Projection = *pProj; }
}

bool MousePicker::RayToOBB(const DirectX::BoundingOrientedBox& OBB)
{
	//�ܼ� �浹 üũ
	//�и��� �̷��� �̿��Ѵ�.
	//OBB�� ������ ������ ������ ���� ����(R = O - C)�� �̿��� 
	//������ ���⺤�Ϳ� OBB�� �� �𼭸��� ������ ���� ���� ���� ������ �����ϰ�
	//OBB�� ���� ���� ���� ������ �������� ������ ���⼺ �˻�� ����Ѵ�.

	//1. OBB�� ���� ���� �� �˻� : �������� ������ ���⼺ �˻縦 �Բ� �����Ѵ�.
	//���� �˻�� OBB�� �������� ���̿� R�� OBB�� ��� ������ ���� ���̸� ���Ѵ�.
	//(OBB�� �������� ����) < abs(DOT(R, OBB�� ��)) => �ܺ�
	//���� �˻�� ������ ���⺤�Ϳ� R���� ������ ���� ���� ��ȣ�� �����Ѵ�.
	// R�� ������ ���Ϳ��� ������ �����̹Ƿ� �� ������ ����� ����� �ڽ��� ��� ��������
	// ���̰� �����Ѵٴ� �ǹ̰��ȴ�.
	//DOT(PickingRay.direction, OBB�� ��) * DOT(R, OBB�� ��) >= 0.0f => �ܺ�

	//����ȭ ���� : OD => Origin Dot / DD => Direction Dot

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

	//2. ������ ���⺤�Ϳ� �𼭸��� �������� �� ������ �˻�
	//��Į�� �������� Ư���� �̿��� ���� ������ 
	//������ �� �ึ�� �����ϴ� ���� �ƴ� ������ ���⺤�Ϳ� centerToOrigin���� �ѹ��� ������
	//�ݺ��Ͽ� ����Ѵ�.
	//OBB�� �����ε� OBB�� ������ ������ �ٸ� OBB�� ���� �ȴٴ� ����
	//��Į�� �������� Ư��, ������ ����� ������ Ư���� �̿��� ������ �����Ѵ�.
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

	//�������� ���ϴ� �κ�
	OBB.Intersects(PickingRay.position, PickingRay.direction, fIntersectionDistance);

	vIntersection = PickingRay.position + (fIntersectionDistance * PickingRay.direction);

	return true;
}

bool MousePicker::RayToAABB(const DirectX::BoundingBox& AABB)
{
	//�������� ���ϴ� �κ�
	if (AABB.Intersects(PickingRay.position, PickingRay.direction, fIntersectionDistance))
	{
		vIntersection = PickingRay.position + (fIntersectionDistance * PickingRay.direction);
		return true;
	}

	return false;
}

bool MousePicker::RayToSphere(const DirectX::BoundingSphere& sphere)
{
	float rayDistLimit = MAX_PICK_DIST;

	// (o-c)
	Vector3 vDir = PickingRay.position - sphere.Center;
	// ���̿��� ���� �񱳸� ���� ���� : d(o-c)
	float  fProj = PickingRay.direction.Dot(vDir);
	// ���� ���� ó��
	if (rayDistLimit < fabs(fProj)) return false;

	//���� ���� 2�� ������
	//f(p) = |p-c|^2 = (p.x - c.x)^2 + (p.y - c.y)^2 + (p.z - c.z)^2 = r^2
	//������ ����ٸ� p�� Ray���� ���� �Ǳ� ������ 
	//ray = r(t) = o + td (o : origin, d : direction)��� �ϸ� 2�� �������� ������ ���� ��Ÿ�� �� �ִ�.
	//f(p) = |r(t)-c|^2 = |o + td - c|^2
	// => t^2(d*d) + 2td(o-c) + (o-c)(o-c)-r^2 = 0
	// t^2 + 2(d(o-c))t + (o-c)(o-c)-r^2 = 0 (d*d = 1, d�� ����ȭ �Ǿ��ִٸ�)
	// ������ ��� a = 1 , b = 2d(o-c), c = (o-c)(o-c)-r^2

	//�Ǻ��� ����� ���� ���
	float b = 2.0f * fProj;
	float c = vDir.Dot(vDir) - (sphere.Radius * sphere.Radius);

	//���� �������� �Ǻ����� ��
	float fDiscriminant = (b * b) - (4.0f * c);
	if (fDiscriminant < 0.0f)
	{
		return false;
	}

	//�������� ��
	fDiscriminant = sqrtf(fDiscriminant);
	float t0 = (-b + fDiscriminant) / 2.0f;
	float t1 = (-b - fDiscriminant) / 2.0f;

	if (t0 >= 0.0f)
	{
		fIntersectionDistance = t0;
		vIntersection = PickingRay.position + PickingRay.direction * fIntersectionDistance;
		return true;
	}
	if (t1 >= 0.0f)
	{
		fIntersectionDistance = t1;
		vIntersection = PickingRay.position + PickingRay.direction * fIntersectionDistance;
		return true;
	}

	return false;
}

bool MousePicker::CheckPick(const Vector3& v0, const Vector3& v1, const Vector3& v2)
{
	if (IntersectTriangle(PickingRay.position, PickingRay.direction, v0, v1, v2, &fIntersectionDistance))
	{
		vIntersection = PickingRay.position + PickingRay.direction * fIntersectionDistance;
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

void MousePicker::SetPickingMode(PICKING_MODE PickMode)
{
	if (optPickingMode != PickMode)
	{
		if (0 <= PickMode && PickMode < NUMBER_OF_PICKING_MODE)
		{
			optPickingMode = PickMode;
			bOneTimeInputBlock = true;
			bClicked = false;
			bPendingClicked = false;
		}
	}
}
