#include "Transform.h"

Transform::Transform(const Transform& transform)
{
	this->worldMatrix = transform.worldMatrix;
	this->position = transform.position;
	this->orientation = transform.orientation;
	this->rotationMatrix = transform.rotationMatrix;

	this->forwardVector = transform.forwardVector;
	this->upVector = transform.upVector;
	this->downVector = transform.downVector;
	this->leftVector = transform.leftVector;
	this->rightVector =	transform.rightVector;
	this->backwardVector = transform.backwardVector;

	this->scale = transform.scale;

	this->lockedPos[0] = transform.lockedPos[0];
	this->lockedPos[1] = transform.lockedPos[1];
	this->lockedPos[2] = transform.lockedPos[2];
	this->worldMatrix = transform.worldMatrix;
}

const XMVECTOR& Transform::GetPosition() const
{
	return this->position;
}

XMVECTOR& Transform::EditPosition()
{
	return this->position;
}

const XMQUATERNION& Transform::GetOrientation() const
{
	return orientation;
}

const XMVECTOR& Transform::GetForwardVector(bool omitY)
{
	return this->forwardVector;
}

const XMVECTOR& Transform::GetLeftVector(bool omitY)
{
	return this->leftVector;
}

const XMVECTOR& Transform::GetRightVector(bool omitY)
{
	return this->rightVector;
}

const XMVECTOR& Transform::GetBackwardVector(bool omitY)
{
	return this->backwardVector;
}
const XMVECTOR& Transform::GetUpVector(bool omitY)
{
	return this->upVector;
}
const XMVECTOR& Transform::GetDownVector(bool omitY)
{
	return this->downVector;
}

const XMMATRIX& Transform::GetWorldMatrix()
{
	return this->worldMatrix;
}

void Transform::SetWorldMatrix(const XMMATRIX& worldMatrix)
{
	this->worldMatrix = worldMatrix;
}

XMMATRIX Transform::GetRotationMatrix() const
{
	return this->rotationMatrix;
}

bool& Transform::GetLockedPos(int dimension)
{
	return this->lockedPos[dimension];
}

void Transform::UpdateDirectionVectors()
{
	this->forwardVector = XMVector3TransformNormal(DEFAULT_FORWARD_VECTOR, this->rotationMatrix);
	this->backwardVector = XMVector3TransformNormal(DEFAULT_BACKWARD_VECTOR, this->rotationMatrix);
	this->rightVector = XMVector3TransformNormal(DEFAULT_RIGHT_VECTOR, this->rotationMatrix);
	this->leftVector = XMVector3TransformNormal(DEFAULT_LEFT_VECTOR, this->rotationMatrix);
	this->upVector = XMVector3TransformNormal(DEFAULT_UP_VECTOR, this->rotationMatrix);
	this->downVector = XMVector3TransformNormal(DEFAULT_DOWN_VECTOR, this->rotationMatrix);
}

void Transform::SetPosition(const XMVECTOR& pos)
{
	this->position = pos;
}

void Transform::SetPosition(const XMFLOAT3& pos)
{
	XMFLOAT3 newPos = pos;
	if (lockedPos[0])
		newPos.x = XMVectorGetX(this->position);
	if (lockedPos[1])
		newPos.y = XMVectorGetY(this->position);
	if (lockedPos[2])
		newPos.z = XMVectorGetZ(this->position);
	this->position = XMLoadFloat3(&newPos);
}

void Transform::SetPosition(float x, float y, float z)
{
	XMFLOAT3 newPos = XMFLOAT3(x, y, z);
	if (lockedPos[0])
		newPos.x = XMVectorGetX(this->position);
	if (lockedPos[1])
		newPos.y = XMVectorGetY(this->position);
	if (lockedPos[2])
		newPos.z = XMVectorGetZ(this->position);
	this->position = XMLoadFloat3(&newPos);
}

void Transform::AdjustPosition(const XMVECTOR& pos)
{
	this->position += pos;
}

void Transform::AdjustPosition(const XMFLOAT3& pos)
{
	XMFLOAT3 newPos = pos;
	if (lockedPos[0])
		newPos.x = XMVectorGetX(this->position);
	if (lockedPos[1])
		newPos.y = XMVectorGetY(this->position);
	if (lockedPos[2])
		newPos.z = XMVectorGetZ(this->position);
	this->position += XMLoadFloat3(&newPos);
}

void Transform::AdjustPosition(float x, float y, float z)
{
	XMFLOAT3 newPos = XMFLOAT3(x, y, z);
	if (lockedPos[0])
		newPos.x = XMVectorGetX(this->position);
	if (lockedPos[1])
		newPos.y = XMVectorGetY(this->position);
	if (lockedPos[2])
		newPos.z = XMVectorGetZ(this->position);
	this->position += XMLoadFloat3(&newPos);
}


void Transform::SetOrientation(const XMQUATERNION& orientation)
{
	this->orientation = orientation;
	this->rotationMatrix = XMMatrixRotationQuaternion(this->orientation);
	this->UpdateDirectionVectors();
}

void Transform::SetOrientation(float roll, float pitch, float yaw)
{
	this->rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	this->orientation = XMQuaternionRotationMatrix(this->rotationMatrix);
	this->UpdateDirectionVectors();
}

void Transform::SetOrientation(XMMATRIX rotationMatrix)
{
	this->rotationMatrix = rotationMatrix;
	this->orientation = XMQuaternionRotationMatrix(this->rotationMatrix);
	this->UpdateDirectionVectors();
}

void Transform::RotateByAxis(XMVECTOR axis, float angle)
{
	this->rotationMatrix *= XMMatrixRotationAxis(axis, angle);
	this->orientation = XMQuaternionRotationMatrix(this->rotationMatrix);
	this->UpdateDirectionVectors();
}

void Transform::SetLockedPos(bool lockVal, int dimension)
{
	this->lockedPos[dimension] = lockVal;
}
