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

struct CB_vertexShader
{
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMMATRIX worldMatrix;
};

struct CB_pixelShader_PosTexNorm
{
	int grayscale;
};

struct CB_pixelShader //CB_pixelShader_ADS_Lighting
{
	DirectX::XMFLOAT3 ambientLightColor;			//12
	float ambientLightStrength;						//4

	DirectX::XMFLOAT3 directionalLightColor;		//12
	float directionalLightStrength;					//4

	DirectX::XMFLOAT3 directionalLightDirection;	//12
	float pointLightStrength;						//4

	DirectX::XMFLOAT3 pointLightColor;				//12
	float pointLightConstantAttenuationFactor;		//4

	DirectX::XMFLOAT3 pointLightPosition;			//12
	float pointLightLinearAttenuationFactor;		//4 

	DirectX::XMFLOAT3 cameraPosition;				//12
	float shininess;								//4

	int grayscale;									//4
	float pointLightExponentAttenuationFactor;		//4

};