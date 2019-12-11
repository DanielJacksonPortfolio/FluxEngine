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
const XMVECTOR& GameObject::GetUpVector(bool omitY)
{
	if (omitY)
		return this->upVector_noY;
	else
		return this->upVector;
}
const XMVECTOR& GameObject::GetDownVector(bool omitY)
{
	if (omitY)
		return this->downVector_noY;
	else
		return this->downVector;
}

const XMFLOAT3 GameObject::GetForwardVectorFloat(bool omitY)
{
	XMFLOAT3 val;
	if (omitY)
		XMStoreFloat3(&val, this->forwardVector_noY);
	else
		XMStoreFloat3(&val, this->forwardVector);
	return val;
}
;
const XMFLOAT3 GameObject::GetLeftVectorFloat(bool omitY)
{
	XMFLOAT3 val;
	if (omitY)
		XMStoreFloat3(&val, this->leftVector_noY);
	else
		XMStoreFloat3(&val, this->leftVector);
	return val;
}

const XMFLOAT3 GameObject::GetRightVectorFloat(bool omitY)
{
	XMFLOAT3 val;
	if (omitY)
		XMStoreFloat3(&val, this->rightVector_noY);
	else
		XMStoreFloat3(&val, this->rightVector);
	return val;
}

const XMFLOAT3 GameObject::GetBackwardVectorFloat(bool omitY)
{
	XMFLOAT3 val;
	if (omitY)
		XMStoreFloat3(&val, this->backwardVector_noY);
	else
		XMStoreFloat3(&val, this->backwardVector);
	return val;
}
const XMFLOAT3 GameObject::GetUpVectorFloat(bool omitY)
{
	XMFLOAT3 val;
	if (omitY)
		XMStoreFloat3(&val, this->upVector_noY);
	else
		XMStoreFloat3(&val, this->upVector);
	return val;
}
const XMFLOAT3 GameObject::GetDownVectorFloat(bool omitY)
{
	XMFLOAT3 val;
	if (omitY)
		XMStoreFloat3(&val, this->downVector_noY);
	else
		XMStoreFloat3(&val, this->downVector);
	return val;
}

void GameObject::UpdateMatrix()
{
	assert("Update Matrix must be overriden");
}

void GameObject::UpdateDirectionVectors()
{
	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, 0.0f);
	this->forwardVector = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
	this->backwardVector = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
	this->leftVector = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatrix);
	this->rightVector = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
	this->upVector = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, vecRotationMatrix);
	this->downVector = XMVector3TransformCoord(this->DEFAULT_DOWN_VECTOR, vecRotationMatrix);

	XMMATRIX vecRotationMatrixNoY = XMMatrixRotationRollPitchYaw(0.0f, this->rot.y, 0.0f);
	this->forwardVector_noY = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatrixNoY);
	this->backwardVector_noY = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrixNoY);
	this->leftVector_noY = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatrixNoY);
	this->rightVector_noY = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotationMatrixNoY);
	this->upVector_noY = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, vecRotationMatrixNoY);
	this->downVector_noY = XMVector3TransformCoord(this->DEFAULT_DOWN_VECTOR, vecRotationMatrixNoY);
}

void GameObject::SetPosition(const XMVECTOR& pos)
{
	XMFLOAT3 newPos;
	XMStoreFloat3(&newPos, pos);
	if (lockedPos[0])
		newPos.x = this->pos.x;
	if (lockedPos[1])
		newPos.y = this->pos.y;
	if (lockedPos[2])
		newPos.z = this->pos.z;
	this->pos = newPos;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void GameObject::SetPosition(const XMFLOAT3& pos)
{
	XMFLOAT3 newPos = pos;
	if (lockedPos[0])
		newPos.x = this->pos.x;
	if (lockedPos[1])
		newPos.y = this->pos.y;
	if (lockedPos[2])
		newPos.z = this->pos.z;
	this->pos = newPos;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void GameObject::SetPosition(float x, float y, float z)
{
	XMFLOAT3 newPos = XMFLOAT3(x, y, z);
	if (lockedPos[0])
		newPos.x = this->pos.x;
	if (lockedPos[1])
		newPos.y = this->pos.y;
	if (lockedPos[2])
		newPos.z = this->pos.z;
	this->pos = newPos;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void GameObject::AdjustPosition(const XMVECTOR& pos)
{
	XMFLOAT3 newPos;
	XMStoreFloat3(&newPos,pos);
	if (!lockedPos[0])
		this->pos.x += newPos.x;
	if (!lockedPos[1])
		this->pos.y += newPos.y;
	if (!lockedPos[2])
		this->pos.z += newPos.z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void GameObject::AdjustPosition(const XMFLOAT3& pos)
{
	if (!lockedPos[0])
		this->pos.x += pos.x;
	if (!lockedPos[1])
		this->pos.y += pos.y;
	if (!lockedPos[2])
		this->pos.z += pos.z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void GameObject::AdjustPosition(float x, float y, float z)
{
	if (!lockedPos[0])
		this->pos.x += x;
	if (!lockedPos[1])
		this->pos.y += y;
	if (!lockedPos[2])
		this->pos.z += z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void GameObject::SetRotation(const XMVECTOR& rot)
{
	XMFLOAT3 newRot;
	XMStoreFloat3(&newRot, rot);
	if (lockedRot[0])
		newRot.x = this->rot.x;
	if (lockedRot[1])
		newRot.y = this->rot.y;
	if (lockedRot[2])
		newRot.z = this->rot.z;
	this->rot = newRot;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void GameObject::SetRotation(const XMFLOAT3& rot)
{
	XMFLOAT3 newRot = rot;
	if (lockedRot[0])
		newRot.x = this->rot.x;
	if (lockedRot[1])
		newRot.y = this->rot.y;
	if (lockedRot[2])
		newRot.z = this->rot.z;
	this->rot = newRot;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void GameObject::SetRotation(float x, float y, float z)
{
	XMFLOAT3 newRot = XMFLOAT3(x, y, z);
	if (lockedPos[0])
		newRot.x = this->rot.x;
	if (lockedPos[1])
		newRot.y = this->rot.y;
	if (lockedPos[2])
		newRot.z = this->rot.z;
	this->rot = newRot;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void GameObject::AdjustRotation(const XMVECTOR& rot)
{
	XMFLOAT3 newRot;
	XMStoreFloat3(&newRot, rot);
	if (!lockedRot[0])
		this->rot.x += newRot.x;
	if (!lockedRot[1])
		this->rot.y += newRot.y;
	if (!lockedRot[2])
		this->rot.z += newRot.z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void GameObject::AdjustRotation(const XMFLOAT3& rot)
{
	if (!lockedRot[0])
		this->rot.x += rot.x;
	if (!lockedRot[1])
		this->rot.y += rot.y;
	if (!lockedRot[2])
		this->rot.z += rot.z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void GameObject::AdjustRotation(float x, float y, float z)
{
	if (!lockedRot[0])
		this->rot.x += x;
	if (!lockedRot[1])
		this->rot.y += y;
	if (!lockedRot[2])
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