#pragma once
#include "GameObject.h"

class RenderableGameObject : public GameObject
{
public:
	bool Init(const std::string& filepath, float scale, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader>& cb_vertexShader, ConstantBuffer<CB_pixelShader>& cb_pixelShader);
	void Draw(const XMMATRIX& viewProjectionMatrix);

protected:
	Model model;
	void UpdateMatrix() override;

	XMMATRIX worldMatrix = XMMatrixIdentity();
};

