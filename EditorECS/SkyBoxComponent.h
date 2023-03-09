#pragma once
#include "Define.h"
#include "TransformComponent.h"
#include "Camera.h"

class SkyBoxComponent
{
	std::vector<Vertex> Vertices;
	std::vector<DWORD>	Indices;

	ID3D11VertexShader*		VertexShader = nullptr;
	ID3D11HullShader*		HullShader = nullptr;
	ID3D11DomainShader*		DomainShader = nullptr;
	ID3D11GeometryShader*	GeometryShader = nullptr;
	ID3D11PixelShader*		PixelShader = nullptr;
	ID3D11Buffer*			VertexBuffer = nullptr;
	ID3D11Buffer*			IndexBuffer = nullptr;
	ID3D11Buffer*			TransformBuffer = nullptr;
	ID3D11Buffer*			CameraMatrixBuffer = nullptr;
	TransformMatrix			TransformData;
	CameraMatrix			CameraMatrixData;


	DXTexture* FrontTexture = nullptr;
	DXTexture* BackTexture = nullptr;
	DXTexture* LeftTexture = nullptr;
	DXTexture* RightTexture = nullptr;
	DXTexture* UpperTexture = nullptr;
	DXTexture* LowerTexture = nullptr;

	Vector3 Scale = Vector3(100.0f, 100.0f, 100.0f);

public:
	SkyBoxComponent();
	~SkyBoxComponent();

public:
	bool Initialize();
	bool Render();

public:
	void SetFrontTexture(DXTexture* texture);
	void SetBackTexture(DXTexture* texture);
	void SetLeftTexture(DXTexture* texture);
	void SetRightTexture(DXTexture* texture);
	void SetUpperTexture(DXTexture* texture);
	void SetLowerTexture(DXTexture* texture);
	void SetScale(Vector3 scale);

public:
	void UpdateTransformMatrix(const TransformComponent& transform);
	virtual void UpdateCamera(Camera* camera, const TransformComponent& transform);
};

