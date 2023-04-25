#include "Character.h"

Character::Character()
{
	this->Name = L"Character"; // +std::to_wstring(this->ID);
	MovementComp = this->AddComponent<MovementComponent>();
	Arrow = this->AddComponent<ArrowComponent>();
}

Character::~Character()
{
}

bool Character::MoveTo(Vector3 destination)
{
	MovementComp->Destination = destination;
	Arrow->Forward = destination - Transform->Translation;
	Arrow->Forward.Normalize();

	Arrow->Right = (Arrow->Up.Cross(Arrow->Forward));
	Arrow->Right.Normalize();

	return true;
}
