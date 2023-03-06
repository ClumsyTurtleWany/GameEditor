#pragma once
#include "Define.h"
#include "System.hpp"
#include "LightData.h"

class LightSystem : public ECS::System
{
private:
	DirectionalLightData DirectionalLights;
	SpotLightData		SpotLights;
	PointLightData		PointLights;
	EyeData				Eye;

	ID3D11Buffer* DirectionalLightBuffer;
	ID3D11Buffer* PointLightBuffer;
	ID3D11Buffer* SpotLightBuffer;
	ID3D11Buffer* EyeBuffer;

public:
	virtual void Tick(ECS::World* world, float time) override;

	void Initialize();
	void CleanUp();
};