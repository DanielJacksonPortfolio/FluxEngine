#pragma once
#include "GameObject.h"
#include "Light.h"

#define SetDirection SetPosition

class DirectionalLight : public GameObject, public Light
{
public:
	bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	XMFLOAT3& GetDirection() { return this->pos; }
};

