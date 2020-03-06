#pragma once
#include "Transform.h"
#include "Appearance.h"
#include <vector>
using namespace DirectX;

class Physics
{
public:
	Physics(Transform* transform, Appearance* appearance) { this->transform = transform; this->appearance = appearance; }
	Physics(const Physics& physics);
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
	XMVECTOR& GetLVelocity() { return this->linearVelocity; }
	void SetLVelocity(XMVECTOR v) { this->linearVelocity = v; }
	float& GetRotationDamping() { return rotationDamping; }
	void UpdateLinearPhysics(float dt);
	void UpdateAngularPhysics(float dt);
private:
	std::vector<XMVECTOR> LForces = {};
	std::vector<XMVECTOR> torques = {};
	XMVECTOR linearVelocity = { 0.0f ,0.0f ,0.0f ,0.0f };
	XMVECTOR linearAcceleration = XMVECTOR();
	XMVECTOR angularAcceleration = XMVECTOR();
	XMVECTOR angularVelocity = XMVECTOR();
	float rotationDamping = 0.99f;
	float mass = 1.0f;
	bool frozen = false;

	XMVECTOR GetLinearResultantForce();
	XMVECTOR GetAngularResultantForce();

	Transform* transform = nullptr;
	Appearance* appearance = nullptr;
};

