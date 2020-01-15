#pragma once
#include "GameObject.h"
class Camera : public GameObject
{
public:
	Camera();
	Camera(std::vector<std::string> data);
	Camera(XMFLOAT3 pos, XMFLOAT3 rot, std::string name = "Camera");
	void SetProjectionValues(float aspectRatio, float nearZ, float farZ);

	const XMMATRIX& GetViewMatrix() const;
	const XMMATRIX& GetProjectionMatrix() const;

	float& GetFOV() { return this->fov; }
	void SetFOV(float fov) { this->fov = fov; }
	bool& GetLookAtMode() { return this->lookAtMode; }
	void SetLookAtMode(bool lookAtMode) { this->lookAtMode = lookAtMode; }

	XMFLOAT3& GetLookAtPos() { return lookAtPos; }
	void SetLookAtPos() { GameObject::SetLookAtPos(lookAtPos); }
	void SetLookAtPos(XMFLOAT3 lookAtPos) { this->lookAtPos = lookAtPos;  if(lookAtMode) GameObject::SetLookAtPos(lookAtPos); }

	std::string Save();
private:
	void UpdateMatrix() override;
	void InitCamera(XMFLOAT3 pos, XMFLOAT3 rot);
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
	float fov = 90.0f;
	bool lookAtMode = false;
	XMFLOAT3 lookAtPos = XMFLOAT3(0.0f, 0.0f, 0.0f);

};

