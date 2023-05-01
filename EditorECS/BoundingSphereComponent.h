#pragma once
#include "Define.h"
#include "BVRender.h"

class BoundingSphereComponent
{
public:
	DirectX::BoundingSphere InitSphere;
	DirectX::BoundingSphere Sphere;

#ifdef _DEBUG
	BVRenderObject* pDebugObj;
#endif //_DEBUG

public:
	BoundingSphereComponent() {}

	BoundingSphereComponent(float radius, Vector3 center = {0.0f, 0.0f, 0.0f})
	{
		InitSphere.Center = center;
		InitSphere.Radius = radius;

#ifdef _DEBUG
		pDebugObj = new BVRenderObject;
		BV_MGR.add(pDebugObj);
		pDebugObj->init(DXDevice::g_pd3dDevice, DXDevice::g_pImmediateContext, InitSphere);
#endif //_DEBUG
	}

	BoundingSphereComponent(DirectX::BoundingOrientedBox& OBB)
	{
		DirectX::BoundingSphere::CreateFromBoundingBox(InitSphere, OBB);

#ifdef _DEBUG
		pDebugObj = new BVRenderObject;
		BV_MGR.add(pDebugObj);
		pDebugObj->init(DXDevice::g_pd3dDevice, DXDevice::g_pImmediateContext, InitSphere);
#endif //_DEBUG
	}

	~BoundingSphereComponent() 
	{
	}

	/*COLLISION_STATE HitTest(BoundingBoxComponent& OBB)
	{
		bool ret = this->Sphere.Intersects(OBB.OBB);

		if (ret) { return CSTATE_OVERLAPPED; }

		return CSTATE_NOHIT;
	}*/

	COLLISION_STATE HitTest(BoundingSphereComponent& Sphere)
	{
		bool ret = this->Sphere.Intersects(Sphere.Sphere);

		if (ret) { return CSTATE_OVERLAPPED; }

		return CSTATE_NOHIT;
	}
};

