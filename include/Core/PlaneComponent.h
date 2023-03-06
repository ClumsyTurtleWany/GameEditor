#pragma once
#include "MeshComponent.hpp"

class PlaneComponent : public MeshComponent
{
public:
	PlaneComponent();
	~PlaneComponent();

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