#pragma once
#include "GameObject.h"

class RenderableGameObject : public GameObject
{
public:
	RenderableGameObject() {}
	RenderableGameObject(const RenderableGameObject& obj);
	bool Init(std::vector<std::string> data, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader>& cb_vertexShader, ConstantBuffer<CB_pixelShader>& cb_pixelShader);
	bool Init(const std::string& filepath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader>& cb_vertexShader, ConstantBuffer<CB_pixelShader>& cb_pixelShader, std::string name = "");
	void Draw(const XMMATRIX& viewProjectionMatrix);
	void DrawDebug(const XMMATRIX& viewProjectionMatrix, Model* sphere);

	bool RayModelIntersect(XMVECTOR rayOrigin, XMVECTOR rayDir, float& nearestIntersect);

	Model& GetModel() { return this->model; }
	bool& GetMovable() { return this->movable; }
	void SetMovable(bool movable) { this->movable = movable; }
	int& GetGrayscale() { return this->grayscale; }
	void SetGrayscale(int grayscale) { this->grayscale = grayscale; }
	bool& GetNormalMapMode() { return this->normalMap; }
	void SetNormalMapMode(bool normalMap) { this->normalMap = normalMap; }
	bool& GetSpecularMapMode() { return this->specularMap; }
	void SetSpecularMapMode(bool specularMap) { this->specularMap = specularMap; }
	bool& GetWireframeMode() { return this->wireframe; }
	void SetWireframeMode(bool wireframe) { this->wireframe = wireframe; }
	float& GetScale() { return this->scale; }
	void SetScale(float scale) { this->scale = scale; }
	bool& GetRenderMode() { return this->renderMode; }
	void SetRenderMode(bool mode) { this->renderMode = mode; }
	std::string Save();

protected:
	Model model;
	void UpdateMatrix() override;
	XMMATRIX worldMatrix = XMMatrixIdentity();
private:
	bool movable = false;
	bool wireframe = false;
	int grayscale = 0;
	bool normalMap = false;
	bool specularMap = false;
	bool renderMode = true;
	float scale = 1.0f;

	std::string filepath;

};

