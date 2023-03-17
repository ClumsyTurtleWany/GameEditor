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
	std::string									Name;							///< FbxFile�� RootNodeName�� �޾ƿ� (BindPoseMap�� ����ִ� ��쿡 ����ϱ� ���ؼ� �ϴ� �־��� - ����� Ȯ�� ���غ�)
	std::map<std::string, Matrix>				BindPoseMap;					///< FbxFile�� NodeDataList���� MeshType ��忡 �ִ� �� �޾ƿ� (�ϴ� �̱۸޽� �����ϰ� ���� �ٱ����� �޾���, GenerateSkeletalMeshFromFileData() ����) 
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