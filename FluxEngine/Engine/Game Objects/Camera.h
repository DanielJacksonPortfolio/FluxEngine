#pragma once
#include "GameObject.h"
class Camera : public GameObject
{
public:	
	Camera() {}
	void Init(XMVECTOR position, XMVECTOR orientation, float fov, float aspectRatio, float nearPlane, float farPlane, std::string name = "Camera");
	void Init(std::vector<std::string> data);

	const XMMATRIX& GetViewMatrix() const;
	const XMMATRIX& GetProjectionMatrix() const;

	float& GetFOV() { return this->fov; }
	void SetFOV(float fov) { this->fov = fov; }

	//XMFLOAT3& GetLookAtPos() { return lookAtPos; }
	//void SetLookAtPos() { if (lookAtMode) GameObject::SetLookAtPos(lookAtPos); }
	//void SetLookAtPos(XMFLOAT3 lookAtPos) { this->lookAtPos = lookAtPos;  if(lookAtMode) GameObject::SetLookAtPos(lookAtPos); }

	float GetFov() const;
	float GetAspectRatio() const;
	float GetNearPlane() const;
	float GetFarPlane() const;

	void SetFov(float fov, bool updateProjection = true);
	void SetAspectRatio(float aspectRatio, bool updateProjection = true);
	void SetNearPlane(float nearPlane, bool updateProjection = true);
	void SetFarPlane(float farPlane, bool updateProjection = true);

	void Update(float deltaTime) override;

	void UpdateProjectionMatrix();
	void UpdateViewMatrix();

	XMVECTOR GetSmoothedOrientation();
	std::string Save();
private:
	XMMATRIX viewMatrix = {};
	XMMATRIX projectionMatrix = {};
	float fov = 90.0f;	
	float aspectRatio = 0.0f;
	float nearPlane = 0.0f;
	float farPlane = 0.0f;

	XMVECTOR smoothedOrientation = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
};

