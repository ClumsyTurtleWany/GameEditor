#pragma once
#include "Define.h"
#include "BVRender.h"

class BoundingBoxComponent
{
public:
	DirectX::BoundingOrientedBox InitOBB;
	DirectX::BoundingOrientedBox OBB;

#ifdef _DEBUG
	BVRenderObject* pDebugObj;
#endif //_DEBUG

public:
	BoundingBoxComponent() 
	{
#ifdef _DEBUG
		pDebugObj = nullptr;
#endif
	}

	BoundingBoxComponent(Vector3 exts, Vector3 center = {0.0f, 0.0f, 0.0f})
	{
		InitOBB.Center = center;
		InitOBB.Extents = exts;
		InitOBB.Orientation = Quaternion(Vector3(0.0f, 0.0f, 0.0f), 1.0f);

#ifdef _DEBUG
		pDebugObj = new BVRenderObject;
		BV_MGR.add(pDebugObj);
		pDebugObj->init(DXDevice::g_pd3dDevice, DXDevice::g_pImmediateContext, InitOBB);
#endif //_DEBUG
	}

	BoundingBoxComponent(Vector3 center, Vector3 exts, Quaternion rotation)
	{
		InitOBB.Center = center;
		InitOBB.Extents = exts;
		InitOBB.Orientation = rotation;

#ifdef _DEBUG
		pDebugObj = new BVRenderObject;
		BV_MGR.add(pDebugObj);
		pDebugObj->init(DXDevice::g_pd3dDevice, DXDevice::g_pImmediateContext, InitOBB);
#endif //_DEBUG
	}

	~BoundingBoxComponent()
	{
	}

public:

	COLLISION_STATE HitTest(BoundingBoxComponent& OBB)
	{
		bool ret = this->OBB.Intersects(OBB.OBB);

		if (ret) { return CSTATE_OVERLAPPED; }
		
		return CSTATE_NOHIT;
	}

	COLLISION_STATE HitTest(BoundingSphereComponent& Sphere)
	{
		bool ret = this->OBB.Intersects(Sphere.Sphere);

		if (ret) { return CSTATE_OVERLAPPED; }

		return CSTATE_NOHIT;
	}

	bool WriteXML(TiXmlElement* parent)
	{
		if (parent == nullptr)
		{
			return false;
		}

		TiXmlElement* pBoundingBox = new TiXmlElement("BoundingBoxComponent");
		parent->LinkEndChild(pBoundingBox);

		if (!WriteVector3(pBoundingBox, InitOBB.Center, "Center"))
		{

		}

		if (!WriteVector3(pBoundingBox, InitOBB.Extents, "Extents"))
		{

		}

		if (!WriteVector4(pBoundingBox, InitOBB.Orientation, "Orientation"))
		{

		}

		return true;
	}

	bool ReadXML(TiXmlElement* parent)
	{
		if (parent == nullptr)
		{
			return false;
		}

		
		Vector3 center;
		if (!ReadVector3(parent, "Center", center))
		{
			OutputDebugString(L"EditorECS::BoundingBoxComponent::ReadXML::Failed Get Center.");
			return false;
		}
		InitOBB.Center = center;

		Vector3 extents;
		if (!ReadVector3(parent, "Extents", extents))
		{
			OutputDebugString(L"EditorECS::BoundingBoxComponent::ReadXML::Failed Get Extents.");
			return false;
		}
		InitOBB.Extents = extents;

		Vector4 orientation;
		if (!ReadVector4(parent, "Orientation", orientation))
		{
			OutputDebugString(L"EditorECS::BoundingBoxComponent::ReadXML::Failed Get Orientation.");
			return false;
		}
		InitOBB.Orientation = orientation;

		return true;
	}
};
