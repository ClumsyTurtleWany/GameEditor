#pragma once
#include "Define.h"
#include "Mesh.h"

class StaticMesh : public Mesh
{
public:
	ID3D11Buffer*	VertexBuffer = nullptr;
	ID3D11Buffer*	IndexBuffer = nullptr;

public:
	StaticMesh();
	virtual ~StaticMesh();

public:
	virtual bool Initialize() override;
	virtual bool Render() override;
	virtual bool RenderShadow() override;
};