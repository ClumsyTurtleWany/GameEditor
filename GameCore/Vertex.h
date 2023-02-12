#pragma once
#include "DXTK/SimpleMath.h"

using namespace DirectX::SimpleMath;
struct Vertex
{
	Vector3 Pos;
	Vector3 Normal;
	Vector4 Color;
	Vector2 Texture;

	Vertex() : Pos({ 0.0, 0.0, 0.0 }), Normal({ 0.0, 0.0, 0.0 }), Color({ 0.0, 0.0, 0.0, 0.0 }), Texture({ 0.0, 0.0 }) {};
	
	Vertex(const Vector3& pos, const Vector4& color, const Vector2& texture)
		: Pos(pos), Normal({ 0.0, 0.0, 0.0 }), Color(color), Texture(texture) {};

	Vertex(const Vector3& pos, const Vector3& normal, const Vector4& color, const Vector2& texture)
		: Pos(pos), Normal(normal), Color(color), Texture(texture) {};
};