#pragma once
#include "DXTK/SimpleMath.h"

using namespace DirectX::SimpleMath;

struct PNCTT
{
	Vector3 Pos;
	Vector3 Normal;
	Vector4 Color;
	Vector2 Texture;
	Vector3 Tangent;

	PNCTT() : Pos(Vector3(0.0, 0.0, 0.0)), Normal(Vector3(0.0, 0.0, 0.0)), Color(Vector4(0.0, 0.0, 0.0, 0.0)), Texture(Vector2(0.0, 0.0)), Tangent(Vector3(0.0, 0.0, 0.0)) {};

	PNCTT(const Vector3& pos, const Vector4& color, const Vector2& texture)
		: Pos(pos), Normal(Vector3(0.0, 0.0, 0.0)), Color(color), Texture(texture), Tangent(Vector3(0.0, 0.0, 0.0)) {};

	PNCTT(const Vector3& pos, const Vector3& normal, const Vector4& color, const Vector2& texture)
		: Pos(pos), Normal(normal), Color(color), Texture(texture), Tangent(Vector3(0.0, 0.0, 0.0)) {};
};