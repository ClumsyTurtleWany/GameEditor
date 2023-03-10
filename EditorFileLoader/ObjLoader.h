#pragma once
#include "Define.h"

struct MTLDATA
{
	std::wstring name;
	float Ns; // �ݻ� ����. �ݻ��� ���̶���Ʈ ����. 0 ~ 1000. 100 ���� ũ�� �ٸ���� �߻�.
	Vector3 Ka; // ambient
	Vector3 Kd; // diffuse color
	Vector3 Ks; // specular color
	Vector3 Ke; // emission color
	float Ni; // ǥ���� ���� �е�. ������. 0.001 ~ 10. 1���� ũ�� ����. ���� 1.5 ������ ������ ������
	float d; // ������ ����. 1.0�� �� ������, 1 �̸� �� �� ���� ����.
	float Tr; // 1 - d, d���� ����.
	Vector3 Tf; // �� ���� �Ӽ�. ex) Tf == (0.0, 1.0, 0.0) �� ��� Green�� ���, Red, Blue�� ���͸�.
	
	// illumination. ����� ���� �� ����. 0~10
	// 0 -> Color on, Ambient off
	// 1 -> Color on, Ambient on
	// 2 -> Highlight on
	// 3 -> Reflection on, Ray trace on
	// 4 -> Transparency: Glass on, Reflection: Ray trace on
	// 5 -> Reflection: Fresnel on, Ray trace on
	// 6 -> Transparency: Reflection on, Reflection: Fresnel off, Ray trace on
	// 7 -> Transparency: Reflection on, Reflection: Fresnel on, Ray trace on
	// 8 -> Reflection on, Ray trace off
	// 9 -> Transparency: Glass on, Reflection: Ray trace off
	// 10 -> Casts shadows onto invisible surfaces
	int illum; 
	
	std::wstring map_Ka; // ambient texture map
	std::wstring map_Kd; // diffuse texture map
	std::wstring map_Ks; // specular color texture map
	std::wstring map_Ns; // specular highlight component
	std::wstring map_bump; // bump map (normal map)
	std::wstring map_d; // alpha texture map
};

struct SMDDATA
{

};

struct OBJDATA
{

};

class ObjLoader : public Singleton<ObjLoader>
{
private:
	friend class Singleton<ObjLoader>;

public:
	bool LoadMtl(std::wstring filename);
	bool LoadSmd(std::wstring filename);
	bool LoadObj(std::wstring filename);
};