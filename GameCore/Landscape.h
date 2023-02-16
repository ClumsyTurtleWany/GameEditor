#pragma once
#include "Define.h"
#include "LandscapeComponent.h"
#include "TransformComponent.h"
#include "DebugCamera.h"

class Landscape
{
private:
	ID3D11DeviceContext* Context;

public:
	std::vector<LandscapeComponent> Components;
	ID3D11InputLayout* VertexLayout = nullptr;
	ID3D11VertexShader* VertexShader = nullptr;
	ID3D11HullShader* HullShader = nullptr;
	ID3D11DomainShader* DomainShader = nullptr;
	ID3D11GeometryShader* GeometryShader = nullptr;
	ID3D11Buffer* TransformBuffer = nullptr;
	TransformMatrix			TransformData;

public:
	Camera* MainCamera;
	

public:
	Landscape();
	virtual ~Landscape();

public:
	void Build(int row, int column, int sectionSize);
	void PreRender();
	void Render();
	void SetContext(ID3D11DeviceContext* context);
	void SetCamera(Camera* camera);
	void UpdateTransformMatrix(const TransformComponent& transform);

public:
	bool Initialize();
};
