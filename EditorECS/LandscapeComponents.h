#pragma once
#include "LandscapeComponent.h"
#include "TransformComponent.h"
#include "Camera.h"
#include "DXRenderTarget.hpp"

class LandscapeComponents
{
public:
	std::vector<LandscapeComponent> Components;

	ID3D11InputLayout* VertexLayout = nullptr;
	ID3D11VertexShader* VertexShader = nullptr;
	ID3D11PixelShader* PixelShader = nullptr;
	ID3D11HullShader* HullShader = nullptr;
	ID3D11DomainShader* DomainShader = nullptr;
	ID3D11GeometryShader* GeometryShader = nullptr;
	ID3D11Buffer* TransformBuffer = nullptr;
	TransformMatrix			TransformData;

	ID3D11Buffer* StreamOutputBuffer = nullptr;

	SculptingData	SculptData;
	ID3D11Buffer* SculptingBuffer = nullptr;

	DXTexture* BaseTexture = nullptr;
	std::vector<DXTexture*> LayerTextureList;

	int Row;
	int Column;
	int SectionSize;
	int CellDistance;

	Vector3 Center;

	std::vector<DirectX::BoundingBox> BoundingBoxList;

	TransformComponent* Transform;

public:
	Camera* MainCamera;

public:
	bool Initialize();

public:
	void Build(int row = 8, int column = 8, int sectionSize = 8, int cellDistance = 100);
	void Render();
	void SetCamera(Camera* camera);
	void UpdateTransformMatrix(const TransformComponent& transform);
	void SetSculptingData(Vector3 pos, float radius, float attenuation, float strength, float weight);
	void SetBaseTexture(DXTexture* texture);
	void AddLayerTexture(DXTexture* texture);
	void Splatting(Vector3 pos, float radius, int idx, float strength);
};