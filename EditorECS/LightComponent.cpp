#include "LightComponent.h"

DirectionalLightComponent::DirectionalLightComponent()
{
	
}

DirectionalLightComponent::~DirectionalLightComponent()
{
}

bool DirectionalLightComponent::WriteXML(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	TiXmlElement* pLight = new TiXmlElement("DirectionalLightComponent");
	parent->LinkEndChild(pLight);

	if (!WriteVector4(pLight, Color, "Color"))
	{

	}

	if (!WriteVector4(pLight, Direction, "Direction"))
	{

	}

	return true;
}

bool DirectionalLightComponent::ReadXML(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	TiXmlElement* root = parent->FirstChildElement("DirectionalLightComponent");
	TiXmlElement* pColor = root->FirstChildElement("Color");
	if (pColor == nullptr)
	{
		return false;
	}

	TiXmlAttribute* pAttribute = pColor->FirstAttribute();
	if (!ReadVector4(pAttribute, Color))
	{
		OutputDebugString(L"EditorECS::DirectionalLightComponent::ReadXML::Failed Get Color.");
		return false;
	}

	TiXmlElement* pDirection = root->FirstChildElement("Direction");
	if (pDirection == nullptr)
	{
		return false;
	}

	pAttribute = pDirection->FirstAttribute();
	if (!ReadVector4(pAttribute, Direction))
	{
		OutputDebugString(L"EditorECS::DirectionalLightComponent::ReadXML::Failed Get Direction.");
		return false;
	}

	return true;
}

PointLightComponent::PointLightComponent()
{
	
}

PointLightComponent::~PointLightComponent()
{
}

bool PointLightComponent::WriteXML(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	TiXmlElement* pLight = new TiXmlElement("PointLightComponent");
	parent->LinkEndChild(pLight);

	if (!WriteVector4(pLight, Color, "Color"))
	{

	}

	if (!WriteVector4(pLight, Direction, "Direction"))
	{

	}

	if (!WriteDouble(pLight, Radius, "Radius"))
	{

	}

	return true;
}

bool PointLightComponent::ReadXML(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	TiXmlElement* root = parent->FirstChildElement("PointLightComponent");
	TiXmlElement* pColor = root->FirstChildElement("Color");
	if (pColor == nullptr)
	{
		return false;
	}

	TiXmlAttribute* pAttribute = pColor->FirstAttribute();
	if (!ReadVector4(pAttribute, Color))
	{
		OutputDebugString(L"EditorECS::PointLightComponent::ReadXML::Failed Get Color.");
		return false;
	}

	TiXmlElement* pDirection = root->FirstChildElement("Direction");
	if (pDirection == nullptr)
	{
		return false;
	}

	pAttribute = pDirection->FirstAttribute();
	if (!ReadVector4(pAttribute, Direction))
	{
		OutputDebugString(L"EditorECS::PointLightComponent::ReadXML::Failed Get Direction.");
		return false;
	}

	TiXmlElement* pRadius = root->FirstChildElement("Radius");
	if (pRadius == nullptr)
	{
		return false;
	}

	pAttribute = pRadius->FirstAttribute();
	if (!ReadDouble(pAttribute, Radius))
	{
		OutputDebugString(L"EditorECS::PointLightComponent::ReadXML::Failed Get Radius.");
		return false;
	}

	return true;
}

SpotLightComponent::SpotLightComponent()
{
	
}

SpotLightComponent::~SpotLightComponent()
{
}

bool SpotLightComponent::WriteXML(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	TiXmlElement* pLight = new TiXmlElement("SpotLightComponent");
	parent->LinkEndChild(pLight);

	if (!WriteVector4(pLight, Color, "Color"))
	{

	}

	if (!WriteVector4(pLight, Direction, "Direction"))
	{

	}

	if (!WriteDouble(pLight, Radius, "Radius"))
	{

	}

	if (!WriteDouble(pLight, Distance, "Distance"))
	{

	}

	return true;
}

bool SpotLightComponent::ReadXML(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	TiXmlElement* root = parent->FirstChildElement("SpotLightComponent");
	TiXmlElement* pColor = root->FirstChildElement("Color");
	if (pColor == nullptr)
	{
		return false;
	}

	TiXmlAttribute* pAttribute = pColor->FirstAttribute();
	if (!ReadVector4(pAttribute, Color))
	{
		OutputDebugString(L"EditorECS::SpotLightComponent::ReadXML::Failed Get Color.");
		return false;
	}

	TiXmlElement* pDirection = root->FirstChildElement("Direction");
	if (pDirection == nullptr)
	{
		return false;
	}

	pAttribute = pDirection->FirstAttribute();
	if (!ReadVector4(pAttribute, Direction))
	{
		OutputDebugString(L"EditorECS::SpotLightComponent::ReadXML::Failed Get Direction.");
		return false;
	}

	TiXmlElement* pRadius = root->FirstChildElement("Radius");
	if (pRadius == nullptr)
	{
		return false;
	}

	pAttribute = pRadius->FirstAttribute();
	if (!ReadDouble(pAttribute, Radius))
	{
		OutputDebugString(L"EditorECS::SpotLightComponent::ReadXML::Failed Get Radius.");
		return false;
	}

	TiXmlElement* pDistance = root->FirstChildElement("Distance");
	if (pDistance == nullptr)
	{
		return false;
	}

	pAttribute = pDistance->FirstAttribute();
	if (!ReadDouble(pAttribute, Distance))
	{
		OutputDebugString(L"EditorECS::SpotLightComponent::ReadXML::Failed Get Distance.");
		return false;
	}

	return true;
}
