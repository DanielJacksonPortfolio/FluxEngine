#include "AnchoredSpring.h"

AnchoredSpring::AnchoredSpring(const XMVECTOR& localConnectionPoint, XMVECTOR anchorPoint, float springConstant, float restLength)
{
	this->localConnectionPoint = localConnectionPoint;
	this->anchorPoint = anchorPoint;
	this->springConstant = springConstant;
	this->restLength = restLength;
}

void AnchoredSpring::UpdateForce(RigidBody* rigidBody, float dt)
{
	XMVECTOR lws = rigidBody->GetPointInWorldSpace(localConnectionPoint);
	XMVECTOR force = lws - anchorPoint;
	float currentLength = XMVectorGetX(XMVector3Length(force));
	force = XMVector3Normalize(force);
	rigidBody->AddForceAtPoint(force * (restLength - currentLength ) * springConstant, lws);
}

void AnchoredSpring::SetAnchor(XMVECTOR anchorPoint)
{
	this->anchorPoint = anchorPoint;
}
