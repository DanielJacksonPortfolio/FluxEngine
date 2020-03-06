#include "DirectionalLight.h"

bool DirectionalLight::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	GameObject::Init(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
	return true;
}

DirectionalLight::DirectionalLight(std::vector<std::string> data, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	Init(device, deviceContext);
	transform->SetPosition(std::stof(data[1]), std::stof(data[2]), std::stof(data[3]));
	SetColor(XMFLOAT3(std::stof(data[4]), std::stof(data[5]), std::stof(data[6])));
	SetStrength(std::stof(data[7]));
	this->UpdateMatrix();
}

std::string DirectionalLight::Save()
{
	std::string output = "D,";
	output += std::to_string(XMVectorGetX(transform->GetPosition())) + ",";
	output += std::to_string(XMVectorGetY(transform->GetPosition())) + ",";
	output += std::to_string(XMVectorGetZ(transform->GetPosition())) + ",";
	output += std::to_string(lightColor.x) + ",";
	output += std::to_string(lightColor.y) + ",";
	output += std::to_string(lightColor.z) + ",";
	output += std::to_string(lightStrength) + ",";
	return output;
}
