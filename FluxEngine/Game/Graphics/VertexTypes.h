#pragma once
#include <DirectXMath.h>
struct Vertex_PTN
{
	Vertex_PTN(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz)
		: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texCoord;
	DirectX::XMFLOAT3 normal;
};
struct Vertex_PT
{
	Vertex_PT(float x, float y, float z,
		float u, float v)
		: pos(x, y, z), texCoord(u, v) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texCoord;
};
struct Vertex_PC
{
	Vertex_PC(float x, float y, float z,
		float r, float g, float b)
		: pos(x, y, z), color(r, g, b) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 color;
};

