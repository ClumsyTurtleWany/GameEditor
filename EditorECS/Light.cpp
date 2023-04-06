#include "Light.h"


Light::Light()
{
	this->AddComponent<TransformComponent>();
	this->Name = L"Light" + std::to_wstring(this->ID);
}

Light::~Light()
{

}