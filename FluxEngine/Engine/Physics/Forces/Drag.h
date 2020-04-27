#pragma once
#include "ForceGenerator.h"

class Drag : public ForceGenerator
{
public:
	Drag(float k1, float k2);

	virtual void UpdateForce(RigidBody* rigidBody, float dt) override;
private:
	float k1 = 0.0f;
	float k2 = 0.0f;
};

