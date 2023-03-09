#pragma once
#include "MeshComponent.hpp"

class CubeComponent : public MeshComponent
{
public:
	CubeComponent();
	~CubeComponent();

};

inline CubeComponent::CubeComponent()
{
	Vertices.resize(8);
	Vertices[0].Pos = Vector3(-0.5f, +0.5f, -0.5f);
	Vertices[1].Pos = Vector3(+0.5f, +0.5f, -0.5f);
	Vertices[2].Pos = Vector3(-0.5f, -0.5f, -0.5f);
	Vertices[3].Pos = Vector3(+0.5f, -0.5f, -0.5f);

	Vertices[4].Pos = Vector3(+0.5f, +0.5f, +0.5f);
	Vertices[5].Pos = Vector3(-0.5f, +0.5f, +0.5f);
	Vertices[6].Pos = Vector3(+0.5f, -0.5f, +0.5f);
	Vertices[7].Pos = Vector3(-0.5f, -0.5f, +0.5f);

	Vertices[0].Texture = Vector2(0.0f, 0.0f);
	Vertices[1].Texture = Vector2(1.0f, 0.0f);
	Vertices[2].Texture = Vector2(0.0f, 1.0f);
	Vertices[3].Texture = Vector2(1.0f, 1.0f);

	Vertices[4].Texture = Vector2(0.0f, 0.0f);
	Vertices[5].Texture = Vector2(1.0f, 0.0f);
	Vertices[6].Texture = Vector2(0.0f, 1.0f);
	Vertices[7].Texture = Vector2(1.0f, 1.0f);

	Vertices[0].Color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Vertices[1].Color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Vertices[2].Color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Vertices[3].Color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	Vertices[4].Color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Vertices[5].Color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Vertices[6].Color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Vertices[7].Color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	Vertices[0].Normal = Vector3(-1.0f, +1.0f, -1.0f); Vertices[0].Normal.Normalize();
	Vertices[1].Normal = Vector3(+1.0f, +1.0f, -1.0f); Vertices[1].Normal.Normalize();
	Vertices[2].Normal = Vector3(-1.0f, -1.0f, -1.0f); Vertices[2].Normal.Normalize();
	Vertices[3].Normal = Vector3(+1.0f, -1.0f, -1.0f); Vertices[3].Normal.Normalize();

	Vertices[4].Normal = Vector3(+1.0f, +1.0f, +1.0f); Vertices[4].Normal.Normalize();
	Vertices[5].Normal = Vector3(-1.0f, +1.0f, +1.0f); Vertices[5].Normal.Normalize();
	Vertices[6].Normal = Vector3(+1.0f, -1.0f, +1.0f); Vertices[6].Normal.Normalize();
	Vertices[7].Normal = Vector3(-1.0f, -1.0f, +1.0f); Vertices[7].Normal.Normalize();

	Indices.resize(36);
	// Front
	Indices[0] = 0;
	Indices[1] = 1;
	Indices[2] = 2;
	Indices[3] = 2;
	Indices[4] = 1;
	Indices[5] = 3;

	// Back
	Indices[6] = 4;
	Indices[7] = 5;
	Indices[8] = 6;
	Indices[9] = 6;
	Indices[10] = 5;
	Indices[11] = 7;

	// Left
	Indices[12] = 5;
	Indices[13] = 0;
	Indices[14] = 7;
	Indices[15] = 7;
	Indices[16] = 0;
	Indices[17] = 2;

	// Right
	Indices[18] = 1;
	Indices[19] = 4;
	Indices[20] = 3;
	Indices[21] = 3;
	Indices[22] = 4;
	Indices[23] = 6;

	// Upper
	Indices[24] = 5;
	Indices[25] = 4;
	Indices[26] = 0;
	Indices[27] = 0;
	Indices[28] = 4;
	Indices[29] = 1;

	// Lower
	Indices[30] = 2;
	Indices[31] = 3;
	Indices[32] = 7;
	Indices[33] = 7;
	Indices[34] = 3;
	Indices[35] = 6;

}

inline CubeComponent::~CubeComponent()
{

}