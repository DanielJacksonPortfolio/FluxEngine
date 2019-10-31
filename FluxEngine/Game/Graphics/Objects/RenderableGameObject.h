#pragma once
#include "GameObject.h"

class RenderableGameObject : public GameObject
{
public:
	bool Init(const std::string& filepath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader_PosTex>& cb_vertexShader);
	void Draw(const XMMATRIX& viewProjectionMatrix);

protected:
	Model model;
	void UpdateMatrix() override;

	XMMATRIX worldMatrix = XMMatrixIdentity();
};

