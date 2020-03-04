#pragma once
#include "../GameObject.h"
#include "Light.h"

#define SetDirection SetPosition

class DirectionalLight : public GameObject, public Light
{
public:
	DirectionalLight() {}
	bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	XMVECTOR& GetDirection() { return this->position; }
	std::string Save();
	DirectionalLight(std::vector<std::string> data, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
};

