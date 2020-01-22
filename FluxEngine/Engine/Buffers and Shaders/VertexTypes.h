#pragma once
#include <DirectXMath.h>
struct Vertex_PosTexNormTan
{
	Vertex_PosTexNormTan() {}
	Vertex_PosTexNormTan(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz, float tx, float ty, float tz)
		: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz), tangent(tx,ty,tx) {}

	DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0.0f,0.0f,0.0f);
	DirectX::XMFLOAT2 texCoord = DirectX::XMFLOAT2(0.0f,0.0f);
	DirectX::XMFLOAT3 normal = DirectX::XMFLOAT3(0.0f,0.0f,0.0f);
	DirectX::XMFLOAT3 tangent = DirectX::XMFLOAT3(0.0f,0.0f,0.0f);
};
