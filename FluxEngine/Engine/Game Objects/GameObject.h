#pragma once
#include "Transform.h"
#include <vector>
#include <string>

static int OBJECT_COUNT = 0;

class GameObject
{
public:
	Transform* GetTransform() { return this->transform; }
	virtual bool Init(XMVECTOR position, XMVECTOR orientation, std::string name = "");
	virtual void Update(float dt);
	virtual std::string Save();

	std::string& GetName();
	void SetName(std::string name);


protected:
	virtual void UpdateMatrix();

	Transform* transform = nullptr;

	std::string name = "";
};
