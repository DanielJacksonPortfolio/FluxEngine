#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include "../../Buffers and Shaders/ConstantBuffer.h"
using namespace DirectX;

class Light
{
public:
	XMFLOAT3& GetColor() { return this->lightColor; }
	void SetColor(XMFLOAT3 color) { this->lightColor = color; }
	float& GetStrength() { return this->lightStrength; }
	void SetStrength(float strength) { this->lightStrength = strength; }
protected:
	XMFLOAT3 lightColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
	float lightStrength = 1.0f;
};

