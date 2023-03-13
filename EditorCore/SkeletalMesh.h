#pragma once
#include "StaticMesh.h"
#include "IWData.h"

class SkeletalMesh : public StaticMesh
{
public:
	ID3D11Buffer* SkinningBuffer = nullptr;
	std::vector<IWData> SkinningIWList;

public:
	SkeletalMesh();
	virtual ~SkeletalMesh();

public:
	virtual bool Render() override;
	virtual bool Initialize() override;
};