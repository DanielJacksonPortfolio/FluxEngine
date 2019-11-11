#include "DirectionalLight.h"

bool DirectionalLight::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateMatrix();
	return true;
}
