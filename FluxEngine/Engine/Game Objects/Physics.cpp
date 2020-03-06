#include "Physics.h"

void Physics::AddTorque(float x, float y, float z, XMVECTOR intersectLocation)
{
	XMVECTOR force = { x,y,z };
	AddTorque(force, intersectLocation);
}

void Physics::AddTorque(XMFLOAT3 xyz, XMVECTOR intersectLocation)
{
	AddTorque(XMLoadFloat3(&xyz), intersectLocation);
}

void Physics::AddTorque(XMVECTOR force, XMVECTOR intersectLocation)
{
	torques.push_back(XMVector3Cross(intersectLocation - this->transform->GetPosition(), force));
}

XMVECTOR Physics::GetLinearResultantForce()
{
	XMVECTOR totalForce = { 0.0f,0.0f, 0.0f, 0.0f };
	for (int i = 0; i < LForces.size(); ++i)
		totalForce += LForces[i];
	LForces.clear();
	return totalForce;
}


void Physics::UpdateLinearPhysics(float dt)
{
	XMVECTOR linearForce = GetLinearResultantForce();
	this->linearAcceleration = linearForce / mass;
	this->linearVelocity = XMVectorSetW(this->linearVelocity + linearAcceleration * dt, 0.0f);

	this->transform->AdjustPosition((this->linearVelocity - (0.5f * linearAcceleration * dt)) * dt);

	this->linearVelocity *= (1.0f - (dt * 0.001f));
}

void Physics::UpdateAngularPhysics(float dt)
{
	XMVECTOR torque = GetAngularResultantForce();
	XMMATRIX inertiaTensor = XMMatrixIdentity() * (0.4f * mass * this->appearance->GetModel()->objectBoundingSphereRadius * this->appearance->GetModel()->objectBoundingSphereRadius, 0.4f * mass * this->appearance->GetModel()->objectBoundingSphereRadius * this->appearance->GetModel()->objectBoundingSphereRadius, 0.4f * mass * this->appearance->GetModel()->objectBoundingSphereRadius * this->appearance->GetModel()->objectBoundingSphereRadius); //Sphere
	this->angularAcceleration = XMVector3Transform(torque, XMMatrixInverse(nullptr, inertiaTensor));
	this->angularAcceleration = XMVectorSetW(this->angularAcceleration, 0.0f);
	this->angularVelocity = this->angularVelocity + (angularAcceleration * dt * 0.001f);
	this->angularVelocity = XMVectorSetW(this->angularVelocity, 0.0f);

	this->angularVelocity *= (1.0f - (dt * 0.001f));

	XMQUATERNION newOrientation = (angularVelocity * dt * 0.01f);
	newOrientation = XMQuaternionMultiply(newOrientation, this->transform->GetOrientation());
	this->transform->SetOrientation(XMQuaternionNormalize(this->transform->GetOrientation() + (newOrientation * 0.5f)));
}

XMVECTOR Physics::GetAngularResultantForce()
{
	XMVECTOR totalTorque = { 0.0f,0.0f, 0.0f, 0.0f };
	for (int i = 0; i < torques.size(); ++i)
		totalTorque += torques[i];
	torques.clear();
	return totalTorque;
}

Physics::Physics(const Physics& physics)
{
	this->frozen = physics.frozen;
	this->mass = physics.mass;
}
