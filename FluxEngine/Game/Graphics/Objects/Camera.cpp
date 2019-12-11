#include "Camera.h"

Camera::Camera()
{
	InitCamera(XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f));
}

Camera::Camera(std::vector<std::string> data)
{
	SetName(data[0]);
	SetFOV(std::stof(data[1]));
	SetLookAtMode(std::stof(data[2]));
	InitCamera(XMFLOAT3(std::stof(data[3]), std::stof(data[4]), std::stof(data[5])), XMFLOAT3(std::stof(data[6]), std::stof(data[7]), std::stof(data[8])));
	SetLockedPos(std::stoi(data[9]), 0);
	SetLockedPos(std::stoi(data[10]), 1);
	SetLockedPos(std::stoi(data[11]), 2);
	SetLockedRot(std::stoi(data[12]), 0);
	SetLockedRot(std::stoi(data[13]), 1);
	SetLockedRot(std::stoi(data[14]), 2);
	SetLookAtPos(XMFLOAT3(std::stof(data[15]), std::stof(data[16]), std::stof(data[17])));
}

Camera::Camera(XMFLOAT3 pos, XMFLOAT3 rot, std::string name)
{
	objectName = name;
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

std::string Camera::Save()
{
	std::string output = "";
	output += objectName + ",";
	output += std::to_string(fov) + ",";
	output += std::to_string(lookAtMode) + ",";
	output += std::to_string(pos.x) + ",";
	output += std::to_string(pos.y) + ",";
	output += std::to_string(pos.z) + ",";
	output += std::to_string(rot.x) + ",";
	output += std::to_string(rot.y) + ",";
	output += std::to_string(rot.z) + ",";
	output += std::to_string(lockedPos[0]) + ",";
	output += std::to_string(lockedPos[1]) + ",";
	output += std::to_string(lockedPos[2]) + ",";
	output += std::to_string(lockedRot[0]) + ",";
	output += std::to_string(lockedRot[1]) + ",";
	output += std::to_string(lockedRot[2]) + ",";
	output += std::to_string(lookAtPos.x) + ",";
	output += std::to_string(lookAtPos.y) + ",";
	output += std::to_string(lookAtPos.z);
	return output;
}

void Camera::SetProjectionValues(float aspectRatio, float nearZ, float farZ)
{
	float fovRadians = (fov / 360.0f) * XM_2PI;
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

	this->UpdateDirectionVectors();
	//if(!lookAtMode)
	this->viewMatrix = XMMatrixLookAtLH(this->posVector, camTarget, upVector);
	//else
	//	this->viewMatrix = XMMatrixLookAtLH(this->lookAtVector, camTarget, upVector);
}
