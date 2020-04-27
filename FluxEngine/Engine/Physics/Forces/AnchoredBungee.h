#pragma once
#include "ForceGenerator.h"
class AnchoredBungee : public ForceGenerator
{
public:
	AnchoredBungee(XMVECTOR anchorPoint, float springConstant, float restLength);
	virtual void UpdateForce(RigidBody* rigidBody, float dt) override;
private:
	XMVECTOR anchorPoint = XMVECTOR();
	float springConstant;
	float restLength;
};

