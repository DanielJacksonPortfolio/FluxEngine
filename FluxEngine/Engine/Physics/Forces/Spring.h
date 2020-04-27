#pragma once
#include "ForceGenerator.h"

class Spring : public ForceGenerator
{
public:
	Spring(const XMVECTOR& localConnectionPoint, RigidBody* connectedObject, const XMVECTOR& otherConnectionPoint, float springConstant, float restLength);
	virtual void UpdateForce(RigidBody* rigidBody, float dt) override;
private:
	XMVECTOR localConnectionPoint;
	XMVECTOR otherConnectionPoint;

	RigidBody* connectedObject = nullptr;
	float springConstant;
	float restLength;
};

