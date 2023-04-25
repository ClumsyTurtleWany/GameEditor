#pragma once
#include "Define.h"
#include "StaticMesh.h"
#include "TransformComponent.h"
#include "Component.hpp"

class StaticMeshComponent : public ECS::Component
{
public:
	std::wstring Name;
	std::wstring								FBXName;	// ADD
	std::string FilePath;

	std::vector<StaticMesh> Meshes;
	ID3D11InputLayout* VertexLayout = nullptr;
	ID3D11VertexShader* VertexShader = nullptr;
	ID3D11HullShader* HullShader = nullptr;
	ID3D11DomainShader* DomainShader = nullptr;
	ID3D11GeometryShader* GeometryShader = nullptr;
	ID3D11Buffer* TransformBuffer = nullptr;
	TransformMatrix			TransformData;

	bool isCreated = false;

	ID3D11VertexShader* ShadowVertexShader = nullptr;
	ID3D11PixelShader* ShadowPixelShader = nullptr;

public:
	StaticMeshComponent();
	virtual ~StaticMeshComponent();

public:
	virtual bool Initialize();
	virtual bool Render();
	virtual bool RenderShadow();
	virtual bool Save(std::wstring filename);// override;
	virtual bool Load(std::wstring filename);// override;

public:
	void UpdateTransformData(const TransformMatrix& transform);
	virtual void UpdateTransformMatrix(const TransformComponent& transform);
	bool SplitString(std::string line, char delimiter, std::vector<std::string>& dst);

public:
	bool WriteXML(TiXmlElement* parent);
	bool ReadXML(TiXmlElement* parent);

public:
	bool Load(std::string filename);
};

