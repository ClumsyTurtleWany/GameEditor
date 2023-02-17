#include "pch.h"
#include "TestClass.h"
#include "StaticMeshComponent.h"
#include "RenderSystem.h"
#include "Timer.hpp"
#include "Actor.h"
#include "DebugCamera.h"
#include "DebugCameraSystem.h"
#include "Landscape.h"
#include "FBXLoader.hpp"

TestClass::TestClass(HWND hWnd)
{
	SetWindowHandle(hWnd);
}

bool TestClass::Initialize()
{
	Actor* NewActor = new Actor;
	auto comp = NewActor->AddComponent<StaticMeshComponent>();
	comp->SetContext(Device.m_pImmediateContext);
	MeshComponent NewMesh;
	NewMesh.SetContext(Device.m_pImmediateContext);
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
	DebugCamera* camera = new DebugCamera;
	camera->SetContext(Device.m_pImmediateContext);
	camera->CreateViewMatrix(Vector3(0.0f, 0.0f, -10.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0, 0.0f));
	camera->CreateProjectionMatrix(1.0f, 500.0f, PI * 0.25, (Device.m_ViewPort.Width) / (Device.m_ViewPort.Height));
	World.SetDebugCamera(camera);

	World.AddSystem(new DebugCameraSystem);

	RenderSystem* renderSystem = new RenderSystem;
	renderSystem->SetContext(Device.m_pImmediateContext);
	World.AddSystem(renderSystem);
	World.AddEntity(NewActor);

	/*auto land = NewActor->AddComponent<Landscape>();
	land->SetContext(Device.m_pImmediateContext);
	land->Build(8, 8, 7);
	land->SetCamera(camera);*/

	FBXLoader::getInstance()->initialize();
	/*if (FBXLoader::getInstance()->initialize())
	{
		FBXLoader::getInstance()->setResourceDirectory(L"../resource/FBX/");
		FBXLoader::getInstance()->LoadDir(L"../resource/FBX/");
	}*/

	Picker.ClientWidth = Device.m_ViewPort.Width;
	Picker.ClientHeight = Device.m_ViewPort.Height;

	return true;
}

bool TestClass::Frame()
{
	Camera* camera = World.GetDebugCamera();
	Picker.View = camera->View;
	Picker.Projection = camera->Projection;
	
	Picker.Update();

	if (!SelectedFilename.empty())
	{
		KeyState keyState_RButton = Input::getInstance()->getKey(VK_RBUTTON);
		if (keyState_RButton == KeyState::Hold)
		{
			
			for (auto& it : World.GetEntities<Landscape>())
			{
				auto landscape = it->GetComponent<Landscape>();
				for (auto& comp : landscape->Components)
				{
					for (auto& face : comp.Faces)
					{
						if (Picker.CheckPick(face.V0.Pos, face.V1.Pos, face.V2.Pos))
						{
							auto mesh = StaticMeshMap.find(SelectedFilename);
							if (mesh != StaticMeshMap.end())
							{
								StaticMeshComponent* meshComp = mesh->second.get();
								Actor* NewActor = new Actor;
								auto staticMeshComp = NewActor->AddComponent<StaticMeshComponent>();
								staticMeshComp->Meshes.assign(meshComp->Meshes.begin(), meshComp->Meshes.end());
								for (auto& it : staticMeshComp->Meshes)
								{
									it.SetContext(Device.m_pImmediateContext);
								}
								staticMeshComp->SetContext(Device.m_pImmediateContext);

								auto transform = NewActor->GetComponent<TransformComponent>();
								transform->Translation = Picker.IntercetionPosition;

								World.AddEntity(NewActor);
							}
							else
							{
								std::unique_ptr<StaticMeshComponent> staticMesh = std::make_unique<StaticMeshComponent>();
								if (FBXLoader::getInstance()->Load(L"../resource/FBX/" + SelectedFilename, staticMesh.get()))
								{
									StaticMeshMap.insert(std::make_pair(SelectedFilename, std::move(staticMesh)));
								}

								auto mesh = StaticMeshMap.find(SelectedFilename);
								if (mesh != StaticMeshMap.end())
								{
									StaticMeshComponent* meshComp = mesh->second.get();
									Actor* NewActor = new Actor;
									auto staticMeshComp = NewActor->AddComponent<StaticMeshComponent>();
									staticMeshComp->Meshes.assign(meshComp->Meshes.begin(), meshComp->Meshes.end());
									//staticMeshComp = meshComp;
									for (auto& it : staticMeshComp->Meshes)
									{
										it.SetContext(Device.m_pImmediateContext);
									}
									staticMeshComp->SetContext(Device.m_pImmediateContext);

									auto transform = NewActor->GetComponent<TransformComponent>();
									transform->Translation = Picker.IntercetionPosition;

									World.AddEntity(NewActor);
								}
							}
						}
					}
					
				}
				
			}
		}
	}

	return true;
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

ID3D11Device* TestClass::GetDevice()
{
	return Device.m_pd3dDevice;
}

ID3D11DeviceContext* TestClass::GetContext()
{
	return Device.m_pImmediateContext;
}

DebugCamera* TestClass::GetDebugCamera()
{
	return World.GetDebugCamera();
}

bool TestClass::CreateLandscape(int width, int height, int sectionSize)
{
	if (width <= 0 || height <= 0 || sectionSize <= 0)
	{
		return false;
	}

	Actor* LandscapeActor = new Actor;
	auto land = LandscapeActor->AddComponent<Landscape>();
	land->SetContext(Device.m_pImmediateContext);
	land->Build(width, height, sectionSize);
	land->SetCamera(World.GetDebugCamera());
	World.AddEntity(LandscapeActor);

	return true;
}
