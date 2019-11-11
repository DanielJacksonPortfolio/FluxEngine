#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include "../Shaders+Buffers/ConstantBuffer.h"
using namespace DirectX;

class Light
{
public:
	XMFLOAT3 lightColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
	float lightStrength = 1.0f;
};

