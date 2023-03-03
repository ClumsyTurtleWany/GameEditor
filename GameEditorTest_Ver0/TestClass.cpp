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
#include "LightSystem.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "CameraSystem.h"

//#include "Landscape.h"

TestClass::TestClass(HWND hWnd)
{
	SetWindowHandle(hWnd);
}

bool TestClass::Initialize()
{
	Actor* NewActor = new Actor;
	//auto comp = NewActor->AddComponent<StaticMeshComponent>();
	//comp->SetContext(Device.m_pImmediateContext);
	//MeshComponent NewMesh;
	//NewMesh.SetContext(Device.m_pImmediateContext);
	//NewMesh.Vertices.assign(4, Vertex());
	//NewMesh.Vertices[0].Pos = { -1.0f, +1.0f, 0.0f }; // p1-LT
	//NewMesh.Vertices[1].Pos = { +1.0f, +1.0f, 0.0f }; // p2-RT
	//NewMesh.Vertices[2].Pos = { -1.0f, -1.0f, 0.0f }; // p3-LB
	//NewMesh.Vertices[3].Pos = { +1.0f, -1.0f, 0.0f }; // p4-LB

	//NewMesh.Vertices[0].Normal = { 0.0f, 0.0f, -1.0f };
	//NewMesh.Vertices[1].Normal = { 0.0f, 0.0f, -1.0f };
	//NewMesh.Vertices[2].Normal = { 0.0f, 0.0f, -1.0f };
	//NewMesh.Vertices[3].Normal = { 0.0f, 0.0f, -1.0f };

	//NewMesh.Vertices[0].Color = { 1.0f, 0.0f, 0.0f, 1.0f }; // p1-LT
	//NewMesh.Vertices[1].Color = { 0.0f, 1.0f, 0.0f, 1.0f }; // p2-RT
	//NewMesh.Vertices[2].Color = { 0.0f, 0.0f, 1.0f, 1.0f }; // p3-LB
	//NewMesh.Vertices[3].Color = { 1.0f, 1.0f, 1.0f, 1.0f }; // p4-LB

	//NewMesh.Vertices[0].Texture = { 0.0f, 0.0f }; // p1-LT
	//NewMesh.Vertices[1].Texture = { 1.0f, 0.0f }; // p2-RT
	//NewMesh.Vertices[2].Texture = { 0.0f, 1.0f }; // p3-LB
	//NewMesh.Vertices[3].Texture = { 1.0f, 1.0f }; // p4-LB

	//NewMesh.Indices.assign(6, 0);
	//NewMesh.Indices[0] = 0;
	//NewMesh.Indices[1] = 1;
	//NewMesh.Indices[2] = 2;
	//NewMesh.Indices[3] = 2;
	//NewMesh.Indices[4] = 1;
	//NewMesh.Indices[5] = 3;

	//comp->Meshes.push_back(NewMesh);
	/*DebugCamera* camera = new DebugCamera;
	camera->SetContext(Device.m_pImmediateContext);
	camera->CreateViewMatrix(Vector3(0.0f, 0.0f, -10.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0, 0.0f));
	camera->CreateProjectionMatrix(1.0f, 500.0f, PI * 0.25, (Device.m_ViewPort.Width) / (Device.m_ViewPort.Height));
	World.SetDebugCamera(camera);*/

	//World.AddSystem(new DebugCameraSystem);

	NewActor->AddComponent<Camera>();
	MainCamera = NewActor->GetComponent<Camera>();
	MainCamera->SetContext(Device.m_pImmediateContext);
	MainCamera->CreateViewMatrix(Vector3(0.0f, 10.0f, -25.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0, 0.0f));
	MainCamera->CreateProjectionMatrix(1.0f, 500.0f, PI * 0.25, (Device.m_ViewPort.Width) / (Device.m_ViewPort.Height));
	
	World.AddSystem(new CameraSystem);
	

	LightSystem* lightSystem = new LightSystem;
	lightSystem->SetContext(Device.m_pImmediateContext);
	lightSystem->Initialize();
	World.AddSystem(lightSystem);

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

	/*DirectionalLight* dirLight = new DirectionalLight;
	auto lightComp = dirLight->GetComponent<DirectionalLightComponent>();
	lightComp->Color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	lightComp->Direction = Vector4(0.0f, -1.0f, 1.0f, 1.0f);
	World.AddEntity(dirLight);*/

	/*SpotLight* spotLight = new SpotLight;
	auto lightComp2 = spotLight->GetComponent<SpotLightComponent>();
	lightComp2->Color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	lightComp2->Direction = Vector4(0.0f, -1.0f, 1.0f, 1.0f);
	lightComp2->Radius = 55.0f;
	auto transformComp = spotLight->GetComponent<TransformComponent>();
	transformComp->Translation += Vector3(10.0f, 20.0f, 10.0f);*/

	/*PointLight* spotLight = new PointLight;
	auto lightComp2 = spotLight->GetComponent<PointLightComponent>();
	lightComp2->Color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	lightComp2->Direction = Vector4(0.0f, 1.0f, 1.0f, 1.0f);
	lightComp2->Radius = 25.0f;
	auto transformComp = spotLight->GetComponent<TransformComponent>();
	transformComp->Translation += Vector3(20.0f, 10.0f, 20.0f);

	World.AddEntity(spotLight);*/

	return true;
}

bool TestClass::Frame()
{
	//Camera* camera = World.GetDebugCamera();
	Picker.View = MainCamera->View;
	Picker.Projection = MainCamera->Projection;
	
	Picker.Update();
	
	//KeyState keyState_RButton = Input::getInstance()->getKey(VK_RBUTTON);
	//if (keyState_RButton == KeyState::Hold)
	//{
	//	if (!SelectedFilename.empty())
	//	{
	//		for (auto& it : World.GetEntities<Landscape>())
	//		{
	//			auto landscape = it->GetComponent<Landscape>();
	//			for (auto& comp : landscape->Components)
	//			{
	//				for (auto& face : comp.Faces)
	//				{
	//					if (Picker.CheckPick(face.V0.Pos, face.V1.Pos, face.V2.Pos))
	//					{
	//						auto mesh = StaticMeshMap.find(SelectedFilename);
	//						if (mesh != StaticMeshMap.end())
	//						{
	//							StaticMeshComponent* meshComp = mesh->second.get();
	//							Actor* NewActor = new Actor;
	//							auto staticMeshComp = NewActor->AddComponent<StaticMeshComponent>();
	//							staticMeshComp->Meshes.assign(meshComp->Meshes.begin(), meshComp->Meshes.end());
	//							for (auto& it : staticMeshComp->Meshes)
	//							{
	//								it.SetContext(Device.m_pImmediateContext);
	//							}
	//							staticMeshComp->SetContext(Device.m_pImmediateContext);

	//							auto transform = NewActor->GetComponent<TransformComponent>();
	//							transform->Translation = Picker.IntercetionPosition;

	//							World.AddEntity(NewActor);
	//						}
	//						else
	//						{
	//							std::unique_ptr<StaticMeshComponent> staticMesh = std::make_unique<StaticMeshComponent>();
	//							if (FBXLoader::getInstance()->Load(L"../resource/FBX/" + SelectedFilename, staticMesh.get()))
	//							{
	//								StaticMeshMap.insert(std::make_pair(SelectedFilename, std::move(staticMesh)));
	//							}

	//							auto mesh = StaticMeshMap.find(SelectedFilename);
	//							if (mesh != StaticMeshMap.end())
	//							{
	//								StaticMeshComponent* meshComp = mesh->second.get();
	//								Actor* NewActor = new Actor;
	//								auto staticMeshComp = NewActor->AddComponent<StaticMeshComponent>();
	//								staticMeshComp->Meshes.assign(meshComp->Meshes.begin(), meshComp->Meshes.end());
	//								//staticMeshComp = meshComp;
	//								for (auto& it : staticMeshComp->Meshes)
	//								{
	//									it.SetContext(Device.m_pImmediateContext);
	//								}
	//								staticMeshComp->SetContext(Device.m_pImmediateContext);

	//								auto transform = NewActor->GetComponent<TransformComponent>();
	//								transform->Translation = Picker.IntercetionPosition;

	//								World.AddEntity(NewActor);
	//							}
	//						}
	//					}
	//				}
	//				
	//			}
	//			
	//		}
	//	}
	//	else
	//	{

	//	}
	//}
	

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
	return true;
}

bool TestClass::Resize(int x, int y, int width, int height)
{
	GameCore::ResizeDevice(x, y, width, height);
	//DebugCamera* camera = GetDebugCamera();
	//camera->CreateProjectionMatrix(camera->NearDistance, camera->FarDistance, PI * 0.25, (Device.m_ViewPort.Width) / (Device.m_ViewPort.Height));

	MainCamera->CreateProjectionMatrix(MainCamera->NearDistance, MainCamera->FarDistance, PI * 0.25, (Device.m_ViewPort.Width) / (Device.m_ViewPort.Height));
	Picker.ClientWidth = Device.m_ViewPort.Width;
	Picker.ClientHeight = Device.m_ViewPort.Height;

	return true;
}

ID3D11Device* TestClass::GetDevice()
{
	return Device.m_pd3dDevice;
}

ID3D11DeviceContext* TestClass::GetContext()
{
	return Device.m_pImmediateContext;
}

//DebugCamera* TestClass::GetDebugCamera()
//{
//	return World.GetDebugCamera();
//}

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
	land->SetCamera(MainCamera);
	World.AddEntity(LandscapeActor);

	/*Landscape* LandscapeActor = new Landscape;
	LandscapeActor->SetContext(Device.m_pImmediateContext);
	LandscapeActor->Build(width, height, sectionSize);
	LandscapeActor->SetCamera(World.GetDebugCamera());
	World.AddEntity(LandscapeActor);*/

	return true;
}

bool TestClass::AddSelectedEntity()
{
	if (!SelectedFilename.empty())
	{
		Camera* camera = MainCamera;

		for (auto& it : World.GetEntities<Landscape>())
		{
			auto landscape = it->GetComponent<Landscape>();
			Matrix matWorld = landscape->TransformData.Mat;
			matWorld = matWorld.Transpose();
			for (auto& comp : landscape->Components)
			{
				for (auto& face : comp.Faces)
				{
					//Matrix matComb = matWorld * camera->View * camera->Projection;
					Vector4 v0 = Vector4(face.V0.Pos.x, face.V0.Pos.y, face.V0.Pos.z, 1.0f);
					Vector4 v1 = Vector4(face.V1.Pos.x, face.V1.Pos.y, face.V1.Pos.z, 1.0f);
					Vector4 v2 = Vector4(face.V2.Pos.x, face.V2.Pos.y, face.V2.Pos.z, 1.0f);
					
					Vector4 mul0 = DirectX::XMVector4Transform(v0, matWorld);
					Vector4 mul1 = DirectX::XMVector4Transform(v1, matWorld);
					Vector4 mul2 = DirectX::XMVector4Transform(v2, matWorld);

					Vector3 rv0 = Vector3(mul0.x, mul0.y, mul0.z);
					Vector3 rv1 = Vector3(mul1.x, mul1.y, mul1.z);
					Vector3 rv2 = Vector3(mul2.x, mul2.y, mul2.z);

					//if (Picker.CheckPick(face.V0.Pos, face.V1.Pos, face.V2.Pos))
					if (Picker.CheckPick(rv0, rv1, rv2))
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

						return true;
					}
				}

			}

		}
	}

	return true;
}

bool TestClass::AddSelectedEntityToOrigin()
{
	if (!SelectedFilename.empty())
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
			transform->Translation = Vector3(0.0f, 0.0f, 0.0f);

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
				transform->Translation = Vector3(0.0f, 0.0f, 0.0f);;

				World.AddEntity(NewActor);
			}
		}

		return true;
		
	}

	return true;
}

bool TestClass::Splatting()
{
	Camera* camera = MainCamera;

	for (auto& it : World.GetEntities<Landscape>())
	{
		auto landscape = it->GetComponent<Landscape>();
		Matrix matWorld = landscape->TransformData.Mat;
		matWorld = matWorld.Transpose();
		for (auto& comp : landscape->Components)
		{
			for (auto& face : comp.Faces)
			{
				//Matrix matComb = matWorld * camera->View * camera->Projection;
				Vector4 v0 = Vector4(face.V0.Pos.x, face.V0.Pos.y, face.V0.Pos.z, 1.0f);
				Vector4 v1 = Vector4(face.V1.Pos.x, face.V1.Pos.y, face.V1.Pos.z, 1.0f);
				Vector4 v2 = Vector4(face.V2.Pos.x, face.V2.Pos.y, face.V2.Pos.z, 1.0f);

				Vector4 mul0 = DirectX::XMVector4Transform(v0, matWorld);
				Vector4 mul1 = DirectX::XMVector4Transform(v1, matWorld);
				Vector4 mul2 = DirectX::XMVector4Transform(v2, matWorld);

				Vector3 rv0 = Vector3(mul0.x, mul0.y, mul0.z);
				Vector3 rv1 = Vector3(mul1.x, mul1.y, mul1.z);
				Vector3 rv2 = Vector3(mul2.x, mul2.y, mul2.z);

				//if (Picker.CheckPick(face.V0.Pos, face.V1.Pos, face.V2.Pos))
				if (Picker.CheckPick(rv0, rv1, rv2))
				{
					comp.Splatting(Picker.IntercetionPosition, 2.0f, SelectedSplattingLayer, 0.5);

					return true;
				}
			}

		}

	}
	

	return true;
}

bool TestClass::Sculpting()
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
					Vector3 range = Vector3(10.0f, 10.0f, 10.0f);
					Vector3 max = Picker.IntercetionPosition + range;
					Vector3 min = Picker.IntercetionPosition - range;

					return true;
				}
			}

		}

	}

	return false;
}
