#pragma once
#include "GameObject.h"
class Camera : public GameObject
{
public:
	Camera();
	Camera(XMFLOAT3 pos, XMFLOAT3 rot);
	

	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

	const XMMATRIX& GetViewMatrix() const;
	const XMMATRIX& GetProjectionMatrix() const;

private:
	void UpdateMatrix() override;
	void InitCamera(XMFLOAT3 pos, XMFLOAT3 rot);
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
};

