#pragma once
#include "RenderableGameObject.h"
#include "Light.h"
class PointLight : public RenderableGameObject, public Light
{
public:
	PointLight() {}
	PointLight(std::vector<std::string> data, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader>& cb_vertexShader, ConstantBuffer<CB_pixelShader>& cb_pixelShader);
	bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader>& cb_vertexShader, ConstantBuffer<CB_pixelShader>& cb_pixelShader);
	std::string Save();
	float& GetCAttenuation() { return this->constantAttenuationFactor; }
	void SetCAttenuation(float af) { this->constantAttenuationFactor = af; }
	float& GetLAttenuation() { return this->linearAttenuationFactor; }
	void SetLAttenuation(float af) { this->linearAttenuationFactor = af; }
	float& GetEAttenuation() { return this->exponentAttenuationFactor; }
	void SetEAttenuation(float af) { this->exponentAttenuationFactor = af; }
private:
	float constantAttenuationFactor = 1.0f;
	float linearAttenuationFactor = 0.09f;
	float exponentAttenuationFactor = 0.032f;
};

