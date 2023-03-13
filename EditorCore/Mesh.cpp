#include "Mesh.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::SetMaterial(Material* material)
{
	MaterialSlot = material;
}
