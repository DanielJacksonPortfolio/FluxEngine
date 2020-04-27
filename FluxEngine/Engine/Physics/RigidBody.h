#pragma once
#include "../Game Objects/Transform.h"
#include "../Game Objects/Appearance.h"
#include <vector>
using namespace DirectX;

class RigidBody
{
public:
	RigidBody(Transform* transform, Appearance* appearance);
	RigidBody(const RigidBody& physics);
	Appearance* GetAppearance() { return this->appearance; }
	Transform* GetTransform() { return this->transform; }

	float GetMass() { return this->mass; }
	float GetInverseMass() { return this->inverseMass; }
	void SetMass(float m) { this->mass = m; this->inverseMass = 1.0f / m; }
	void SetInverseMass(float m) { this->mass = 1.0f / m; this->inverseMass = m; }
	XMVECTOR GetPosition() { return this->transform->GetPosition(); }
	void SetPosition(XMVECTOR pos) { return this->transform->SetPosition(pos); }
	void AdjustPosition(XMVECTOR pos) { return this->transform->AdjustPosition(pos); }
	XMVECTOR GetLVelocity() { return this->linearVelocity; }
	void SetLVelocity(XMVECTOR v) { this->linearVelocity = v; }
	XMVECTOR GetLAcceleration() { return this->linearAcceleration; }
	void SetLAcceleration(XMVECTOR a) { this->linearAcceleration = a; }

	XMVECTOR GetAVelocity() { return this->angularVelocity; }
	void SetAVelocity(XMVECTOR v) { this->angularVelocity = v; }
	XMVECTOR GetAAcceleration() { return this->angularAcceleration; }
	void SetAAcceleration(XMVECTOR a) { this->angularAcceleration = a; }
	void SetInertiaTensor(const XMMATRIX& inertiaTensor);

	XMVECTOR GetLowestPoint();

	void AddForce(float x, float y, float z);
	void AddForce(XMFLOAT3 xyz);
	void AddForce(XMVECTOR force);

	void AddForceAtPoint(XMVECTOR force, XMVECTOR point);

	XMVECTOR GetPointInWorldSpace(XMVECTOR point);

	void AddForceAtModelPoint(XMVECTOR force, XMVECTOR point);

	void AddTorque(float x, float y, float z, XMVECTOR intersectLocation);
	void AddTorque(XMFLOAT3 xyz, XMVECTOR intersectLocation);
	void AddTorque(XMVECTOR force, XMVECTOR intersectLocation);

	void Integrate(float dt);
	bool GetFrozen() { return this->frozen; }
	void SetFrozen(bool f) { this->frozen = f; }
	//void UpdateLinearPhysics(float dt);
	//void UpdateAngularPhysics(float dt);
	void ClearAccumlators();
	void CalculateDerivedData();

private:

	void ClearForceAccumlator();
	void ClearTorqueAccumlator();

	Transform* transform = nullptr;
	Appearance* appearance = nullptr;

	float mass = 1.0f;
	float inverseMass = 1.0f;
	float linearDamping = 1.0f;
	float angularDamping = 0.95f;
	XMVECTOR linearVelocity = { 0.0f ,0.0f ,0.0f ,0.0f };
	XMVECTOR linearAcceleration = XMVECTOR();
	XMVECTOR forceAccumulator = {};

	XMVECTOR angularAcceleration = XMVECTOR();
	XMVECTOR angularVelocity = XMVECTOR();
	XMVECTOR torqueAccumulator = {};
	XMMATRIX inverseInertiaTensor;
	XMMATRIX inverseInertiaTensorWorld;

	bool frozen = false;
	bool isAwake = false;
};

