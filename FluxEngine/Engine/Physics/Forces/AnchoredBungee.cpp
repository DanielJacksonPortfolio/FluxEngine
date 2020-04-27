#include "AnchoredBungee.h"

AnchoredBungee::AnchoredBungee(XMVECTOR anchorPoint, float springConstant, float restLength)
{
	this->anchorPoint = anchorPoint;
	this->springConstant = springConstant;
	this->restLength = restLength;
}

void AnchoredBungee::UpdateForce(RigidBody* rigidBody, float dt)
{
	XMVECTOR force = rigidBody->GetPosition() - anchorPoint;
	float magnitude = XMVectorGetX(XMVector3Length(force));
	if (magnitude <= restLength) return;
	magnitude = springConstant * (restLength - magnitude);
	rigidBody->AddForce(XMVector3Normalize(force) * -magnitude);
}
