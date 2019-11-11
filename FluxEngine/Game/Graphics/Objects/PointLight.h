#pragma once
#include "RenderableGameObject.h"
#include "Light.h"
class PointLight : public RenderableGameObject, public Light
{
public:
	bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader>& cb_vertexShader, ConstantBuffer<CB_pixelShader>& cb_pixelShader);

	float constantAttenuationFactor = 1.0f;
	float linearAttenuationFactor = 0.09f;
	float exponentAttenuationFactor = 0.032f;

};

