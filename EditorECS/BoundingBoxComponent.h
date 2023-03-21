#pragma once
#include "Define.h"
#include "BVRender.h"

class BoundingBoxComponent
{
public:
	DirectX::BoundingOrientedBox InitOBB;
	DirectX::BoundingOrientedBox OBB;

#ifdef _DEBUG
	BVRenderObject DebugObj;
#endif //_DEBUG

public:
	BoundingBoxComponent() {}

	BoundingBoxComponent(Vector3 exts, Vector3 center = {0.0f, 0.0f, 0.0f})
	{
		InitOBB.Center = center;
		InitOBB.Extents = exts;
		InitOBB.Orientation = Quaternion(Vector3(0.0f, 0.0f, 0.0f), 1.0f);

#ifdef _DEBUG
		DebugObj.init(DXDevice::g_pd3dDevice, DXDevice::g_pImmediateContext, InitOBB);
#endif //_DEBUG
	}

	BoundingBoxComponent(Vector3 center, Vector3 exts, Quaternion rotation)
	{
		InitOBB.Center = center;
		InitOBB.Extents = exts;
		InitOBB.Orientation = rotation;

#ifdef _DEBUG
		DebugObj.init(DXDevice::g_pd3dDevice, DXDevice::g_pImmediateContext, InitOBB);
#endif //_DEBUG
	}

	~BoundingBoxComponent() {}
};
