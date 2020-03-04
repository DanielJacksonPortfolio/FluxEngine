#include "PropObject.h"

bool PropObject::Init(std::vector<std::string> data, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader>& cb_vertexShader, ConstantBuffer<CB_pixelShader>& cb_pixelShader)
{
	if(!Init(XMVectorSet(std::stof(data[2]), std::stof(data[3]), std::stof(data[4]), 1.0f), XMVectorSet(std::stof(data[5]), std::stof(data[6]), std::stof(data[7]), std::stof(data[8])),data[1], device, deviceContext, cb_vertexShader, cb_pixelShader, data[0]))
		return false;
	SetScale(std::stof(data[9]));
	this->model.SetBoundingShape(XMMatrixRotationQuaternion(this->orientation));
	return true;
}

void PropObject::AddTorque(float x, float y, float z, XMVECTOR intersectLocation)
{
	XMVECTOR force = { x,y,z };
	AddTorque(force, intersectLocation);
}

void PropObject::AddTorque(XMFLOAT3 xyz, XMVECTOR intersectLocation)
{
	AddTorque(XMLoadFloat3(&xyz), intersectLocation);
}

void PropObject::AddTorque(XMVECTOR force, XMVECTOR intersectLocation)
{
	torques.push_back(XMVector3Cross(intersectLocation - this->position, force));
}

XMVECTOR PropObject::GetLinearResultantForce()
{
	XMVECTOR totalForce = { 0.0f,0.0f, 0.0f, 0.0f };
	for (int i = 0; i < LForces.size(); ++i)
		totalForce += LForces[i];
	LForces.clear();
	return totalForce;
}

XMVECTOR PropObject::GetAngularResultantForce()
{
	XMVECTOR totalTorque = { 0.0f,0.0f, 0.0f, 0.0f };
	for (int i = 0; i < torques.size(); ++i)
		totalTorque += torques[i];
	torques.clear();
	return totalTorque;
}

void PropObject::UpdateLinearPhysics(float dt)
{
	XMVECTOR linearForce = GetLinearResultantForce();
	this->linearAcceleration = linearForce / mass;
	this->linearVelocity = XMVectorSetW(this->linearVelocity + linearAcceleration * dt, 0.0f);

	this->position = this->position + (this->linearVelocity - (0.5f * linearAcceleration * dt)) * dt;
	UpdateMatrix();

	this->linearVelocity *= (1.0f - (dt * 0.001f));
}

void PropObject::UpdateAngularPhysics(float dt)
{
	XMVECTOR torque = GetAngularResultantForce();
	XMMATRIX inertiaTensor = XMMatrixIdentity() * (0.4f * mass * this->model.objectBoundingSphereRadius * this->model.objectBoundingSphereRadius, 0.4f * mass * this->model.objectBoundingSphereRadius * this->model.objectBoundingSphereRadius, 0.4f * mass * this->model.objectBoundingSphereRadius * this->model.objectBoundingSphereRadius); //Sphere
	this->angularAcceleration = XMVector3Transform(torque,XMMatrixInverse(nullptr, inertiaTensor));
	this->angularAcceleration = XMVectorSetW(this->angularAcceleration, 0.0f);
	this->angularVelocity = this->angularVelocity + (angularAcceleration * dt * 0.001f);
	this->angularVelocity = XMVectorSetW(this->angularVelocity, 0.0f);

	this->angularVelocity *= (1.0f - (dt * 0.001f));

	XMQUATERNION newOrientation = (angularVelocity * dt * 0.01f);
	newOrientation = XMQuaternionMultiply(newOrientation, this->orientation);
	this->orientation += (newOrientation * 0.5f);
	this->orientation = XMQuaternionNormalize(this->orientation);
}

void PropObject::Update(float dt)
{
	UpdateLinearPhysics(dt);
	UpdateAngularPhysics(dt);
}

PropObject::PropObject(const PropObject& obj)
{
	this->model = obj.model;
	this->worldMatrix = obj.worldMatrix;
	this->frozen = obj.frozen;
	this->wireframe = obj.wireframe;
	this->grayscale = obj.grayscale;
	this->normalMap = obj.normalMap;
	this->specularMap = obj.specularMap;
	this->renderMode = obj.renderMode;
	this->scale = obj.scale;
	this->filepath = obj.filepath;
}

std::string PropObject::Save()
{
	std::string output = "";
	output += name + ",";
	output += filepath + ",";
	output += std::to_string(XMVectorGetX(position)) + ",";
	output += std::to_string(XMVectorGetY(position)) + ",";
	output += std::to_string(XMVectorGetZ(position)) + ",";
	output += std::to_string(XMVectorGetX(orientation)) + ",";
	output += std::to_string(XMVectorGetY(orientation)) + ",";
	output += std::to_string(XMVectorGetZ(orientation)) + ",";
	output += std::to_string(XMVectorGetW(orientation)) + ",";
	output += std::to_string(scale);
	return output;
}

bool PropObject::Init(XMVECTOR pos, XMVECTOR orientation, const std::string& filepath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader>& cb_vertexShader, ConstantBuffer<CB_pixelShader>& cb_pixelShader, std::string name)
{
	GameObject::Init(pos, orientation, name);
	this->filepath = filepath;
	if (!model.Init(filepath, device, deviceContext, cb_vertexShader, cb_pixelShader))
	{
		ErrorLogger::Log("Failed to load model @ " + filepath);
		return false;
	}
	return true;
}

void PropObject::Draw(const XMMATRIX& viewProjectionMatrix)
{
	if(renderMode)
		model.Draw(this->worldMatrix, viewProjectionMatrix);
}
void PropObject::DrawDebug(const XMMATRIX& viewProjectionMatrix, Model* sphere, Model* box)
{
	if(renderMode)
		model.DrawDebug(this->position, scale, viewProjectionMatrix, sphere, box);
}

void PropObject::UpdateMatrix()
{
	this->worldMatrix = XMMatrixScaling(scale,scale,scale) * XMMatrixRotationQuaternion(this->orientation) * XMMatrixTranslationFromVector(this->position);
	this->UpdateDirectionVectors();
}

bool PropObject::RayModelIntersect(XMVECTOR rayOrigin, XMVECTOR rayDir, float& nearestIntersect, XMVECTOR& intersectLocation)
{
	UpdateMatrix();
	this->model.SetBoundingShape(XMMatrixRotationQuaternion(this->orientation));
	return model.RayModelIntersect(worldMatrix, position, scale, rayOrigin, rayDir, nearestIntersect, intersectLocation);
}