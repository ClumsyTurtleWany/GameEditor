#pragma once
#include "Define.h"
//#include "fbxsdk.h" // 헤더 안에 하위 헤더들을 불러오지 못함.
#include <fbxsdk.h>
//#pragma comment(lib, "libfbxsdk.lib")
#pragma comment(lib, "libfbxsdk-md.lib") // 다중 스레드 디버그 dll (md), 다중 스레드 (mt)
#pragma comment(lib, "libxml2-md.lib") // libfbxsdk-md 에 필요한 lib
#pragma comment(lib, "zlib-md.lib") // libfbxsdk-md 에 필요한 lib

#pragma warning(disable : 26451)
#pragma warning(disable : 26495)
#pragma warning(disable : 26812)

#include "FBXObject.hpp"
#include "DXTextureManager.hpp"
#include "StaticMeshComponent.h"
#include "SkeletalMeshComponent.h"
// 추가
#include "AnimationComponent.h"


class FBXLoader : public Singleton<FBXLoader>
{
private:
	friend class Singleton<FBXLoader>;

	FbxManager*		m_pManager = nullptr;
	FbxImporter*	m_pImporter = nullptr;
	
	std::wstring	ResourceDirection = L"";

	std::map<std::wstring, FBXFileData*> FbxFileList;

public:
	virtual bool Initialize();
	virtual bool Release();

private:
	// Parser
	bool ParseScene(FbxScene* scene, FBXFileData* dst);
	bool ParseNode(FbxNode* node, FBXFileData* dst);
	bool ParseMesh(FbxMesh* mesh, FBXFileData* dst, FBXNodeData* dstData);
	bool ParseMeshSkinning(FbxMesh* mesh, FBXFileData* dst, FBXNodeData* dstData);
	bool ParseDummy(FbxNull* dummy, FBXFileData* dst);
	bool ParseSkeleton(FbxSkeleton* skeleton, FBXFileData* dst);
	bool PreProcess(FBXFileData* dst);
	
	// Read Data
	bool ReadTextureCoord(FbxLayerElementUV* uv, int vertexIdx, int uvIdx, FbxVector2& dst);
	bool ReadColorCoord(FbxLayerElementVertexColor* color, int vertexIdx, int colorIdx, FbxColor& dst);
	bool ReadNormal(FbxLayerElementNormal* normal, int vertexIdx, int normalIdx, FbxVector4& dst);
	bool ReadTangent(FbxLayerElementTangent* tangent, int vertexIdx, int tangentIdx, FbxVector4& dst);

	int GetSubMaterialIndex(FbxLayerElementMaterial* material, int polyIdx);

	DXTexture* FindTexture(FbxSurfaceMaterial* surface, const char* name, std::wstring* rst = nullptr);
	std::wstring GetTextureFileName(FbxSurfaceMaterial* surface, const char* name);

	// Matrix
	FbxAMatrix GetGeometryMatrix(FbxNode* node);
	FbxAMatrix GetNormalMatrix(const FbxAMatrix& src);
	FbxAMatrix GetWorldMatrix(FbxNode* node);

public:
	bool LoadDir(std::wstring path);
	//bool Load(std::wstring _path, FBXObject* _dst);
	bool Load(std::wstring filename);
	bool GetFBXFileList(std::wstring path, std::vector<std::wstring>& dst);

public:
	//FBXObject* getObject(std::wstring _key);

public:
	void SetResourceDirectory(std::wstring dir);

public:
	Matrix ToMatrix(const FbxAMatrix& _src);
	Matrix ConvertToDxMatrix(const FbxAMatrix& _src);
	inline bool Matrix4x4Decompose(Matrix& src, Vector3& scale, Vector4& rotation, Vector3& translation)
	{
		DirectX::XMVECTOR s, r, t;

		if (!DirectX::XMMatrixDecompose(&s, &r, &t, src))
		{
			return false;
		}

		scale = s;
		rotation = r;
		translation = t;

		return true;
	}

	// (Vector3f) Linear Interpolation
	inline Vector3 LinearInterpolation(const Vector3& _v1, const Vector3& _v2, float _t)
	{
		DirectX::XMVECTOR v1 = _v1;
		DirectX::XMVECTOR v2 = _v2;
		DirectX::XMVECTOR rst = DirectX::XMVectorLerp(v1, v2, _t);
		return rst;
	}

	inline Vector4 LinearInterpolation(const Vector4& _v1, const Vector4& _v2, float _t)
	{
		DirectX::XMVECTOR v1 = _v1;
		DirectX::XMVECTOR v2 = _v2;
		DirectX::XMVECTOR rst = DirectX::XMVectorLerp(v1, v2, _t);
		return rst;
	}

	// (Quaternion) Linear Interpolation
	inline Vector4 SphereLinearInterpolation(const Vector4& _v1, const Vector4& _v2, float _t)
	{
		DirectX::XMVECTOR v1 = _v1;
		DirectX::XMVECTOR v2 = _v2;
		DirectX::XMVECTOR rst = DirectX::XMQuaternionSlerp(v1, v2, _t);
		return rst;
	}

	inline Matrix QuaternionToMatrix4x4(const Vector4& _v)
	{
		DirectX::XMVECTOR v = _v;
		DirectX::XMMATRIX rst = DirectX::XMMatrixRotationQuaternion(v);
		return rst;
	}

public:
	bool GenerateAnimationTrack(FBXFileData* data, float sampling);
	bool GenerateStaticMeshFromFileData(std::wstring filename, StaticMeshComponent* dst);
	bool GenerateSkeletalMeshFromFileData(std::wstring filename, SkeletalMeshComponent* dst);
	// 추가
	bool GenerateAnimationFromFileData(std::wstring filename, AnimationComponent* dst);
};