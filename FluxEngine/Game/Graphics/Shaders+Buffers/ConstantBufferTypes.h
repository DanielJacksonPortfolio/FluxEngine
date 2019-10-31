#pragma once
#include <DirectXMath.h>

struct CB_vertexShader_PosCol_3D
{
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMMATRIX worldMatrix;
};

struct CB_vertexShader_PosTex
{
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMMATRIX worldMatrix;
};
