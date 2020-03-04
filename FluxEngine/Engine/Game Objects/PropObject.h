#pragma once
#include "GameObject.h"

class PropObject : public GameObject
{
public:
	void Update(float dt) override;

	PropObject() {}
	PropObject(const PropObject& obj);
	bool Init(std::vector<std::string> data, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader>& cb_vertexShader, ConstantBuffer<CB_pixelShader>& cb_pixelShader);
	bool Init(XMVECTOR position, XMVECTOR orientation, const std::string& filepath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader>& cb_vertexShader, ConstantBuffer<CB_pixelShader>& cb_pixelShader, std::string name = "");
	
	void Draw(const XMMATRIX& viewProjectionMatrix);
	void DrawDebug(const XMMATRIX& viewProjectionMatrix, Model* sphere, Model* box);

	bool RayModelIntersect(XMVECTOR rayOrigin, XMVECTOR rayDir, float& nearestIntersect, XMVECTOR& intersectLocation);

	Model& GetModel() { return this->model; }
	int& GetGrayscale() { return this->grayscale; }
	void SetGrayscale(int grayscale) { this->grayscale = grayscale; }
	bool& GetNormalMapMode() { return this->normalMap; }
	void SetNormalMapMode(bool normalMap) { this->normalMap = normalMap; }
	bool& GetSpecularMapMode() { return this->specularMap; }
	void SetSpecularMapMode(bool specularMap) { this->specularMap = specularMap; }
	bool& GetWireframeMode() { return this->wireframe; }
	void SetWireframeMode(bool wireframe) { this->wireframe = wireframe; }
	float& GetScale() { return this->scale; }
	void SetScale(float scale) { this->scale = scale; UpdateMatrix(); }
	bool& GetRenderMode() { return this->renderMode; }
	void SetRenderMode(bool mode) { this->renderMode = mode; }
	std::string Save();

	XMVECTOR& GetLAcceleration() { return this->linearAcceleration; }
	void SetLAcceleration(XMVECTOR a) { this->linearAcceleration = a; }
	XMVECTOR& GetAAcceleration() { return this->angularAcceleration; }
	void SetAAcceleration(XMVECTOR a) { this->angularAcceleration = a; }
	XMVECTOR& GetAVelocity() { return this->angularVelocity; }
	void SetAVelocity(XMVECTOR v) { this->angularVelocity = v; }
	float& GetMass() { return this->mass; }
	void SetMass(float m) { this->mass = m; }
	bool& GetFrozen() { return this->frozen; }
	void SetFrozen(bool f) { this->frozen = f; }

	void AddLinearForce(float x, float y, float z) { LForces.push_back({ x,y,z }); }
	void AddLinearForce(XMFLOAT3 xyz) { LForces.push_back({ XMLoadFloat3(&xyz) }); }
	void AddLinearForce(XMVECTOR force) { LForces.push_back(force); }
	void AddTorque(float x, float y, float z, XMVECTOR intersectLocation);
	void AddTorque(XMFLOAT3 xyz, XMVECTOR intersectLocation);
	void AddTorque(XMVECTOR force, XMVECTOR intersectLocation);
	float rotationDamping = 0.99f;

protected:
	Model model;
	void UpdateMatrix() override;
	XMMATRIX worldMatrix = XMMatrixIdentity();

private:
	XMVECTOR GetLinearResultantForce();
	XMVECTOR GetAngularResultantForce();
	void UpdateLinearPhysics(float dt);
	void UpdateAngularPhysics(float dt);

	std::vector<XMVECTOR> LForces = {};
	std::vector<XMVECTOR> torques = {};

	bool wireframe = false;
	float scale = 1.0f;
	int grayscale = 0;
	bool normalMap = false;
	bool specularMap = false;
	bool renderMode = true;

	XMVECTOR linearAcceleration = XMVECTOR();
	XMVECTOR angularAcceleration = XMVECTOR();
	XMVECTOR angularVelocity = XMVECTOR();
	float mass = 1.0f;
	bool frozen = false;

	std::string filepath;
};

