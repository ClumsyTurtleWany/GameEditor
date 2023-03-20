#include "Landscape.h"
#include "TransformComponent.h"
#include "DXSamplerState.hpp"

Landscape::Landscape()
{
	this->Name = L"Landscape" + std::to_wstring(this->ID);
	Transform = this->AddComponent<TransformComponent>();
	Components = this->AddComponent<LandscapeComponents>();
	Components->Transform = Transform;
	Components->Initialize();
}

Landscape::~Landscape()
{

}
