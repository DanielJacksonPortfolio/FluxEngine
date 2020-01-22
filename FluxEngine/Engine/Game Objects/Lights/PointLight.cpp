#include "PointLight.h"

bool PointLight::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader>& cb_vertexShader, ConstantBuffer<CB_pixelShader>& cb_pixelShader)
{
	if (!model.Init("Data//Objects//light.fbx", device, deviceContext, cb_vertexShader, cb_pixelShader))
		return false;

	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(XM_PIDIV2, 0.0f, 0.0f);
	this->UpdateMatrix();
	return true;
}

PointLight::PointLight(std::vector<std::string> data, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader>& cb_vertexShader, ConstantBuffer<CB_pixelShader>& cb_pixelShader)
{
	Init(device, deviceContext, cb_vertexShader, cb_pixelShader);
	SetPosition(std::stof(data[1]), std::stof(data[2]), std::stof(data[3]));
	SetColor(XMFLOAT3(std::stof(data[4]), std::stof(data[5]), std::stof(data[6])));
	SetStrength(std::stof(data[7]));
	SetCAttenuation(std::stof(data[8]));
	SetLAttenuation(std::stof(data[9]));
	SetEAttenuation(std::stof(data[10]));
}

std::string PointLight::Save()
{
	std::string output = "P,";
	output += std::to_string(pos.x) + ",";
	output += std::to_string(pos.y) + ",";
	output += std::to_string(pos.z) + ",";	
	output += std::to_string(lightColor.x) + ",";
	output += std::to_string(lightColor.y) + ",";
	output += std::to_string(lightColor.z) + ",";	
	output += std::to_string(lightStrength) + ",";
	output += std::to_string(constantAttenuationFactor) + ",";
	output += std::to_string(linearAttenuationFactor) + ",";
	output += std::to_string(exponentAttenuationFactor);
	return output;
}
