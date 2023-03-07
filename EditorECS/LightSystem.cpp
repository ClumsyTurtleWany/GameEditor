#include "LightSystem.h"
#include "World.hpp"
#include "LightComponent.h"
#include "TransformComponent.h"
#include "DXShaderManager.h"
#include "Camera.h"

void LightSystem::Tick(ECS::World* world, float time)
{
	CleanUp();

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
		PointLights.Radius[PointLights.Cnt].x = lightComp->Radius;
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

		SpotLights.Color[SpotLights.Cnt] = lightComp->Color;
		SpotLights.Direction[SpotLights.Cnt] = lightComp->Direction;
		SpotLights.Position[SpotLights.Cnt] = Vector4(transformComp->Translation.x, transformComp->Translation.y, transformComp->Translation.z, 0.0f);
		SpotLights.Radius[SpotLights.Cnt].x = lightComp->Radius;
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
		break;
	}

	if (camera != nullptr)
	{
		Eye.Position = Vector4(camera->Pos.x, camera->Pos.y, camera->Pos.z, 0.0f);
		Eye.Direction = Vector4(camera->Look.x, camera->Look.y, camera->Look.z, 0.0f);

		if (DirectionalLightBuffer != nullptr)
		{
			DXDevice::g_pImmediateContext->UpdateSubresource(DirectionalLightBuffer, 0, NULL, &DirectionalLights, 0, 0);
			DXDevice::g_pImmediateContext->PSSetConstantBuffers(0, 1, &DirectionalLightBuffer);
		}

		if (PointLightBuffer != nullptr)
		{
			DXDevice::g_pImmediateContext->UpdateSubresource(PointLightBuffer, 0, NULL, &PointLights, 0, 0);
			DXDevice::g_pImmediateContext->PSSetConstantBuffers(1, 1, &PointLightBuffer);
		}

		if (SpotLightBuffer != nullptr)
		{
			DXDevice::g_pImmediateContext->UpdateSubresource(SpotLightBuffer, 0, NULL, &SpotLights, 0, 0);
			DXDevice::g_pImmediateContext->PSSetConstantBuffers(2, 1, &SpotLightBuffer);
		}

		if (EyeBuffer != nullptr)
		{
			DXDevice::g_pImmediateContext->UpdateSubresource(EyeBuffer, 0, NULL, &Eye, 0, 0);
			DXDevice::g_pImmediateContext->PSSetConstantBuffers(3, 1, &EyeBuffer);
		}
	}
}

void LightSystem::Initialize()
{
	DirectionalLightBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<DirectionalLightData>(DirectionalLights);
	PointLightBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<PointLightData>(PointLights);
	SpotLightBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<SpotLightData>(SpotLights);
	EyeBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<EyeData>(Eye);

}

void LightSystem::CleanUp()
{
	for (int idx = 0; idx < MAX_LIGHT_CNT; idx++)
	{
		DirectionalLights.Color[idx] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		DirectionalLights.Direction[idx] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		SpotLights.Color[idx] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		SpotLights.Position[idx] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		SpotLights.Direction[idx] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		SpotLights.Radius[idx] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		PointLights.Color[idx] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		PointLights.Position[idx] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		PointLights.Direction[idx] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		PointLights.Radius[idx] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	DirectionalLights.Cnt = 0;
	SpotLights.Cnt = 0;
	PointLights.Cnt = 0;
	Eye.Position = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	Eye.Direction = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
}
