#include "TransformComponent.h"

TransformComponent::TransformComponent() 
	: Translation(Vector3(0.0, 0.0, 0.0)), Rotation(Vector3(0.0, 0.0, 0.0)), Scale(Vector3(1.0, 1.0, 1.0))
{

};
TransformComponent::TransformComponent(Vector3 translation, Vector3 rotation, Vector3 scale)
	: Translation(translation), Rotation(rotation), Scale(scale)
{

}

bool TransformComponent::WriteXML(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	TiXmlElement* pTransform = new TiXmlElement("Transform");
	parent->LinkEndChild(pTransform);
	
	if (!WriteVector3(pTransform, Translation, "Translation"))
	{

	}

	if (!WriteVector3(pTransform, Rotation, "Rotation"))
	{

	}

	if (!WriteVector3(pTransform, Scale, "Scale"))
	{

	}

	return true;
}

bool TransformComponent::ReadXML(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	TiXmlElement* root = parent->FirstChildElement("Transform");

	TiXmlElement* pTranslation = root->FirstChildElement("Translation");
	if (pTranslation == nullptr)
	{
		return false;
	}

	TiXmlAttribute* pAttribute = pTranslation->FirstAttribute();
	if (!ReadVector3(pAttribute, Translation))
	{
		OutputDebugString(L"EditorECS::TransformComponent::ReadXML::Failed Get Translation.");
		return false;
	}
	
	TiXmlElement* pRotation = root->FirstChildElement("Rotation");
	if (pRotation == nullptr)
	{
		return false;
	}

	pAttribute = pRotation->FirstAttribute();
	if (!ReadVector3(pAttribute, Rotation))
	{
		OutputDebugString(L"EditorECS::TransformComponent::ReadXML::Failed Get Translation.");
		return false;
	}

	TiXmlElement* pScale = root->FirstChildElement("Scale");
	if (pScale == nullptr)
	{
		return false;
	}

	pAttribute = pScale->FirstAttribute();
	if (!ReadVector3(pAttribute, Scale))
	{
		OutputDebugString(L"EditorECS::TransformComponent::ReadXML::Failed Get Scale.");
		return false;
	}

	return true;
}