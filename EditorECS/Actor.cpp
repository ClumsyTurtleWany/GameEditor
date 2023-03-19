#include "Actor.h"

Actor::Actor()
{
	this->Name = L"Actor" + std::to_wstring(this->ID);
	this->AddComponent<TransformComponent>();
}

Actor::~Actor()
{

}