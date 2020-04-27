#include "Scene.h"

Scene::Scene(unsigned maxContacts, unsigned iterations)
{
	this->maxContacts = maxContacts;
	this->iterations = iterations;
}

void Scene::StartFrame(const std::vector<std::pair<std::string, PropObject*>>& props, ForceRegistry* registry)
{
	this->props.clear();
	for (std::pair<std::string, PropObject*> object : props)
	{
		this->props.push_back(object.second);
		//this->props.back()->GetRigidBody()->ClearAccumlators();
		//this->props.back()->GetRigidBody()->CalculateDerivedData();
	}
	this->registry = registry;
}

unsigned Scene::GenerateContacts()
{
	unsigned limit = maxContacts;
	Contact* nextContact = contacts;

	for (ContactGenerator* generator : contactGenerators)
	{
		unsigned used = generator->AddContact(nextContact, limit);
		limit -= used;
		nextContact += used;

		if (limit <= 0) break;
	}

	return maxContacts - limit;
}

void Scene::Integrate(float dt)
{
	for (PropObject* prop : props)
	{
		prop->Update(dt);
	}
}

void Scene::RunPhysics(float dt)
{
	registry->UpdateForces(dt);
	Integrate(dt);
	//unsigned usedContacts = GenerateContacts();
	//if (usedContacts)
	//{
	//	if (iterations == 0)
	//	{
	//		resolver.SetIterations(usedContacts * 2);
	//	}
	//	resolver.ResolveContacts(contacts, usedContacts, dt);
	//}
}
