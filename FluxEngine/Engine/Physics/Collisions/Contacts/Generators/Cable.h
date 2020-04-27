#pragma once
#include "Link.h"
class Cable : public Link
{
public:
	Cable(RigidBody* rigidBody0, RigidBody* rigidBody1, float maxLength, float restitution) : Link(rigidBody0, rigidBody1), maxLength(maxLength), resititution(restitution) {}
	virtual unsigned AddContact(Contact* contact, unsigned limit) const override;
private:
	float maxLength;
	float resititution;
};

