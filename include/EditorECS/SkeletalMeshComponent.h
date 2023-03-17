#pragma once
#include "Define.h"
#include "SkeletalMesh.h"
#include "BindPoseAnimationData.h"
#include "TransformComponent.h"

class SkeletalMeshComponent
{
public:	
	std::vector<SkeletalMesh> Meshes;
	ID3D11InputLayout* VertexLayout = nullptr;
	ID3D11VertexShader* VertexShader = nullptr;
	ID3D11HullShader* HullShader = nullptr;
	ID3D11DomainShader* DomainShader = nullptr;
	ID3D11GeometryShader* GeometryShader = nullptr;
	ID3D11Buffer* TransformBuffer = nullptr;
	TransformMatrix			TransformData;
	ID3D11Buffer* BPABuffer = nullptr;
	BindPoseAnimationData	BPAData;

	bool isCreated = false;

	// add
	std::string									Name;							///< FbxFile의 RootNodeName을 받아옴 (BindPoseMap이 비어있는 경우에 사용하기 위해서 일단 넣었음 - 제대로 확인 안해봄)
	std::map<std::string, Matrix>				BindPoseMap;					///< FbxFile의 NodeDataList에서 MeshType 노드에 있는 맵 받아옴 (일단 싱글메시 가정하고 포문 바깥에서 받았음, GenerateSkeletalMeshFromFileData() 참고) 
	std::map<std::string, unsigned int>			BindPoseKeyToIndexMap;

public:
	SkeletalMeshComponent();
	virtual ~SkeletalMeshComponent();

public:
	virtual bool Render();
	virtual bool Initialize();

public:
	virtual void UpdateTransformMatrix(const TransformComponent& transform);
};