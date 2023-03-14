#pragma once
#include "Define.h"

//#include "fbxsdk.h" // ��� �ȿ� ���� ������� �ҷ����� ����.
#include <fbxsdk.h>
//#pragma comment(lib, "libfbxsdk.lib")
#pragma comment(lib, "libfbxsdk-md.lib") // ���� ������ ����� dll (md), ���� ������ (mt)
#pragma comment(lib, "libxml2-md.lib") // libfbxsdk-md �� �ʿ��� lib
#pragma comment(lib, "zlib-md.lib") // libfbxsdk-md �� �ʿ��� lib

#pragma warning(disable : 26451)
#pragma warning(disable : 26495)
#pragma warning(disable : 26812)

#include <set>
#include "StaticMeshComponent.h"
#include "SkeletalMeshComponent.h"


struct FBXAnimationTrack
{
	UINT Frame;
	Matrix AnimationMatrix; // self * parent
	Matrix SelfAnimationMatrix;  // matAnimation * inv(Parent)
	Vector3 Translation;
	Vector4 Rotation; // Quaternion
	Vector3 Scale;
};

struct FBXAnimationSceneInfo
{
	FbxTime::EMode TimeMode;
	UINT StartFrame;
	UINT EndFrame;
	float TickPerFrame = 160.0f;
	float FrameSpeed = 30.0f;
};

struct SkinWeight
{
	float weight = 0.0f;
	std::string BoneName = "";

	bool operator <(const SkinWeight& _comp) const
	{
		return weight < _comp.weight;
	}

	bool operator >(const SkinWeight& _comp) const
	{
		return weight > _comp.weight;
	}
};

struct SkinningData
{
	// IndexWeightData �� �Ѱ��ִ°� 4���ϱ�, Sort �ؼ� �־�δ°� ����ϱ⿡ ����.
	std::multiset<SkinWeight, std::greater<SkinWeight>> SkinWeightList;

	void insert(float _weight, std::string _boneName)
	{
		SkinWeight data;
		data.weight = _weight;
		data.BoneName = _boneName;
		SkinWeightList.insert(data);
	}

	void insert(SkinWeight _data)
	{
		SkinWeightList.insert(_data);
	}
};

using FBXAnimationTrackList = std::vector<FBXAnimationTrack>;

struct FBXNodeData
{
	// Node Type
	FbxNodeAttribute::EType AttributeType;

	// Node Name
	std::string				Name;

	FbxNode*				Node = nullptr;
	FbxMesh*				FMesh = nullptr;
	FbxNull*				Dummy = nullptr;
	FbxSkeleton*			Skeleton = nullptr;

	// Local Geometry Matrix
	FbxAMatrix				LocalGeometryMatrix;
	FbxAMatrix				LocalNormalMatrix;

	// Bind Pose Matrix
	//Matrix4x4				BindPoseMatrix;
	std::map<std::string, Matrix>				BindPoseMap;
	std::map<std::string, unsigned int>				BindPoseKeyToIndexMap;

	// Original Animation Track
	FBXAnimationTrackList	AnimationTrack;
	// Interpolation Animation Track
	std::vector<Matrix>	InterpolationFrameMatrix;

	int MaterialNum = 0;
	
	//std::vector<FbxMaterial> Materials;

	std::vector<SkinningData> SkinningList;

	std::vector<SkeletalMesh> MeshList;
	std::vector<Material*> MaterialList;
};

class FBXFileData
{
public:
	std::wstring									FilePath;
	FBXAnimationSceneInfo							AnimationSceneInfo;
	std::map<std::string, FBXAnimationTrackList>	AnimationTrackMap;

	std::map<std::string, std::vector<Matrix>>		InterpolationFrameMatrixList;

	//std::map<std::string, Matrix4x4>				BindPoseMap;
	std::map<std::string, FbxAMatrix>				LocalGeometryMatrixMap;
	std::map<std::string, FbxAMatrix>				NormalMatrixMap;

	std::vector<FbxNode*>							NodeList;
	std::vector<std::string>						NodeNameList;
	std::vector<FbxMesh*>							MeshList;
	std::vector<FbxNull*>							DummyList;
	std::vector<FbxSkeleton*>						SkeletonList;

	std::vector<FBXNodeData>						NodeDataList;
};
