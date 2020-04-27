#pragma once
#include "Contact.h"
class ContactResolver
{
public:
	ContactResolver(unsigned iterations);
	ContactResolver();
	void SetIterations(unsigned iterations);
	void ResolveContacts(Contact* contacts, unsigned numContacts, float dt);
protected:
	unsigned iterations;
	unsigned iterationsUsed;
private:
};

