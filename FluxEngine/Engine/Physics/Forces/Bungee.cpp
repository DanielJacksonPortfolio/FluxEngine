#include "Bungee.h"

Bungee::Bungee(RigidBody* otherRigidBody, float springConstant, float restLength)
{
	this->otherRigidBody = otherRigidBody;
	this->springConstant = springConstant;
	this->restLength = restLength;
}

void Bungee::UpdateForce(RigidBody* rigidBody, float dt)
{
	XMVECTOR force = rigidBody->GetPosition() - otherRigidBody->GetPosition();
	float magnitude = XMVectorGetX(XMVector3Length(force));
	if (magnitude <= restLength) return;
	magnitude = springConstant * (restLength - magnitude);
	rigidBody->AddForce(XMVector3Normalize(force) * -magnitude);
}
