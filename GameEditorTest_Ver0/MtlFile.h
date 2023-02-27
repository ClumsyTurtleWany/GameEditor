#pragma once
#include "Define.h"
#include <fstream>
#include <sstream>

struct MATERIAL
{
	std::wstring name;
	float Ns = 0.0f; // 반사 지수. 반사광 하이라이트 초점. 0 ~ 1000. 100 보다 크면 앨리어싱 발생.
	Vector3 Ka; // ambient
	Vector3 Kd; // diffuse color
	Vector3 Ks; // specular color
	Vector3 Ke; // emission color
	float Ni = 0.0f; // 표면의 광학 밀도. 굴절률. 0.001 ~ 10. 1보다 크면 굴절. 보통 1.5 정도가 유리의 굴절률
	float d = 0.0f; // 재질의 투명성. 1.0일 때 불투명, 1 미만 일 때 투명도 적용.
	float Tr = 0.0f; // 1 - d, d값의 반전.
	Vector3 Tf; // 빛 투과 속성. ex) Tf == (0.0, 1.0, 0.0) 일 경우 Green만 통과, Red, Blue는 필터링.

	// illumination. 사용할 조명 모델 지정. 0~10
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
	int illum = 0;

	std::wstring map_Ka; // ambient texture map
	std::wstring map_Kd; // diffuse texture map
	std::wstring map_Ks; // specular color texture map
	std::wstring map_Ns; // specular highlight component
	std::wstring map_bump; // bump map (normal map)
	std::wstring map_d; // alpha texture map
};

class MtlFile
{
public:
	std::vector<MATERIAL> _materialList;
	int _materialCnt;

public:
	bool Load(std::wstring filename);

public:
	bool SplitString(std::string line, char delimiter, std::vector<std::string>& dst);
};