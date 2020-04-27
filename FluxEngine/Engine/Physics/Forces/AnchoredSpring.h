#pragma once
#include "ForceGenerator.h"
class AnchoredSpring : public ForceGenerator
{
public:
	AnchoredSpring(const XMVECTOR& localConnectionPoint, XMVECTOR anchorPoint, float springConstant, float restLength);
	virtual void UpdateForce(RigidBody* rigidBody, float dt) override;
	void SetAnchor(XMVECTOR anchorPoint);
private:
	XMVECTOR anchorPoint;
	XMVECTOR localConnectionPoint;
	float springConstant;
	float restLength;
};

