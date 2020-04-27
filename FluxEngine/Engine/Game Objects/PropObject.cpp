#include "PropObject.h"

bool PropObject::Init(std::vector<std::string> data, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader>& cb_vertexShader, ConstantBuffer<CB_pixelShader>& cb_pixelShader)
{
	if(!Init(XMVectorSet(std::stof(data[2]), std::stof(data[3]), std::stof(data[4]), 1.0f), XMVectorSet(std::stof(data[5]), std::stof(data[6]), std::stof(data[7]), std::stof(data[8])),data[1], device, deviceContext, cb_vertexShader, cb_pixelShader, data[0]))
		return false;
	this->transform->SetScale(std::stof(data[9]));
	this->rigidBody->SetMass(std::stof(data[10]));
	this->appearance->GetModel()->SetBoundingShape(XMMatrixRotationQuaternion(this->transform->GetOrientation()));
	return true;
}


void PropObject::Update(float dt)
{
	transform->Update(dt);
	rigidBody->Integrate(dt);
}

PropObject::PropObject(const PropObject& obj)
{
	this->transform = obj.transform;
	this->appearance = obj.appearance;
	this->rigidBody = obj.rigidBody;
}

std::string PropObject::Save()
{
	std::string output = "";
	output += name + ",";
	output += appearance->GetFilepath() + ",";
	output += std::to_string(XMVectorGetX(this->transform->GetPosition())) + ",";
	output += std::to_string(XMVectorGetY(this->transform->GetPosition())) + ",";
	output += std::to_string(XMVectorGetZ(this->transform->GetPosition())) + ",";
	output += std::to_string(XMVectorGetX(this->transform->GetOrientation())) + ",";
	output += std::to_string(XMVectorGetY(this->transform->GetOrientation())) + ",";
	output += std::to_string(XMVectorGetZ(this->transform->GetOrientation())) + ",";
	output += std::to_string(XMVectorGetW(this->transform->GetOrientation())) + ",";
	output += std::to_string(this->transform->GetScale()) + ",";
	output += std::to_string(this->rigidBody->GetMass());
	return output;
}

bool PropObject::Init(XMVECTOR pos, XMVECTOR orientation, const std::string& filepath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader>& cb_vertexShader, ConstantBuffer<CB_pixelShader>& cb_pixelShader, std::string name)
{
	GameObject::Init(pos, orientation, name);
	this->appearance = new Appearance();
	this->appearance->SetFilepath(filepath);
	if (!this->appearance->GetModel()->Init(filepath, device, deviceContext, cb_vertexShader, cb_pixelShader))
	{
		ErrorLogger::Log("Failed to load model @ " + filepath);
		return false;
	}
	this->rigidBody = new RigidBody(transform, appearance);
	return true;
}

void PropObject::Draw(const XMMATRIX& viewProjectionMatrix)
{
	if(this->appearance->GetRenderMode())
		this->appearance->GetModel()->Draw(this->transform->GetWorldMatrix(), viewProjectionMatrix);
}
void PropObject::DrawDebug(const XMMATRIX& viewProjectionMatrix, Model* sphere, Model* box)
{
	if(this->appearance->GetRenderMode())
		this->appearance->GetModel()->DrawDebug(this->transform->GetPosition(), this->transform->GetRotationMatrix(), this->transform->GetScale(), viewProjectionMatrix, sphere, box);
}

void PropObject::UpdateMatrix()
{
	this->transform->SetWorldMatrix(XMMatrixScaling(this->transform->GetScale(), this->transform->GetScale(), this->transform->GetScale()) * XMMatrixRotationQuaternion(this->transform->GetOrientation()) * XMMatrixTranslationFromVector(this->transform->GetPosition()));
	this->transform->UpdateDirectionVectors();
}
