#pragma once
#include "ContactGenerator.h"
class Link : public ContactGenerator //CABLE
{
public:
	Link(RigidBody* rigidBody0, RigidBody* rigidBody1) { rigidBody[0] = rigidBody0; rigidBody[1] = rigidBody1;}
protected:
	RigidBody* rigidBody[2];
	float CurrentLength() const;
};

