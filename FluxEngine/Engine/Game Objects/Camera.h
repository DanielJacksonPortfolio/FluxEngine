#pragma once
#include "GameObject.h"

class Camera : public GameObject
{
public:	
	Camera() {}
	void Init(XMVECTOR position, XMVECTOR orientation, float fov, float aspectRatio, float nearPlane, float farPlane, std::string name = "Camera");
	void Update(float dt) override;
	void Init(std::vector<std::string> data);

	const XMMATRIX& GetViewMatrix() const;
	const XMMATRIX& GetProjectionMatrix() const;
	float GetFov() const;
	float GetAspectRatio() const;
	float GetNearPlane() const;
	float GetFarPlane() const;

	void SetFov(float fov, bool updateProjection = true);
	void SetAspectRatio(float aspectRatio, bool updateProjection = true);
	void SetNearPlane(float nearPlane, bool updateProjection = true);
	void SetFarPlane(float farPlane, bool updateProjection = true);

	float& GetSpeed() { return moveSpeed; }

	std::string Save() override;
private:
	void UpdateProjectionMatrix();
	void UpdateViewMatrix();

	XMMATRIX viewMatrix = {};
	XMMATRIX projectionMatrix = {};
	float fov = 90.0f;	
	float aspectRatio = 0.0f;
	float nearPlane = 0.0f;
	float farPlane = 0.0f;

	float moveSpeed = 0.01f;
};

