#pragma once
#include "Link.h"
class Rod : public Link
{
public:
	Rod(RigidBody* rigidBody0, RigidBody* rigidBody1, float length) : Link(rigidBody0, rigidBody1), length(length){}
	virtual unsigned AddContact(Contact* contact, unsigned limit) const override;
private:	
	float length;
};

