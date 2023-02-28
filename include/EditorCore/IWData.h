#pragma once
#include "DXTK/SimpleMath.h"

using namespace DirectX::SimpleMath;
struct IWData
{
	Vector4 Index;
	Vector4 Weight;

	IWData() : Index(Vector4()), Weight(Vector4()) {};
	IWData(Vector4 index, Vector4 weight) : Index(index), Weight(weight) {};
};