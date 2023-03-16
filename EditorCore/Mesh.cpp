#include "Mesh.h"
#include <fstream>

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
