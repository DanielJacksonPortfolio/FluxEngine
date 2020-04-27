#include "Gravity.h"

#include "../Collisions/CollisionHandler.h"

Gravity::Gravity(const XMVECTOR& gravity)
{
	this->gravity = gravity;
}

void Gravity::UpdateForce(RigidBody* rigidBody, float dt)
{
	if (enabled)
	{
		if (rigidBody->GetMass() == 0.0f)
		{
			ErrorLogger::Log("RigidBody has no/infinite mass.");
			return;
		}

		rigidBody->AddForce(gravity * rigidBody->GetMass());

		if (XMVectorGetY(rigidBody->GetLowestPoint()) < 0.0f)
		{
			rigidBody->AddForce(-gravity * rigidBody->GetMass());
			rigidBody->GetTransform()->AdjustPosition(0, 0.0f - XMVectorGetY(rigidBody->GetTransform()->GetPosition() + rigidBody->GetAppearance()->GetModel()->GetMinimumCoord(rigidBody->GetTransform()->GetRotationMatrix()) * rigidBody->GetTransform()->GetScale()), 0);
			rigidBody->SetLVelocity(CollisionHandler::Instance()->VectorReflection(rigidBody->GetLVelocity(), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f)) * 0.5);
		}
	}

}