#pragma once
#include "../../Tools/StrippedWindows.h"
#include <DirectXMath.h>
using namespace DirectX;

static const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
static const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
static const XMVECTOR DEFAULT_DOWN_VECTOR = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
static const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
static const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
static const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

class Transform
{
public:
	Transform() {}
	Transform(const Transform& transform);

	void Update(float dt);
	const XMVECTOR& GetPosition() const;
	const XMVECTOR& GetLastPosition() const;
	const XMQUATERNION& GetOrientation() const;
	XMMATRIX GetRotationMatrix() const;

	void SetLastPosition(const XMVECTOR& pos);

	void UpdateLastPosition();

	void SetPosition(const XMVECTOR& pos);
	void SetPosition(const XMFLOAT3& pos);
	void SetPosition(float x, float y, float z);

	void AdjustPosition(const XMVECTOR& pos);
	void AdjustPosition(XMFLOAT3 pos);
	void AdjustPosition(float x, float y, float z);

	void SetOrientation(const XMQUATERNION& orient);
	void SetOrientation(float roll, float pitch, float yaw);
	void SetOrientation(XMMATRIX rotationMatrix);

	void RotateByAxis(XMVECTOR axis, float angle);

	const XMVECTOR& GetForwardVector(bool omitY = false);
	const XMVECTOR& GetLeftVector(bool omitY = false);
	const XMVECTOR& GetRightVector(bool omitY = false);
	const XMVECTOR& GetBackwardVector(bool omitY = false);
	const XMVECTOR& GetUpVector(bool omitY = false);
	const XMVECTOR& GetDownVector(bool omitY = false);

	const XMMATRIX& GetWorldMatrix();
	void SetWorldMatrix(const XMMATRIX& worldMatrix);

	bool& GetLockedPos(int dimension);
	void SetLockedPos(bool lockVal, int dimension);
	void UpdateDirectionVectors();

	float& GetScale() { return this->scale; }
	void SetScale(float scale) { this->scale = scale; }
private:

	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMVECTOR position = XMVECTOR();
	XMQUATERNION orientation = XMQuaternionIdentity();
	XMMATRIX rotationMatrix = {};

	XMVECTOR forwardVector = XMVECTOR();
	XMVECTOR upVector = XMVECTOR();
	XMVECTOR downVector = XMVECTOR();
	XMVECTOR leftVector = XMVECTOR();
	XMVECTOR rightVector = XMVECTOR();
	XMVECTOR backwardVector = XMVECTOR();

	XMVECTOR lastPosition = XMVECTOR();
	float scale = 1.0f;

	bool lockedPos[3] = { false,false,false };
};

