#include "Spring.h"

Spring::Spring(const XMVECTOR& localConnectionPoint, RigidBody* connectedObject, const XMVECTOR& otherConnectionPoint, float springConstant, float restLength)
{
	this->localConnectionPoint = localConnectionPoint;
	this->connectedObject = connectedObject;
	this->springConstant = springConstant;
	this->restLength = restLength;
	this->otherConnectionPoint = otherConnectionPoint;
}

void Spring::UpdateForce(RigidBody* rigidBody, float dt)
{
	XMVECTOR lws = rigidBody->GetPointInWorldSpace(localConnectionPoint);
	XMVECTOR ows = rigidBody->GetPointInWorldSpace(otherConnectionPoint);

	XMVECTOR force = lws - ows;

	rigidBody->AddForceAtPoint(XMVector3Normalize(force) * std::fabs(XMVectorGetX(XMVector3Length(force)) - restLength) * -springConstant, lws);
}
