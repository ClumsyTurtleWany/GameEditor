#pragma once
#include "MeshComponent.hpp"

class PlaneComponent : public MeshComponent
{
public:
	PlaneComponent();
	~PlaneComponent();

public:
	bool CreateTangentSpaceVectors(Vertex v0, Vertex v1, Vertex v2, Vector3& tangent);
	bool CalcTangent();
};

inline PlaneComponent::PlaneComponent()
{
	Vertices.resize(4);

	Vertices[0].Pos = { -1.0f, +1.0f, 0.0f }; // p1-LT
	Vertices[1].Pos = { +1.0f, +1.0f, 0.0f }; // p2-RT
	Vertices[2].Pos = { -1.0f, -1.0f, 0.0f }; // p3-LB
	Vertices[3].Pos = { +1.0f, -1.0f, 0.0f }; // p4-LB

	Vertices[0].Normal = { 0.0f, 0.0f, -1.0f };
	Vertices[1].Normal = { 0.0f, 0.0f, -1.0f };
	Vertices[2].Normal = { 0.0f, 0.0f, -1.0f };
	Vertices[3].Normal = { 0.0f, 0.0f, -1.0f };

	Vertices[0].Color = { 1.0f, 1.0f, 1.0f, 1.0f }; // p1-LT
	Vertices[1].Color = { 1.0f, 1.0f, 1.0f, 1.0f }; // p2-RT
	Vertices[2].Color = { 1.0f, 1.0f, 1.0f, 1.0f }; // p3-LB
	Vertices[3].Color = { 1.0f, 1.0f, 1.0f, 1.0f }; // p4-LB

	Vertices[0].Texture = { 0.0f, 0.0f }; // p1-LT
	Vertices[1].Texture = { 1.0f, 0.0f }; // p2-RT
	Vertices[2].Texture = { 0.0f, 1.0f }; // p3-LB
	Vertices[3].Texture = { 1.0f, 1.0f }; // p4-LB

	Indices.resize(6);
	Indices[0] = 0;
	Indices[1] = 1;
	Indices[2] = 2;
	Indices[3] = 2;
	Indices[4] = 1;
	Indices[5] = 3;
}

inline PlaneComponent::~PlaneComponent()
{

}

bool PlaneComponent::CreateTangentSpaceVectors(Vertex v0, Vertex v1, Vertex v2, Vector3& tangent)
{
	Vector3 edge1 = v1.Pos - v0.Pos;
	Vector3 edge2 = v2.Pos - v0.Pos;

	Vector2 edge1uv = v1.Texture - v0.Texture;
	Vector2 edge2uv = v2.Texture - v0.Texture;

	float denominator = edge1uv.x * edge2uv.y - edge2uv.x * edge1uv.y;
	if (denominator < 0.0001f && denominator > -0.0001f)
	{
		tangent = Vector3(1.0f, 0.0f, 0.0f);
	}
	else
	{
		float scale1 = 1.0f;
		Vector3 T;
		Vector3 B;
		Vector3 N;

		T = Vector3((edge2uv.y * edge1.x - edge1uv.y * edge2.x) * scale1,
			(edge2uv.y * edge1.y - edge1uv.y * edge2.y) * scale1,
			(edge2uv.y * edge1.z - edge1uv.y * edge2.z) * scale1);

		B = Vector3((-edge2uv.x * edge1.x + edge1uv.x * edge2.x) * scale1,
			(-edge2uv.x * edge1.y + edge1uv.x * edge2.y) * scale1,
			(-edge2uv.x * edge1.z + edge1uv.x * edge2.z) * scale1);

		N = T.Cross(B);

		float scale2 = 1.0f /
			((T.x * B.y * N.z - T.z * B.y * N.x) +
				(B.x * N.y * T.z - B.z * N.y * T.x) +
				(N.x * T.y * B.z - N.z * T.y * B.x));

		tangent.x = (B.Cross(N).x * scale2);
		tangent.y = -(N.Cross(T).x * scale2);
		tangent.z = (T.Cross(B).x * scale2);
		tangent.Normalize();

		Vector3 biNormal;
		biNormal.x = -(B.Cross(N).y * scale2);
		biNormal.y = (N.Cross(T).y * scale2);
		biNormal.z = (T.Cross(B).y * scale2);
		biNormal.Normalize();

		Vector3 normal;
		normal.x = (B.Cross(N).z * scale2);
		normal.y = -(N.Cross(T).z * scale2);
		normal.z = (T.Cross(B).z * scale2);
		normal.Normalize();

		int a = 0;
	}

	return true;
}

bool PlaneComponent::CalcTangent()
{
	int index = 0;
	int i0, i1, i2, i3, i4, i5;
	for (size_t idx = 0; idx < Vertices.size(); idx += 4)
	{
		i0 = Indices[index + 0];
		i1 = Indices[index + 1];
		i2 = Indices[index + 2];
		i3 = Indices[index + 3];
		i4 = Indices[index + 4];
		i5 = Indices[index + 5];

		Vector3 tangent;
		CreateTangentSpaceVectors(Vertices[i0], Vertices[i1], Vertices[i2], tangent);
		Vertices[idx + 0].Tangent = tangent;

		CreateTangentSpaceVectors(Vertices[i1], Vertices[i2], Vertices[i0], tangent);
		Vertices[idx + 1].Tangent = tangent;

		CreateTangentSpaceVectors(Vertices[i2], Vertices[i0], Vertices[i1], tangent);
		Vertices[idx + 2].Tangent = tangent;

		CreateTangentSpaceVectors(Vertices[i5], Vertices[i3], Vertices[i4], tangent);
		Vertices[idx + 3].Tangent = tangent;

		index += 6;
	}

	return true;
}