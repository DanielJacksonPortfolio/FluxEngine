#include "Drag.h"

Drag::Drag(float k1, float k2)
{
	this->k1 = k1;
	this->k2 = k2;
}

void Drag::UpdateForce(RigidBody* rigidBody, float dt)
{
	if (enabled)
	{
		XMVECTOR force = rigidBody->GetLVelocity();

		float dragCoefficient = XMVectorGetX(XMVector3Length(force));
		rigidBody->AddForce(XMVector3Normalize(force) * (k1  + k2 * dragCoefficient) * -dragCoefficient);
	}
}
