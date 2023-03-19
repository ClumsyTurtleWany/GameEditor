#include "Character.h"

Character::Character()
{
	this->Name = L"Character" + std::to_wstring(this->ID);
	MovementComp = this->AddComponent<MovementComponent>();
}

Character::~Character()
{
}

bool Character::MoveTo(Vector3 destination)
{
	MovementComp->Destination = destination;
	return true;
}
