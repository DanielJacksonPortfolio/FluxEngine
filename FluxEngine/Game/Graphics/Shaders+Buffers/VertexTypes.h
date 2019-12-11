#pragma once
#include <DirectXMath.h>
struct Vertex_PosTexNormTanBinorm
{
	Vertex_PosTexNormTanBinorm() {}
	Vertex_PosTexNormTanBinorm(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz, float tx, float ty, float tz,float bx, float by, float bz)
		: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz), tangent(tx,ty,tx), binormal(bx,by,bz) {}

	DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0.0f,0.0f,0.0f);
	DirectX::XMFLOAT2 texCoord = DirectX::XMFLOAT2(0.0f,0.0f);
	DirectX::XMFLOAT3 normal = DirectX::XMFLOAT3(0.0f,0.0f,0.0f);
	DirectX::XMFLOAT3 tangent = DirectX::XMFLOAT3(0.0f,0.0f,0.0f);
	DirectX::XMFLOAT3 binormal = DirectX::XMFLOAT3(0.0f,0.0f,0.0f);
};
