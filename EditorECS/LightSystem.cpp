#include "LightSystem.h"
#include "World.hpp"
#include "LightComponent.h"
#include "TransformComponent.h"
#include "DXShaderManager.h"
#include "Camera.h"

void LightSystem::Tick(ECS::World* world, float time)
{
	DirectionalLights.Cnt = 0;
	for (auto& entity : world->GetEntities<DirectionalLightComponent>())
	{
		auto lightComp = entity->GetComponent<DirectionalLightComponent>();
		auto transformComp = entity->GetComponent<TransformComponent>();

		DirectionalLights.Color[DirectionalLights.Cnt] = lightComp->Color;
		DirectionalLights.Direction[DirectionalLights.Cnt] = lightComp->Direction;
		DirectionalLights.Cnt++;
		if (DirectionalLights.Cnt >= MAX_LIGHT_CNT)
		{
			break;
		}
	}

	PointLights.Cnt = 0;
	for (auto& entity : world->GetEntities<PointLightComponent>())
	{
		auto lightComp = entity->GetComponent<PointLightComponent>();
		auto transformComp = entity->GetComponent<TransformComponent>();

		PointLights.Color[PointLights.Cnt] = lightComp->Color;
		PointLights.Direction[PointLights.Cnt] = lightComp->Direction;
		PointLights.Position[PointLights.Cnt] = Vector4(transformComp->Translation.x, transformComp->Translation.y, transformComp->Translation.z, 0.0f);
		PointLights.Radius[PointLights.Cnt] = lightComp->Radius;
		PointLights.Cnt++;
		if (PointLights.Cnt >= MAX_LIGHT_CNT)
		{
			break;
		}
	}

	SpotLights.Cnt = 0;
	for (auto& entity : world->GetEntities<SpotLightComponent>())
	{
		auto lightComp = entity->GetComponent<SpotLightComponent>();
		auto transformComp = entity->GetComponent<TransformComponent>();

		SpotLights.Color[PointLights.Cnt] = lightComp->Color;
		SpotLights.Direction[PointLights.Cnt] = lightComp->Direction;
		SpotLights.Position[PointLights.Cnt] = Vector4(transformComp->Translation.x, transformComp->Translation.y, transformComp->Translation.z, 0.0f);
		SpotLights.Radius[PointLights.Cnt] = lightComp->Radius;
		SpotLights.Cnt++;
		if (SpotLights.Cnt >= MAX_LIGHT_CNT)
		{
			break;
		}
	}

	Camera* camera = nullptr;
	for (auto& entity : world->GetEntities<Camera>())
	{
		camera = entity->GetComponent<Camera>();
	}
	
	if (camera != nullptr)
	{
		Eye.Position = Vector4(camera->Pos.x, camera->Pos.y, camera->Pos.z, 0.0f);
		Eye.Direction = Vector4(camera->Look.x, camera->Look.y, camera->Look.z, 0.0f);
	}
	else
	{
		Eye.Position = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		Eye.Direction = Vector4(0.0f, 0.0f, 1.0f, 0.0f);
	}

	Context->UpdateSubresource(DirectionalLightBuffer, 0, NULL, &DirectionalLights, 0, 0);
	Context->UpdateSubresource(PointLightBuffer, 0, NULL, &PointLights, 0, 0);
	Context->UpdateSubresource(SpotLightBuffer, 0, NULL, &SpotLights, 0, 0);
	Context->UpdateSubresource(EyeBuffer, 0, NULL, &Eye, 0, 0);

	Context->PSSetConstantBuffers(0, 1, &DirectionalLightBuffer);
	Context->PSSetConstantBuffers(1, 1, &PointLightBuffer);
	Context->PSSetConstantBuffers(2, 1, &SpotLightBuffer);
	Context->PSSetConstantBuffers(3, 1, &EyeBuffer);
}

void LightSystem::SetContext(ID3D11DeviceContext* context)
{
	Context = context;
}

void LightSystem::Initialize()
{
	DirectionalLightBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<DirectionalLightData>(DirectionalLights);
	PointLightBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<PointLightData>(PointLights);
	SpotLightBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<SpotLightData>(SpotLights);
	EyeBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<EyeData>(Eye);

}
