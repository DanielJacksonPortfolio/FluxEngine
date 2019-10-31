#include "Camera.h"

Camera::Camera()
{
	InitCamera(XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f));
}
Camera::Camera(XMFLOAT3 pos, XMFLOAT3 rot)
{
	InitCamera(pos,rot);
}

void Camera::InitCamera(XMFLOAT3 pos, XMFLOAT3 rot)
{
	this->pos = pos;
	this->posVector = XMLoadFloat3(&this->pos);
	this->rot = rot;
	this->rotVector = XMLoadFloat3(&this->rot);
	UpdateMatrix();
}

void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	this->projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

const XMMATRIX& Camera::GetViewMatrix() const
{
	return this->viewMatrix;
}

const XMMATRIX& Camera::GetProjectionMatrix() const
{
	return this->projectionMatrix;
}

void Camera::UpdateMatrix()
{
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z);

	XMVECTOR camTarget = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, camRotationMatrix);

	camTarget += posVector;

	XMVECTOR upDir = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, camRotationMatrix);

	this->viewMatrix = XMMatrixLookAtLH(this->posVector, camTarget, upDir);
	this->UpdateDirectionVectors();
}
