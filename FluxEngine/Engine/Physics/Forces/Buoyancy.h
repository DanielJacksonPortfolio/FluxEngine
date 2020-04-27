#pragma once
#include "ForceGenerator.h"
class Buoyancy : public ForceGenerator
{
public:
	Buoyancy(const XMVECTOR& cofb, float fluidHeight, float fluidDensity = 1000.0f);
	virtual void UpdateForce(RigidBody* rigidBody, float dt) override;
private:
	float fluidHeight = 0.0f;
	float fluidDensity = 0.0f;
	XMVECTOR centreOfBuoyancy = XMVECTOR();
};

