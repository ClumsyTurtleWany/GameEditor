#include "CameraArmComponent.h"

CameraArmComponent::CameraArmComponent()
{
}

CameraArmComponent::~CameraArmComponent()
{
}

bool CameraArmComponent::WriteXML(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	TiXmlElement* pCameraArmComponent = new TiXmlElement("CameraArmComponent");
	parent->LinkEndChild(pCameraArmComponent);

	if (!WriteDouble(pCameraArmComponent, Distance, "Distance"))
	{

	}

	if (!WriteDouble(pCameraArmComponent, Pitch, "Pitch"))
	{

	}

	if (!WriteDouble(pCameraArmComponent, Yaw, "Yaw"))
	{

	}

	if (!WriteDouble(pCameraArmComponent, Yaw, "Roll"))
	{

	}
}

bool CameraArmComponent::ReadXML(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	TiXmlElement* root = parent->FirstChildElement("CameraArmComponent");

	if (!ReadDouble(root, "Distance", Distance))
	{

	}

	if (!ReadDouble(root, "Pitch", Pitch))
	{

	}

	if (!ReadDouble(root, "Yaw", Yaw))
	{

	}

	if (!ReadDouble(root, "Roll", Roll))
	{

	}

	return true;
}
