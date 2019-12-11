#pragma once
#include <DirectXMath.h>
struct Vertex_PosTexNormTanBinorm
{
	Vertex_PosTexNormTanBinorm() {}
	Vertex_PosTexNormTanBinorm(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz, float tx, float ty, float tz,float bx, float by, float bz)
		: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz), tangent(tx,ty,tx), binormal(bx,by,bz) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texCoord;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 tangent;
	DirectX::XMFLOAT3 binormal;
};
struct Vertex_PosTexNormTan
{
	Vertex_PosTexNormTan() {}
	Vertex_PosTexNormTan(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz, float tx, float ty, float tz)
		: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz), tangent(tx,ty,tx) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texCoord;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 tangent;
};
struct Vertex_PosTexNorm
{
	Vertex_PosTexNorm() {}
	Vertex_PosTexNorm(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz)
		: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texCoord;
	DirectX::XMFLOAT3 normal;
};
struct Vertex_PosTex
{
	Vertex_PosTex() {}
	Vertex_PosTex(float x, float y, float z,
		float u, float v)
		: pos(x, y, z), texCoord(u, v) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texCoord;
};
struct Vertex_PosCol
{
	Vertex_PosCol() {}
	Vertex_PosCol(float x, float y, float z,
		float r, float g, float b)
		: pos(x, y, z), color(r, g, b) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 color;
};

