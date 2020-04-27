#pragma once
#include "ForceGenerator.h"
class Bungee : public ForceGenerator
{
public:
	Bungee(RigidBody* otherRigidBody, float springConstant, float restLength);
	virtual void UpdateForce(RigidBody* rigidBody, float dt) override;
private:
	RigidBody* otherRigidBody = nullptr;
	float springConstant;
	float restLength;
};

