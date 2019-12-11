#pragma once
#include <DirectXMath.h>

struct CB_vertexShader
{
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMMATRIX worldMatrix;
	float elapsedTime;
};

#define MAX_LIGHTS 256
struct CB_pixelShader //CB_pixelShader_Specular
{
	DirectX::XMFLOAT4 pointLightColor[MAX_LIGHTS];
	DirectX::XMFLOAT4 pointLightPosition[MAX_LIGHTS];
	DirectX::XMFLOAT4 pointLightFactors[MAX_LIGHTS];

	DirectX::XMFLOAT4 directionalLightColor[MAX_LIGHTS];
	DirectX::XMFLOAT4 directionalLightDirection[MAX_LIGHTS];
	DirectX::XMFLOAT4 directionalLightStrength[MAX_LIGHTS];

	DirectX::XMFLOAT3 ambientLightColor;
	float ambientLightStrength;

	DirectX::XMFLOAT3 cameraPosition;
	float shininess;

	int grayscale;
	BOOL normalMap;
	BOOL specularMap;
	int numPLights;

	int numDLights;
};