#include "Buoyancy.h"

Buoyancy::Buoyancy(const XMVECTOR& cofb, float fluidHeight, float fluidDensity)
{
	this->centreOfBuoyancy = cofb;
	this->fluidHeight = fluidHeight;
	this->fluidDensity = fluidDensity;
}

void Buoyancy::UpdateForce(RigidBody* rigidBody, float dt)
{
	float totalSubmersionDepth = rigidBody->GetTransform()->GetScale();
	if (rigidBody->GetAppearance()->GetBoundingShape() == Model::BoundingShape::AABB)
	{
		totalSubmersionDepth *= rigidBody->GetAppearance()->GetModel()->GetHeight(); // Entire shape will be submerged when submersion depth = this;
	}
	else
	{
		totalSubmersionDepth *= rigidBody->GetAppearance()->GetModel()->GetScaledBoundingSphereRadius() * 2; // Entire shape will be submerged when submersion depth = this;
	}
	float submersionDepth = XMVectorGetY(rigidBody->GetLowestPoint());
	if (submersionDepth >= fluidHeight + totalSubmersionDepth) return;
	float volume = rigidBody->GetAppearance()->GetVolume() * rigidBody->GetTransform()->GetScale(); //// FULL should be PERCENT

	if (submersionDepth <= fluidHeight - totalSubmersionDepth) // Fully submerged
	{
		rigidBody->AddForceAtModelPoint(XMVectorSet(0.0f, fluidDensity * volume, 0.0f, 0.0f), centreOfBuoyancy);
	}
	else
	{
		rigidBody->AddForceAtModelPoint(XMVectorSet(0.0f, fluidDensity * volume * ((submersionDepth - totalSubmersionDepth - fluidHeight) / (2.0f * totalSubmersionDepth)), 0.0f, 0.0f), centreOfBuoyancy);
	}
}
