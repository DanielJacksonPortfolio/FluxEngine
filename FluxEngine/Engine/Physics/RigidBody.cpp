#include "RigidBody.h"

void RigidBody::AddForce(float x, float y, float z)
{
	AddForce(XMVectorSet(x, y, z, 1.0f));
}

void RigidBody::AddForce(XMFLOAT3 xyz)
{
	AddForce(XMLoadFloat3(&xyz));
}

void RigidBody::AddForce(XMVECTOR force)
{
	forceAccumulator += force;
	isAwake = true;
}

void RigidBody::AddForceAtPoint(XMVECTOR force, XMVECTOR point)
{
	point -= this->GetPosition();
	forceAccumulator += force;
	torqueAccumulator += XMVector3Cross(point, force);
	isAwake = true;
}

XMVECTOR RigidBody::GetPointInWorldSpace(XMVECTOR point)
{
	return XMVector3TransformCoord(point, this->transform->GetWorldMatrix());
}

void RigidBody::AddForceAtModelPoint(XMVECTOR force, XMVECTOR point)
{
	AddForceAtPoint(force, GetPointInWorldSpace(point - this->appearance->GetModel()->GetOriginVector()));////
}

void RigidBody::AddTorque(float x, float y, float z, XMVECTOR intersectLocation)
{
	AddTorque(XMVectorSet(x,y,z,0.0f), intersectLocation);
}

void RigidBody::AddTorque(XMFLOAT3 xyz, XMVECTOR intersectLocation)
{
	AddTorque(XMLoadFloat3(&xyz), intersectLocation);
}

void RigidBody::AddTorque(XMVECTOR force, XMVECTOR intersectLocation)
{
	torqueAccumulator += XMVector3Cross(intersectLocation - this->transform->GetPosition(), force);
}

//XMVECTOR RigidBody::GetLinearResultantForce()
//{
//	XMVECTOR totalForce = { 0.0f,0.0f, 0.0f, 0.0f };
//	for (XMVECTOR force : forces)
//		totalForce += force;
//	forces.clear();
//	return totalForce;
//}

//XMVECTOR RigidBody::GetAngularResultantForce()
//{
//	XMVECTOR totalTorque = { 0.0f,0.0f, 0.0f, 0.0f };
//	for (XMVECTOR torque : torques)
//		totalTorque += torque;
//	torques.clear();
//	return totalTorque;
//}

//void RigidBody::UpdateLinearPhysics(float dt)
//{
//	//XMVECTOR linearForce = GetLinearResultantForce();
//	this->linearAcceleration = forceAccumulator / mass;
//	this->linearVelocity = XMVectorSetW(this->linearVelocity + linearAcceleration * dt, 0.0f);
//
//	this->AdjustPosition((this->linearVelocity - (0.5f * linearAcceleration * dt)) * dt);
//
//	//this->linearVelocity *= (1.0f - (dt * 0.001f));
//}

void RigidBody::SetInertiaTensor(const XMMATRIX& inertiaTensor)
{
	inverseInertiaTensor = XMMatrixInverse(nullptr, inertiaTensor);
}

XMVECTOR RigidBody::GetLowestPoint()
{
	XMVECTOR lowPoint = this->GetPosition();
	if (this->GetAppearance()->GetBoundingShape() == Model::BoundingShape::SPHERE)
	{
		lowPoint = XMVectorSetY(lowPoint, XMVectorGetY(lowPoint) - this->GetAppearance()->GetScaledBoundingSphereRadius());
	}
	else if (this->GetAppearance()->GetBoundingShape() == Model::BoundingShape::OBB)
	{
		lowPoint = XMVectorSetY(lowPoint, XMVectorGetY(lowPoint + this->GetAppearance()->GetMinimumCoord(transform->GetRotationMatrix()) * transform->GetScale()));
	}
	else
	{
		lowPoint = XMVectorSetY(lowPoint, XMVectorGetY(lowPoint + this->GetAppearance()->GetMinimumCoord() * transform->GetScale()));
	}
	return lowPoint;
}
//
//void RigidBody::UpdateAngularPhysics(float dt)
//{
//	//XMVECTOR torque = GetAngularResultantForce();
//	if (this->appearance->GetModel()->boundingShape == Model::BoundingShape::SPHERE)
//		SetInertiaTensor(XMMatrixIdentity() * (0.4f * mass * this->appearance->GetModel()->GetScaledBoundingSphereRadius() * this->appearance->GetModel()->GetScaledBoundingSphereRadius(), 0.4f * mass * this->appearance->GetModel()->GetScaledBoundingSphereRadius() * this->appearance->GetModel()->GetScaledBoundingSphereRadius(), 0.4f * mass * this->appearance->GetModel()->GetScaledBoundingSphereRadius() * this->appearance->GetModel()->GetScaledBoundingSphereRadius())); //Sphere
//	else
//	{
//		float hs = std::powf(this->appearance->GetModel()->GetAABBHeight(), 2);
//		float ws = std::powf(this->appearance->GetModel()->GetAABBWidth(), 2);
//		float ds = std::powf(this->appearance->GetModel()->GetAABBDepth(), 2);
//		XMFLOAT3X3 tensor = XMFLOAT3X3();
//		tensor._11 = ((mass / 12.0f) * (hs + ds));
//		tensor._22 = ((mass / 12.0f) * (ws + ds));
//		tensor._33 = ((mass / 12.0f) * (hs + ws));
//		SetInertiaTensor(XMLoadFloat3x3(&tensor));
//	}
//	XMMATRIX inverseInertiaTensorWorld = inverseInertiaTensor * this->transform->GetRotationMatrix();
//	this->angularAcceleration = XMVector3Transform(torqueAccumulator, inverseInertiaTensorWorld);
//	this->angularAcceleration = XMVectorSetW(this->angularAcceleration, 0.0f);
//	this->angularVelocity = this->angularVelocity + (angularAcceleration * dt * 0.001f);
//	this->angularVelocity = XMVectorSetW(this->angularVelocity, 0.0f);
//
//	this->angularVelocity *= (1.0f - (dt * 0.001f));
//
//	XMQUATERNION newOrientation = (angularVelocity * dt * 0.01f);
//	newOrientation = XMQuaternionMultiply(newOrientation, this->transform->GetOrientation());
//	this->transform->SetOrientation(XMQuaternionNormalize(this->transform->GetOrientation() + (newOrientation * 0.5f)));
//
//	ClearTorqueAccumlator();
//}

void RigidBody::Integrate(float dt)
{
	XMVECTOR lastFrameAcceleration = linearAcceleration + forceAccumulator * inverseMass; 

	this->linearVelocity = XMVectorSetW(this->linearVelocity + lastFrameAcceleration * dt, 0.0f) * std::powf(linearDamping, dt);

	this->AdjustPosition(this->linearVelocity * dt);

	this->angularAcceleration = XMVectorSetW(XMVector3Transform(torqueAccumulator, inverseInertiaTensorWorld), 0.0f);
	this->angularVelocity = XMVectorSetW(this->angularVelocity + (angularAcceleration * dt * 0.001f), 0.0f) * std::powf(angularDamping, dt);
	
  	this->transform->SetOrientation(XMQuaternionNormalize(this->transform->GetOrientation() + (XMQuaternionMultiply(angularVelocity * dt, this->transform->GetOrientation()) * 0.5f)));
	//this->transform->SetOrientation(this->transform->GetOrientation() + (this->angularVelocity * dt));

	CalculateDerivedData();
	ClearAccumlators();
}

void RigidBody::CalculateDerivedData()
{
	//this->transform->SetOrientation(XMQuaternionNormalize(this->transform->GetOrientation()));
	
	this->transform->SetWorldMatrix(XMMatrixScaling(this->transform->GetScale(), this->transform->GetScale(), this->transform->GetScale()) * this->transform->GetRotationMatrix() * XMMatrixTranslationFromVector(this->transform->GetPosition()));
	this->transform->UpdateDirectionVectors();
	inverseInertiaTensorWorld = inverseInertiaTensor * this->transform->GetRotationMatrix();
}

void RigidBody::ClearAccumlators()
{
	ClearForceAccumlator();
	ClearTorqueAccumlator();
}

void RigidBody::ClearForceAccumlator()
{
	this->forceAccumulator = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
}

void RigidBody::ClearTorqueAccumlator()
{
	this->torqueAccumulator = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
}

RigidBody::RigidBody(Transform* transform, Appearance* appearance)
{
	this->transform = transform; this->appearance = appearance;
	this->appearance->SetScaledBoundingSphereRadius(this->transform->GetScale());
	if (this->appearance->GetModel()->GetBoundingShape() == Model::BoundingShape::SPHERE)
		SetInertiaTensor(XMMatrixIdentity() * (0.4f * mass * this->appearance->GetModel()->GetScaledBoundingSphereRadius() * this->appearance->GetModel()->GetScaledBoundingSphereRadius(), 0.4f * mass * this->appearance->GetModel()->GetScaledBoundingSphereRadius() * this->appearance->GetModel()->GetScaledBoundingSphereRadius(), 0.4f * mass * this->appearance->GetModel()->GetScaledBoundingSphereRadius() * this->appearance->GetModel()->GetScaledBoundingSphereRadius())); //Sphere
	else
	{
		float hs = std::powf(this->appearance->GetModel()->GetHeight(), 2);
		float ws = std::powf(this->appearance->GetModel()->GetWidth(), 2);
		float ds = std::powf(this->appearance->GetModel()->GetDepth(), 2);
		XMFLOAT3X3 tensor = XMFLOAT3X3();
		tensor._11 = ((mass / 12.0f) * (hs + ds));
		tensor._22 = ((mass / 12.0f) * (ws + ds));
		tensor._33 = ((mass / 12.0f) * (hs + ws));
		SetInertiaTensor(XMLoadFloat3x3(&tensor));
	}
	inverseInertiaTensorWorld = inverseInertiaTensor * this->transform->GetRotationMatrix();
}
RigidBody::RigidBody(const RigidBody& physics)
{
	this->frozen = physics.frozen;
	this->mass = physics.mass;
}
