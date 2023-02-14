#include "pch.h"
#include "TestClass.h"
#include "StaticMeshComponent.h"
#include "RenderSystem.h"
#include "Timer.hpp"
#include "Actor.h"

TestClass::TestClass(HWND hWnd)
{
	SetWindowHandle(hWnd);
}

bool TestClass::Initialize()
{
	Actor* NewActor = new Actor;
	auto comp = NewActor->AddComponent<StaticMeshComponent>();
	MeshComponent NewMesh;
	NewMesh.Vertices.assign(4, Vertex());
	NewMesh.Vertices[0].Pos = { -1.0f, +1.0f, 0.0f }; // p1-LT
	NewMesh.Vertices[1].Pos = { +1.0f, +1.0f, 0.0f }; // p2-RT
	NewMesh.Vertices[2].Pos = { -1.0f, -1.0f, 0.0f }; // p3-LB
	NewMesh.Vertices[3].Pos = { +1.0f, -1.0f, 0.0f }; // p4-LB

	NewMesh.Vertices[0].Color = { 1.0f, 0.0f, 0.0f, 1.0f }; // p1-LT
	NewMesh.Vertices[1].Color = { 0.0f, 1.0f, 0.0f, 1.0f }; // p2-RT
	NewMesh.Vertices[2].Color = { 0.0f, 0.0f, 1.0f, 1.0f }; // p3-LB
	NewMesh.Vertices[3].Color = { 1.0f, 1.0f, 1.0f, 1.0f }; // p4-LB

	NewMesh.Vertices[0].Texture = { 0.0f, 0.0f }; // p1-LT
	NewMesh.Vertices[1].Texture = { 1.0f, 0.0f }; // p2-RT
	NewMesh.Vertices[2].Texture = { 0.0f, 1.0f }; // p3-LB
	NewMesh.Vertices[3].Texture = { 1.0f, 1.0f }; // p4-LB

	NewMesh.Indices.assign(6, 0);
	NewMesh.Indices[0] = 0;
	NewMesh.Indices[1] = 1;
	NewMesh.Indices[2] = 2;
	NewMesh.Indices[3] = 2;
	NewMesh.Indices[4] = 1;
	NewMesh.Indices[5] = 3;

	comp->Meshes.push_back(NewMesh);

	World.AddSystem(new RenderSystem);
	World.AddEntity(NewActor);

	return false;
}

bool TestClass::Frame()
{
	return false;
}

bool TestClass::Render()
{
	float tickTime = Timer::getInstance()->getDeltaTime();
	World.Tick(tickTime);
	return true;
}

bool TestClass::Release()
{
	return false;
}
