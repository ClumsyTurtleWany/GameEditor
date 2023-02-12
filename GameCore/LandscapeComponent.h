#pragma once
#include <vector>
#include "SimpleMath.h"

struct Face
{
	union
	{
		struct
		{
			DirectX::SimpleMath::Vector3 _v0;
			DirectX::SimpleMath::Vector3 _v1;
			DirectX::SimpleMath::Vector3 _v2;
		}
		v[3];
	};
};

struct Section
{
	int Column;
	int Row;
	std::vector<Face> Faces;

};

struct LandscapeComponent
{

};