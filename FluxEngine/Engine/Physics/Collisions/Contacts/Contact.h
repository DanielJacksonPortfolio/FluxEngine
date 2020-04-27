#pragma once
#include "../../RigidBody.h"

class Contact
{
public:
	void Resolve(float dt);
	float CalculateSeperatingVelocity() const;
	//Contact(RigidBody* rigidBody0, RigidBody* rigidBody1, float friction, float restitutionCoefficient, float penetrationDepth, XMVECTOR contactNormal, XMVECTOR contactLocation) : friction(friction), restitutionCoefficient(restitutionCoefficient), penetrationDepth(penetrationDepth), contactNormal(contactNormal), contactLocation(contactLocation) { rigidBody[0] = rigidBody0; rigidBody[1] = rigidBody1; }

	RigidBody* rigidBody[2] = { nullptr,nullptr };
	float restitutionCoefficient = 0.0f;
	float penetrationDepth = 0.0f;
	float friction = 0.0f;
	XMVECTOR contactNormal = XMVECTOR();
	XMVECTOR contactLocation = XMVECTOR();
	void SetBodyData(RigidBody* body1, RigidBody* body2, float friction, float restitution);
private:
	void ResolveVelocity(float dt);
	void ResolveInterpenetration(float dt);



};

