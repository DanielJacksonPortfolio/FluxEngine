#pragma once
#include "ForceGenerator.h"

class Gravity : public ForceGenerator
{
public:
	Gravity(const XMVECTOR& gravity);
	virtual void UpdateForce(RigidBody* rigidBody, float dt) override;

private:
	XMVECTOR gravity = XMVectorSet(0.0f,0.0f,0.0f,0.0f);
};

