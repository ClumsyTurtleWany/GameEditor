#include "MovementComponent.h"

MovementComponent::MovementComponent() 
	: Location(Vector3(0.0f, 0.0f, 0.0f)), Forward(Vector3(0.0f, 0.0f, 0.0f)), Destination(Vector3(0.0f, 0.0f, 0.0f)), Speed(0.0f) 
{
};

MovementComponent::MovementComponent(Vector3 location, Vector3 forword, Vector3 destination, float speed)
	: Location(location), Forward(forword), Destination(destination), Speed(speed) 
{
};

MovementComponent::MovementComponent(Vector3 location, Vector3 destination, Vector3 initVelo, Vector3 initAccel)
	: Location(location), Destination(destination), InitVelocity(initVelo), InitAcceleration(initAccel)
{
	Speed = 0.0f;
	IsMoving = true;
}

MovementComponent::~MovementComponent()
{
	
}

bool MovementComponent::WriteXML(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	TiXmlElement* pMovementComponent = new TiXmlElement("MovementComponent");
	parent->LinkEndChild(pMovementComponent);

	if (!WriteVector3(pMovementComponent, Location, "Location"))
	{

	}

	if (!WriteVector3(pMovementComponent, Forward, "Forward"))
	{

	}

	if (!WriteVector3(pMovementComponent, Destination, "Destination"))
	{

	}

	if (!WriteDouble(pMovementComponent, Speed, "Speed"))
	{

	}

	return true;
}

bool MovementComponent::ReadXML(TiXmlElement* parent)
{
	if (parent == nullptr)
	{
		return false;
	}

	TiXmlElement* root = parent->FirstChildElement("MovementComponent");

	TiXmlElement* pLocation = root->FirstChildElement("Location");
	if (pLocation == nullptr)
	{
		return false;
	}

	TiXmlAttribute* pAttribute = pLocation->FirstAttribute();
	if (!ReadVector3(pAttribute, Location))
	{
		OutputDebugString(L"EditorECS::MovementComponent::ReadXML::Failed Get Location.");
		return false;
	}

	TiXmlElement* pForward = root->FirstChildElement("Forward");
	if (pForward == nullptr)
	{
		return false;
	}

	pAttribute = pForward->FirstAttribute();
	if (!ReadVector3(pAttribute, Forward))
	{
		OutputDebugString(L"EditorECS::MovementComponent::ReadXML::Failed Get Forward.");
		return false;
	}

	TiXmlElement* pDestination = root->FirstChildElement("Destination");
	if (pDestination == nullptr)
	{
		return false;
	}

	pAttribute = pDestination->FirstAttribute();
	if (!ReadVector3(pAttribute, Destination))
	{
		OutputDebugString(L"EditorECS::MovementComponent::ReadXML::Failed Get Destination.");
		return false;
	}

	TiXmlElement* pSpeed = root->FirstChildElement("Speed");
	if (pSpeed == nullptr)
	{
		return false;
	}

	pAttribute = pSpeed->FirstAttribute();
	if (!ReadDouble(pAttribute, Speed))
	{
		OutputDebugString(L"EditorECS::MovementComponent::ReadXML::Failed Get Speed.");
		return false;
	}

	return true;
}
