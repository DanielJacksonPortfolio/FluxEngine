#include "Cable.h"

unsigned Cable::AddContact(Contact* contact, unsigned limit) const
{
	float length = CurrentLength();
	if (length > maxLength)
	{
		return 0;
	}

	contact->rigidBody[0] = rigidBody[0];
	contact->rigidBody[1] = rigidBody[1];
	contact->restitutionCoefficient = resititution;
	contact->contactNormal = XMVector3Normalize(rigidBody[1]->GetPosition() - rigidBody[0]->GetPosition());
	contact->penetrationDepth = length - maxLength;

	return 1;
}
