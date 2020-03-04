#include "Camera.h"

void Camera::Init(std::vector<std::string> data)
{
	linearVelocity = { 0.01f ,0.0f ,0.0f ,0.0f };
	Init(XMVectorSet(std::stof(data[1]), std::stof(data[2]), std::stof(data[3]), 1.0f), XMVectorSet(std::stof(data[4]), std::stof(data[5]), std::stof(data[6]), std::stof(data[7])), std::stof(data[8]), std::stof(data[9]), std::stof(data[10]), std::stof(data[11]), data[0]);
	SetLockedPos(std::stoi(data[12]), 0);
	SetLockedPos(std::stoi(data[13]), 1);
	SetLockedPos(std::stoi(data[14]), 2);
	//SetLookAtPos(XMFLOAT3(std::stof(data[15]), std::stof(data[16]), std::stof(data[17])));
}

void Camera::Update(float deltaTime)
{
	smoothedOrientation = XMQuaternionSlerp(smoothedOrientation, orientation, 15.0f * deltaTime);
}

void Camera::UpdateProjectionMatrix()
{
	this->projectionMatrix = XMMatrixPerspectiveFovLH((this->fov / 180.0f) * XM_PI, this->aspectRatio, this->nearPlane, this->farPlane);
}

void Camera::UpdateViewMatrix()
{
	this->viewMatrix = XMMatrixLookToLH(this->position, this->forwardVector, this->upVector);
}

void Camera::Init(XMVECTOR pos, XMVECTOR orientation, float fov, float aspectRatio, float nearPlane, float farPlane, std::string name)
{
	GameObject::Init(pos, orientation, name);
	this->fov = fov;
	this->aspectRatio = aspectRatio;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
	this->UpdateViewMatrix();
	this->UpdateProjectionMatrix();
}

std::string Camera::Save()
{
	std::string output = "";
	output += name + ",";
	output += std::to_string(XMVectorGetX(position)) + ",";
	output += std::to_string(XMVectorGetY(position)) + ",";
	output += std::to_string(XMVectorGetZ(position)) + ",";
	output += std::to_string(XMVectorGetX(orientation)) + ",";
	output += std::to_string(XMVectorGetY(orientation)) + ",";
	output += std::to_string(XMVectorGetZ(orientation)) + ",";
	output += std::to_string(XMVectorGetW(orientation)) + ",";
	output += std::to_string(fov) + ",";
	output += std::to_string(aspectRatio) + ",";
	output += std::to_string(nearPlane) + ",";
	output += std::to_string(farPlane) + ",";
	output += std::to_string(lockedPos[0]) + ",";
	output += std::to_string(lockedPos[1]) + ",";
	output += std::to_string(lockedPos[2]) + ",";
	return output;
}

float Camera::GetFov() const
{
	return this->fov;
}

float Camera::GetAspectRatio() const
{
	return this->aspectRatio;
}

float Camera::GetNearPlane() const
{
	return this->nearPlane;
}

float Camera::GetFarPlane() const
{
	return farPlane;
}

void Camera::SetFov(float fov, bool updateProjection)
{
	this->fov = fov;
	if (updateProjection)
	{
		this->UpdateProjectionMatrix();
	}
}

void Camera::SetAspectRatio(float aspectRatio, bool updateProjection)
{
	this->aspectRatio = aspectRatio;
	if (updateProjection)
	{
		UpdateProjectionMatrix();
	}
}

void Camera::SetNearPlane(float nearPlane, bool updateProjection)
{
	this->nearPlane = nearPlane;
	if (updateProjection)
	{
		UpdateProjectionMatrix();
	}
}

void Camera::SetFarPlane(float farPlane, bool updateProjection)
{
	this->farPlane = farPlane;
	if (updateProjection)
	{
		UpdateProjectionMatrix();
	}
}

const XMMATRIX& Camera::GetViewMatrix() const
{
	return this->viewMatrix;
}

const XMMATRIX& Camera::GetProjectionMatrix() const
{
	return this->projectionMatrix;
}

XMVECTOR Camera::GetSmoothedOrientation()
{
	return this->smoothedOrientation;
}