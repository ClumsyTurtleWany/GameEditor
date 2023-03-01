#pragma once
#include "Define.h"
#include "LandscapeComponent.h"
#include "TransformComponent.h"
#include "Entity.hpp"
#include "SculptingData.h"
#include "Camera.h"

class Landscape //: public ECS::Entity
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

	ID3D11Buffer* StreamOutputBuffer = nullptr;
	
	SculptingData	SculptData;
	ID3D11Buffer* SculptingBuffer = nullptr;

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
	void SetSculptingData(Vector3 pos, float radius, float attenuation, float strength, float weight);

public:
	bool Initialize();
};
