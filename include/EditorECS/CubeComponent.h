#pragma once
#include "Mesh.h"

class CubeComponent : public StaticMesh
{
public:
	CubeComponent();
	~CubeComponent();

};

inline CubeComponent::CubeComponent()
{
	// Front
	Vertices[0] = Vertex(Vector3(-0.5f, +0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f));
	Vertices[1] = Vertex(Vector3(+0.5f, +0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f));
	Vertices[2] = Vertex(Vector3(-0.5f, -0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f));
	Vertices[3] = Vertex(Vector3(+0.5f, -0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f));

	// Left
	Vertices[4] = Vertex(Vector3(-0.5f, +0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f));
	Vertices[5] = Vertex(Vector3(-0.5f, +0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f));
	Vertices[6] = Vertex(Vector3(-0.5f, -0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f));
	Vertices[7] = Vertex(Vector3(-0.5f, -0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f));

	// Right
	Vertices[8] = Vertex(Vector3(+0.5f, +0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f));
	Vertices[9] = Vertex(Vector3(+0.5f, +0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f));
	Vertices[10] = Vertex(Vector3(+0.5f, -0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f));
	Vertices[11] = Vertex(Vector3(+0.5f, -0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f));

	// Back
	Vertices[12] = Vertex(Vector3(+0.5f, +0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f));
	Vertices[13] = Vertex(Vector3(-0.5f, +0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f));
	Vertices[14] = Vertex(Vector3(+0.5f, -0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f));
	Vertices[15] = Vertex(Vector3(-0.5f, -0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f));

	// Upper
	Vertices[16] = Vertex(Vector3(-0.5f, +0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f));
	Vertices[17] = Vertex(Vector3(+0.5f, +0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f));
	Vertices[18] = Vertex(Vector3(-0.5f, +0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f));
	Vertices[19] = Vertex(Vector3(+0.5f, +0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f));

	// Lower
	Vertices[20] = Vertex(Vector3(-0.5f, -0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f));
	Vertices[21] = Vertex(Vector3(+0.5f, -0.5f, -0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f));
	Vertices[22] = Vertex(Vector3(-0.5f, -0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f));
	Vertices[23] = Vertex(Vector3(+0.5f, -0.5f, +0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f));


	Indices.resize(36);
	// Front
	Indices[0] = 0;
	Indices[1] = 1;
	Indices[2] = 2;
	Indices[3] = 2;
	Indices[4] = 1;
	Indices[5] = 3;

	// Left
	Indices[6] = 4;
	Indices[7] = 5;
	Indices[8] = 6;
	Indices[9] = 6;
	Indices[10] = 5;
	Indices[11] = 7;

	// Right
	Indices[12] = 8;
	Indices[13] = 9;
	Indices[14] = 10;
	Indices[15] = 10;
	Indices[16] = 9;
	Indices[17] = 11;

	// Back
	Indices[18] = 12;
	Indices[19] = 13;
	Indices[20] = 14;
	Indices[21] = 14;
	Indices[22] = 13;
	Indices[23] = 15;

	// Upper
	Indices[24] = 16;
	Indices[25] = 17;
	Indices[26] = 18;
	Indices[27] = 18;
	Indices[28] = 17;
	Indices[29] = 19;

	// Lower
	Indices[30] = 20;
	Indices[31] = 21;
	Indices[32] = 22;
	Indices[33] = 22;
	Indices[34] = 21;
	Indices[35] = 23;
}

inline CubeComponent::~CubeComponent()
{

}