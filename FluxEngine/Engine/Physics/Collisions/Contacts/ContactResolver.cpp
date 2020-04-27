#include "ContactResolver.h"

ContactResolver::ContactResolver(unsigned iterations)
{
	SetIterations(iterations);
}

ContactResolver::ContactResolver()
{
	SetIterations(0);
}

void ContactResolver::SetIterations(unsigned iterations)
{
	this->iterations = iterations;
}

void ContactResolver::ResolveContacts(Contact* contacts, unsigned numContacts, float dt)
{
	unsigned i;

	iterationsUsed = 0;
	while (iterationsUsed < iterations)
	{
		float max = FLT_MAX;
		unsigned maxIndex = numContacts;
		for (i = 0; i < numContacts; ++i)
		{
			float sepVel = contacts[i].CalculateSeperatingVelocity();
			if (sepVel < max && (sepVel < 0 || contacts[i].penetrationDepth > 0))
			{
				max = sepVel;
				maxIndex = i;
			}
		}

		if (maxIndex == numContacts) break;

		contacts[maxIndex].Resolve(dt);
		++iterationsUsed;
	}
}
