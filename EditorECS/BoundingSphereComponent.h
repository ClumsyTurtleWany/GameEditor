#pragma once
#include "Define.h"

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
};

