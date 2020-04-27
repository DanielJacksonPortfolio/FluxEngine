#pragma once
#include "PropObject.h"
#include "../Physics/Collisions/Contacts/Generators/ContactGenerator.h"
#include "../Physics/Collisions/Contacts/ContactResolver.h"
#include "../Physics/Forces/ForceRegistry.h"
#include  <vector>
#include  <string>

class Scene
{
public:
	Scene(unsigned maxContacts, unsigned iterations = 0);
	void StartFrame(const std::vector<std::pair<std::string,PropObject*>>& props, ForceRegistry* registry);
	void RunPhysics(float dt);
private:
	unsigned GenerateContacts();
	void Integrate(float dt);

	std::vector<PropObject*> props = {};
	Contact* contacts = nullptr;
	unsigned maxContacts;
	unsigned iterations = 0;
	std::vector<ContactGenerator*> contactGenerators = {};
	ForceRegistry* registry = nullptr;
	ContactResolver resolver;
};

