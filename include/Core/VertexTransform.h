#pragma once
#include "DXTK/SimpleMath.h"

using namespace DirectX::SimpleMath;
struct VertexTransform
{
	Matrix World;
	Matrix View;
	Matrix Projection;
	Vector4 Light;
};