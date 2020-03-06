#include "GameObject.h"

bool GameObject::Init(XMVECTOR position, XMVECTOR orientation, std::string name)
{
	if (name == "")
	{
		name = "Object " + std::to_string(++OBJECT_COUNT);
	}
	this->name = name;
	this->transform = new Transform();
	this->transform->SetPosition(position);
	this->transform->SetOrientation(orientation);
	this->UpdateMatrix();
	return true;
}

std::string& GameObject::GetName()
{
	return this->name;
}

void GameObject::SetName(std::string name)
{
	this->name = name;
}

void GameObject::UpdateMatrix()
{
	assert("Update Matrix must be overridden");
}

void GameObject::Update(float dt)
{
	assert("Update must be overridden");
}

std::string GameObject::Save()
{
	assert("Save must be overridden");
	return "";
}
