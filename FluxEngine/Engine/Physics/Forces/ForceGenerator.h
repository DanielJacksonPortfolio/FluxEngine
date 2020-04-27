#pragma once
#include "../../../Tools/ErrorLogger.h"
#include "../RigidBody.h"

class ForceGenerator
{
public:
	void Enable() { enabled = true; }
	void Disable() { enabled = false; }
	void Toggle() { enabled = !enabled; }
	virtual void UpdateForce(RigidBody* rigidBody, float dt) = 0;
protected:
	bool enabled = true;
};

