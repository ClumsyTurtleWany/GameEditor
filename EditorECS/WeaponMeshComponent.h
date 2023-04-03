#pragma once
#include "Define.h"
#include "StaticMesh.h"
#include "TransformComponent.h"
#include "Component.hpp"
#include "SkeletalMeshComponent.h"

class WeaponMeshComponent
{
public:
	int SocketIndex = 0;
	Matrix InitOffset;				///< 전치된 상태로 설정되어있음

	std::wstring Name;

	std::vector<StaticMesh> Meshes;
	ID3D11InputLayout* VertexLayout = nullptr;
	ID3D11VertexShader* VertexShader = nullptr;
	ID3D11HullShader* HullShader = nullptr;
	ID3D11DomainShader* DomainShader = nullptr;
	ID3D11GeometryShader* GeometryShader = nullptr;
	ID3D11Buffer* PositionBuffer = nullptr;
	TransformMatrix			PositionData;

	bool isCreated = false;

public:
	class WeaponMeshComponent();
	virtual ~WeaponMeshComponent();

public:
	virtual bool Initialize();
	virtual bool Render();

public:
	void SetOffset(Vector3 T, Vector3 R, Vector3 S, Vector3 Origin);
	bool Attach(const SkeletalMeshComponent& mesh, std::string bone);
	virtual void UpdatePos(TransformMatrix pos);
};