#include "ForceRegistry.h"

void ForceRegistry::AddRegistration(RigidBody* rigidBody, ForceGenerator* generator)
{
	Registration fr;
	if (rigidBody != nullptr)
	{
		fr.rigidBody = rigidBody;
	}
	else
	{
		ErrorLogger::Log("RigidBody not valid, registration not created");
		return;
	}
	if (generator != nullptr)
	{
		fr.generator = generator;
	}
	else
	{
		ErrorLogger::Log("Generator not valid, registration not created");
		return;
	}
	for (std::vector<Registration>::iterator i = registrations.begin(); i != registrations.end(); ++i)
	{
		if (i->rigidBody == fr.rigidBody && i->generator == fr.generator)
		{
			ErrorLogger::Log("Registration already exists");
			return;
		}
	}
	registrations.push_back(fr);
}

void ForceRegistry::RemoveRegistration(RigidBody* rigidBody, ForceGenerator* generator)
{
	for (std::vector<Registration>::iterator fr = registrations.begin(); fr != registrations.end(); ++fr)
	{
		if (fr->rigidBody == rigidBody && fr->generator == generator)
		{
			registrations.erase(fr);
		}
	}
}

void ForceRegistry::ClearAll()
{
	registrations.clear();
}

void ForceRegistry::UpdateForces(float dt)
{
	for (Registration fr : registrations)
	{
		fr.generator->UpdateForce(fr.rigidBody, dt);
	}
}
