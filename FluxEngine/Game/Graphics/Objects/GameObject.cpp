#include "GameObject.h"

const XMVECTOR& GameObject::GetPositionVector() const
{
	return this->posVector;
}

const XMFLOAT3& GameObject::GetPositionFloat3() const
{
	return this->pos;
}

const XMVECTOR& GameObject::GetRotationVector() const
{
	return this->rotVector;
}

const XMFLOAT3& GameObject::GetRotationFloat3() const
{
	return this->rot;
}

const XMVECTOR& GameObject::GetForwardVector(bool omitY)
{
	if (omitY)
		return this->forwardVector_noY;
	else
		return this->forwardVector;
}

const XMVECTOR& GameObject::GetLeftVector(bool omitY)
{
	if (omitY)
		return this->leftVector_noY;
	else
		return this->leftVector;
}

const XMVECTOR& GameObject::GetRightVector(bool omitY)
{
	if (omitY)
		return this->rightVector_noY;
	else
		return this->rightVector;
}

const XMVECTOR& GameObject::GetBackwardVector(bool omitY)
{
	if (omitY)
		return this->backwardVector_noY;
	else
		return this->backwardVector;
}

const XMFLOAT3& GameObject::GetForwardVectorFloat(bool omitY)
{
	XMFLOAT3 val;
	if (omitY)
		XMStoreFloat3(&val, this->forwardVector_noY);
	else
		XMStoreFloat3(&val, this->forwardVector);
	return val;
}

const XMFLOAT3& GameObject::GetLeftVectorFloat(bool omitY)
{
	XMFLOAT3 val;
	if (omitY)
		XMStoreFloat3(&val, this->leftVector_noY);
	else
		XMStoreFloat3(&val, this->leftVector);
	return val;
}

const XMFLOAT3& GameObject::GetRightVectorFloat(bool omitY)
{
	XMFLOAT3 val;
	if (omitY)
		XMStoreFloat3(&val, this->rightVector_noY);
	else
		XMStoreFloat3(&val, this->rightVector);
	return val;
}

const XMFLOAT3& GameObject::GetBackwardVectorFloat(bool omitY)
{
	XMFLOAT3 val;
	if (omitY)
		XMStoreFloat3(&val, this->backwardVector_noY);
	else
		XMStoreFloat3(&val, this->backwardVector);
	return val;
}

void GameObject::UpdateMatrix()
{
	assert("Update Matrix must be overrriden");
}

void GameObject::UpdateDirectionVectors()
{
	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, 0.0f);
	this->forwardVector = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
	this->backwardVector = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
	this->leftVector = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatrix);
	this->rightVector = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotationMatrix);

	XMMATRIX vecRotationMatrixNoY = XMMatrixRotationRollPitchYaw(0.0f, this->rot.y, 0.0f);
	this->forwardVector_noY = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatrixNoY);
	this->backwardVector_noY = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrixNoY);
	this->leftVector_noY = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatrixNoY);
	this->rightVector_noY = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotationMatrixNoY);
}

void GameObject::SetPosition(const XMVECTOR& pos)
{
	XMStoreFloat3(&this->pos, pos);
	this->posVector = pos;
	this->UpdateMatrix();
}

void GameObject::SetPosition(const XMFLOAT3& pos)
{
	this->pos = pos;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void GameObject::SetPosition(float x, float y, float z)
{
	this->pos = XMFLOAT3(x, y, z);
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void GameObject::AdjustPosition(const XMVECTOR& pos)
{
	this->posVector += pos;
	XMStoreFloat3(&this->pos, this->posVector);
	this->UpdateMatrix();
}

void GameObject::AdjustPosition(const XMFLOAT3& pos)
{
	this->pos.x += pos.x;
	this->pos.y += pos.y;
	this->pos.z += pos.z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void GameObject::AdjustPosition(float x, float y, float z)
{
	this->pos.x += x;
	this->pos.y += y;
	this->pos.z += z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void GameObject::SetRotation(const XMVECTOR& rot)
{
	XMStoreFloat3(&this->rot, rot);
	this->rotVector = rot;
	this->UpdateMatrix();
}

void GameObject::SetRotation(const XMFLOAT3& rot)
{
	this->rot = rot;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void GameObject::SetRotation(float x, float y, float z)
{
	this->rot = XMFLOAT3(x, y, z);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void GameObject::AdjustRotation(const XMVECTOR& rot)
{
	this->rotVector += rot;
	XMStoreFloat3(&this->rot, rot);
	this->UpdateMatrix();
}

void GameObject::AdjustRotation(const XMFLOAT3& rot)
{
	this->rot.x += rot.x;
	this->rot.y += rot.y;
	this->rot.z += rot.z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void GameObject::AdjustRotation(float x, float y, float z)
{
	this->rot.x += x;
	this->rot.y += y;
	this->rot.z += z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void GameObject::SetLookAtPos(XMFLOAT3 lookAtPos)
{
	if (lookAtPos.x == this->pos.x && lookAtPos.y == this->pos.y && lookAtPos.z == this->pos.z)
		return;

	lookAtPos.x = this->pos.x - lookAtPos.x;
	lookAtPos.y = this->pos.y - lookAtPos.y;
	lookAtPos.z = this->pos.z - lookAtPos.z;

	float pitch = 0.0f;
	if (lookAtPos.y != 0.0f)
	{
		const float distance = sqrt(pow(lookAtPos.x, 2) + pow(lookAtPos.z, 2));
		pitch = atan(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	if (lookAtPos.x != 0.0f)
	{
		yaw = atan(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0.0f)
	{
		yaw += XM_PI;
	}
	float roll = 0.0f;
	this->SetRotation(pitch, yaw, roll);
}
void GameObject::SetLookAtPos(float x, float y, float z)
{
	SetLookAtPos(XMFLOAT3(x, y, z));
}