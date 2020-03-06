#pragma once
#include "GameObject.h"
#include "Physics.h"

class PropObject : public GameObject
{
public:
	PropObject() {}
	PropObject(const PropObject& obj);
	bool Init(std::vector<std::string> data, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader>& cb_vertexShader, ConstantBuffer<CB_pixelShader>& cb_pixelShader);
	bool Init(XMVECTOR position, XMVECTOR orientation, const std::string& filepath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader>& cb_vertexShader, ConstantBuffer<CB_pixelShader>& cb_pixelShader, std::string name = "");
	void Draw(const XMMATRIX& viewProjectionMatrix);
	void DrawDebug(const XMMATRIX& viewProjectionMatrix, Model* sphere, Model* box);

	Appearance* GetAppearance() { return this->appearance; }
	Physics* GetPhysics() { return this->physics; }

	std::string Save() override;
	void Update(float dt) override;
	void UpdateMatrix() override;

protected:
	Appearance* appearance = nullptr;

private:
	Physics* physics = nullptr;
};

