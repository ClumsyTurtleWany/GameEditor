#include "MovementComponent.h"

MovementComponent::MovementComponent() 
	: Location(Vector3(0.0f, 0.0f, 0.0f)), Forward(Vector3(0.0f, 0.0f, 0.0f)), Destination(Vector3(0.0f, 0.0f, 0.0f)), Speed(0.0f) 
{
};

MovementComponent::MovementComponent(Vector3 location, Vector3 forword, Vector3 destination, float speed)
	: Location(location), Forward(forword), Destination(destination), Speed(speed) 
{
};

MovementComponent::~MovementComponent()
{

}
