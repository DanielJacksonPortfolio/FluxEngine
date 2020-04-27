#include "Rod.h"

unsigned Rod::AddContact(Contact* contact, unsigned limit) const
{
	float currentLength = CurrentLength();

	if (currentLength == length)
	{
		return 0;
	}

	XMVECTOR contactNormal = XMVector3Normalize(rigidBody[1]->GetPosition() - rigidBody[0]->GetPosition());
	float penetrationDepth = 0.0f;

	if (currentLength > length)
	{
		penetrationDepth = currentLength - length;
	}
	else
	{
		contactNormal *= -1;
		penetrationDepth = length - currentLength;
	}

	contact->rigidBody[0] = rigidBody[0];
	contact->rigidBody[1] = rigidBody[1];
	contact->restitutionCoefficient = 0;
	contact->contactNormal = contactNormal;
	contact->penetrationDepth = penetrationDepth;

	return 1;

}
