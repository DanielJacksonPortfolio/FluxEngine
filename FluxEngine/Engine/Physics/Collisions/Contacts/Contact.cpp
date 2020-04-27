#include "Contact.h"

void Contact::Resolve(float dt)
{
	ResolveVelocity(dt);
	ResolveInterpenetration(dt);
}

float Contact::CalculateSeperatingVelocity() const
{
	XMVECTOR relativeVelocity = rigidBody[0]->GetLVelocity();
	if (rigidBody[1] != nullptr)
	{
		relativeVelocity -= rigidBody[1]->GetLVelocity();
	}
	return XMVectorGetX(XMVector3Dot(relativeVelocity,contactNormal));
}

void Contact::SetBodyData(RigidBody* body1, RigidBody* body2, float friction, float restitution)
{
	this->rigidBody[0] = body1;
	this->rigidBody[1] = body2;
	this->friction = friction;
	this->restitutionCoefficient = restitution;
}

void Contact::ResolveVelocity(float dt)
{
	float seperatingVelocity = CalculateSeperatingVelocity();
	if (seperatingVelocity >= 0) return;
	float deltaVelocity = -seperatingVelocity * restitutionCoefficient;
	XMVECTOR accelerationBasedVelocity = rigidBody[0]->GetLAcceleration();
	if (rigidBody[1] != nullptr) accelerationBasedVelocity -= rigidBody[1]->GetLAcceleration();
	float accelerationBasedSeperatingVelocity = XMVectorGetX(XMVector3Dot(accelerationBasedVelocity, contactNormal)) * dt;
	if (accelerationBasedSeperatingVelocity < 0)
	{
		deltaVelocity += restitutionCoefficient * accelerationBasedSeperatingVelocity;

		if (deltaVelocity < 0) deltaVelocity = 0;
	}
	deltaVelocity -= seperatingVelocity;

	float totalInverseMass = rigidBody[0]->GetInverseMass();
	if (rigidBody[1] != nullptr) totalInverseMass += rigidBody[1]->GetInverseMass();
	if (totalInverseMass <= 0.0f) return;
	float impulse = deltaVelocity / totalInverseMass;
	XMVECTOR impulsePerIMass = contactNormal * impulse;

	rigidBody[0]->SetLVelocity(rigidBody[0]->GetLVelocity() + impulsePerIMass * rigidBody[0]->GetInverseMass());
	if (rigidBody[1] != nullptr)
	{
		rigidBody[1]->SetLVelocity(rigidBody[1]->GetLVelocity() + impulsePerIMass * -rigidBody[1]->GetInverseMass());
	}
}

void Contact::ResolveInterpenetration(float dt)
{
	if (penetrationDepth <= 0.0f) return;

	float totalInverseMass = rigidBody[0]->GetInverseMass();
	if (rigidBody[1] != nullptr) totalInverseMass += rigidBody[1]->GetInverseMass();
	if (totalInverseMass <= 0.0f) return;

	XMVECTOR movePerIMass = contactNormal * (penetrationDepth / totalInverseMass);
	XMVECTOR rigidBodyMovement[2];
	rigidBodyMovement[0] = movePerIMass * rigidBody[0]->GetInverseMass();
	rigidBodyMovement[1] = rigidBody[1] != nullptr ? movePerIMass * rigidBody[1]->GetInverseMass() : XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	rigidBody[0]->SetPosition(rigidBody[0]->GetPosition() + rigidBodyMovement[0]);
	if (rigidBody[1] != nullptr) rigidBody[1]->SetPosition(rigidBody[1]->GetPosition() + rigidBodyMovement[1]);
}
