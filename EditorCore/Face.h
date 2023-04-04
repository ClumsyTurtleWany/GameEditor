#pragma once
#include "Define.h"

struct Face
{
	/*Vertex V0;
	Vertex V1;
	Vertex V2;

	Face() : V0(Vertex()), V1(Vertex()), V2(Vertex()) {};
	Face(const Vertex& v0, const Vertex& v1, const Vertex& v2)
		: V0(v0), V1(v1), V2(v2)
	{

	}

	Vector3 GetNormal()
	{

	}*/
	union
	{
		struct
		{
			UINT V0;
			UINT V1;
			UINT V2;
		};
		UINT V[3];
	};

	Face() : V0(0), V1(0), V2(0) {};
	Face(const UINT& v0, const UINT& v1, const UINT& v2)
		: V0(v0), V1(v1), V2(v2)
	{

	}


};