#pragma once
#include "Define.h"
#include "System.hpp"
#include "Camera.h"

class LightSystem : public ECS::System
{
public:
	DirectionalLightData DirectionalLights;
	PointLightData		PointLights;
	SpotLightData		SpotLights;
	EyeData				Eye;

	ID3D11Buffer* DirectionalLightBuffer;
	ID3D11Buffer* PointLightBuffer;
	ID3D11Buffer* SpotLightBuffer;
	ID3D11Buffer* EyeBuffer;

	// Shadow
	DXRenderTarget* DirectionalLightShadowRenderTarget;
	DXRenderTarget* PointLightShadowRenderTarget;
	DXRenderTarget* SpotLightShadowRenderTarget;

	Matrix TextureConversionMatrix;

	CameraMatrix DirectionalLightShadowMatrix;
	CameraMatrix PointLightShadowMatrix;
	CameraMatrix SpotLightShadowMatrix;

	ID3D11Buffer* DirectionalLightShadowBuffer;
	ID3D11Buffer* PointLightShadowBuffer;
	ID3D11Buffer* SpotLightShadowBuffer;

public:
	Camera* MainCamera = nullptr;

public:
	virtual void Tick(ECS::World* world, float time) override;

	void Initialize();
	void CleanUp();
};