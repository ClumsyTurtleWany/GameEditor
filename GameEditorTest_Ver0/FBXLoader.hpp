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


class FBXLoader : public Singleton<FBXLoader>
{
private:
	friend class Singleton<FBXLoader>;

	FbxManager*		m_pManager = nullptr;
	FbxImporter*	m_pImporter = nullptr;
	
	std::wstring	m_wstrResourceDir = L"";

	std::map<std::wstring, std::unique_ptr<FBXObject>> m_ObjectMap;
	std::map<std::wstring, std::unique_ptr<StaticMeshComponent>> m_StaticMeshMap;
	std::vector<std::wstring> KeyStringList;

public:
	virtual bool initialize();
	virtual bool frame();
	virtual bool render();
	virtual bool release();

private:
	// Parser
	bool ParseScene(FbxScene* _scene, FBXFileData* _dst);
	bool ParseNode(FbxNode* _node, FBXFileData* _dst);
	bool PreProcess(FBXFileData* _dst);
	bool ParseMesh(FbxMesh* _mesh, FBXFileData* _dst, FBXNodeData* _dstData);
	bool ParseMeshLayer(FbxMesh* _mesh, MeshData* _dstData);
	bool ParseMeshSkinning(FbxMesh* _mesh, FBXFileData* _dst, FBXNodeData* _dstData);
	bool ParseDummy(FbxNull* _dummy, FBXFileData* _dst);
	bool ParseSkeleton(FbxSkeleton* _skeleton, FBXFileData* _dst);
	
	
	// Read Data
	bool ReadTextureCoord(FbxLayerElementUV* _uv, int _vertexIdx, int _uvIdx, FbxVector2& _dst);
	bool ReadColorCoord(FbxLayerElementVertexColor* _color, int _vertexIdx, int _colorIdx, FbxColor& _dst);
	bool ReadNormal(FbxLayerElementNormal* _normal, int _vertexIdx, int _normalIdx, FbxVector4& _dst);
	bool ReadTangent(FbxLayerElementTangent* _tangent, int _vertexIdx, int _tangentIdx, FbxVector4& _dst);

	int getSubMaterialIndex(FbxLayerElementMaterial* _material, int _polyIdx);

	DXTexture* FindTexture(FbxSurfaceMaterial* _surface, const char* _name, std::wstring* _rst = nullptr);
	std::wstring getTextureFileName(FbxSurfaceMaterial* _surface, const char* _name);

	// Matrix
	FbxAMatrix getGeometryMatrix(FbxNode* _node);
	FbxAMatrix getNormalMatrix(const FbxAMatrix& _src);
	FbxAMatrix getWorldMatrix(FbxNode* _node);

public:
	bool LoadDir(std::wstring _path);
	bool Load(std::wstring _path, FBXObject* _dst);
	bool Load(std::wstring _path, StaticMeshComponent* _dst);
	bool GetFBXFileList(std::wstring _path, std::vector<std::wstring>& _dst);

public:
	FBXObject* getObject(std::wstring _key);

public:
	void setResourceDirectory(std::wstring _dir);

public:
	Matrix toMatrix4x4(const FbxAMatrix& _src);
	Matrix ConvertToDxMatrix(const FbxAMatrix& _src);
	inline bool Matrix4x4Decompose(Matrix& _src, Vector3& _scale, Vector4& _rotation, Vector3& _translation)
	{
		DirectX::XMVECTOR s, r, t;

		if (!DirectX::XMMatrixDecompose(&s, &r, &t, _src))
		{
			return false;
		}

		_scale = s;
		_rotation = r;
		_translation = t;

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
	bool GenerateAnimationTrack(FBXFileData* _data);
	bool GenerateObjectFromFileData(FBXFileData* _src, FBXObject* _dst);
	bool GenerateStaticMeshFromFileData(FBXFileData* _src, StaticMeshComponent* _dst);
};