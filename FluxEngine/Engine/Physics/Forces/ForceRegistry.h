#pragma once
#include "ForceGenerator.h"
#include <vector>

class ForceRegistry
{
protected:
	struct Registration
	{
		RigidBody* rigidBody = nullptr;
		ForceGenerator* generator = nullptr;
	};
	std::vector<Registration> registrations;
public:
	void AddRegistration(RigidBody* rigidBody, ForceGenerator* generator);
	void RemoveRegistration(RigidBody* rigidBody, ForceGenerator* generator);
	void ClearAll();
	void UpdateForces(float dt);
};

