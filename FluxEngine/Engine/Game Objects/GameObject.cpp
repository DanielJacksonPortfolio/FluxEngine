#include "GameObject.h"

bool GameObject::Init(XMVECTOR position, XMVECTOR orientation, std::string name)
{
	if (name == "")
	{
		name = "Object " + std::to_string(++OBJECT_COUNT);
	}
	this->name = name;
	this->position = position;
	this->SetOrientation(orientation);

	return true;
}

const XMVECTOR& GameObject::GetPosition() const
{
	return this->position;
}

const XMQUATERNION& GameObject::GetOrientation() const
{
	return orientation;
}

const XMMATRIX& GameObject::GetOrientationMatrix() const
{
	return XMMatrixRotationQuaternion(orientation);
}

const XMVECTOR& GameObject::GetForwardVector(bool omitY)
{
	return this->forwardVector;
}

const XMVECTOR& GameObject::GetLeftVector(bool omitY)
{
	return this->leftVector;
}

const XMVECTOR& GameObject::GetRightVector(bool omitY)
{
	return this->rightVector;
}

const XMVECTOR& GameObject::GetBackwardVector(bool omitY)
{
	return this->backwardVector;
}
const XMVECTOR& GameObject::GetUpVector(bool omitY)
{
	return this->upVector;
}
const XMVECTOR& GameObject::GetDownVector(bool omitY)
{
	return this->downVector;
}

XMMATRIX GameObject::GetRotationMatrix() const
{
	return this->rotationMatrix;
}

bool& GameObject::GetLockedPos(int dimension) 
{ 
	return this->lockedPos[dimension]; 
}

std::string& GameObject::GetName()
{
	return this->name;
}

void GameObject::UpdateMatrix()
{
	assert("Update Matrix must be overridden");
}

void GameObject::UpdateDirectionVectors()
{
	this->forwardVector = XMVector3TransformNormal(DEFAULT_FORWARD_VECTOR, this->rotationMatrix);
	this->backwardVector = XMVector3TransformNormal(DEFAULT_BACKWARD_VECTOR, this->rotationMatrix);
	this->rightVector = XMVector3TransformNormal(DEFAULT_RIGHT_VECTOR, this->rotationMatrix);
	this->leftVector = XMVector3TransformNormal(DEFAULT_LEFT_VECTOR, this->rotationMatrix);
	this->upVector = XMVector3TransformNormal(DEFAULT_UP_VECTOR, this->rotationMatrix);
	this->downVector = XMVector3TransformNormal(DEFAULT_DOWN_VECTOR, this->rotationMatrix);
}

void GameObject::SetPosition(const XMVECTOR& pos)
{
	this->position = pos;
	this->UpdateMatrix();
}

void GameObject::SetPosition(const XMFLOAT3& pos)
{
	XMFLOAT3 newPos = pos;
	if (lockedPos[0])
		newPos.x = XMVectorGetX(this->position);
	if (lockedPos[1])
		newPos.y = XMVectorGetY(this->position);
	if (lockedPos[2])
		newPos.z = XMVectorGetZ(this->position);
	this->position = XMLoadFloat3(&newPos);
	this->UpdateMatrix();
}

void GameObject::SetPosition(float x, float y, float z)
{
	XMFLOAT3 newPos = XMFLOAT3(x, y, z);
	if (lockedPos[0])
		newPos.x = XMVectorGetX(this->position);
	if (lockedPos[1])
		newPos.y = XMVectorGetY(this->position);
	if (lockedPos[2])
		newPos.z = XMVectorGetZ(this->position);
	this->position = XMLoadFloat3(&newPos);
	this->UpdateMatrix();
}

void GameObject::AdjustPosition(const XMVECTOR& pos)
{
	this->position += pos;
	this->UpdateMatrix();
}

void GameObject::AdjustPosition(const XMFLOAT3& pos)
{
	XMFLOAT3 newPos = pos;
	if (lockedPos[0])
		newPos.x = XMVectorGetX(this->position);
	if (lockedPos[1])
		newPos.y = XMVectorGetY(this->position);
	if (lockedPos[2])
		newPos.z = XMVectorGetZ(this->position);
	this->position += XMLoadFloat3(&newPos);
	this->UpdateMatrix();
}

void GameObject::AdjustPosition(float x, float y, float z)
{
	XMFLOAT3 newPos = XMFLOAT3(x, y, z);
	if (lockedPos[0])
		newPos.x = XMVectorGetX(this->position);
	if (lockedPos[1])
		newPos.y = XMVectorGetY(this->position);
	if (lockedPos[2])
		newPos.z = XMVectorGetZ(this->position);
	this->position += XMLoadFloat3(&newPos);
	this->UpdateMatrix();
}

void GameObject::SetOrientation(const XMQUATERNION& orientation)
{
	this->orientation = orientation;
	this->rotationMatrix = XMMatrixRotationQuaternion(this->orientation);
	this->UpdateDirectionVectors();
}

void GameObject::SetOrientation(float roll, float pitch, float yaw)
{
	this->rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	this->orientation = XMQuaternionRotationMatrix(this->rotationMatrix);
	this->UpdateDirectionVectors();
}

void GameObject::SetOrientation(XMMATRIX rotationMatrix)
{
	this->rotationMatrix = rotationMatrix;
	this->orientation = XMQuaternionRotationMatrix(this->rotationMatrix);
	this->UpdateDirectionVectors();
}

void GameObject::RotateByAxis(XMVECTOR axis, float angle)
{
	this->rotationMatrix *= XMMatrixRotationAxis(axis, angle);
	this->orientation = XMQuaternionRotationMatrix(this->rotationMatrix);
	this->UpdateDirectionVectors();
}

void GameObject::SetLockedPos(bool lockVal, int dimension)
{
	this->lockedPos[dimension] = lockVal;
}

void GameObject::SetName(std::string name)
{
	this->name = name;
}

//void GameObject::SlerpOrientation(const XMQUATERNION& orient, float dt)
//{
//	this->orientation = XMQuaternionNormalize(XMQuaternionSlerp(this->orientation, orient, dt));
//}

//void GameObject::SetLookAtPos(XMFLOAT3 lookAtPos)
//{
//	if (lookAtPos.x == XMVectorGetX(this->position) && lookAtPos.y == XMVectorGetY(this->position) && lookAtPos.z == XMVectorGetZ(this->position))
//		return;
//
//	lookAtPos.x = XMVectorGetX(this->position) - lookAtPos.x;
//	lookAtPos.y = XMVectorGetY(this->position) - lookAtPos.y;
//	lookAtPos.z = XMVectorGetZ(this->position) - lookAtPos.z;
//
//	float pitch = 0.0f;
//	if (lookAtPos.y != 0.0f)
//	{
//		const float distance = sqrt(pow(lookAtPos.x, 2) + pow(lookAtPos.z, 2));
//		pitch = atan(lookAtPos.y / distance);
//	}
//
//	float yaw = 0.0f;
//	if (lookAtPos.x != 0.0f)
//	{
//		yaw = atan(lookAtPos.x / lookAtPos.z);
//	}
//	if (lookAtPos.z > 0.0f)
//	{
//		yaw += XM_PI;
//	}
//	float roll = 0.0f;
//	this->SetRotation(pitch, yaw, roll);
//}

//void GameObject::SetLookAtPos(float x, float y, float z)
//{
//	SetLookAtPos(XMFLOAT3(x, y, z));
//}