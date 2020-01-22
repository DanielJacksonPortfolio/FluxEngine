#pragma once
#include "Model Data/Model.h"

class GameObject
{
public:

	const XMVECTOR& GetPositionVector() const;
	const XMFLOAT3& GetPositionFloat3() const;
	const XMVECTOR& GetRotationVector() const;
	const XMFLOAT3& GetRotationFloat3() const;
	const XMFLOAT3& GetOrientationQuaternion() const;
	const XMFLOAT3& GetOrientationMatrix() const;

	void SetPosition(const XMVECTOR& pos);
	void SetPosition(const XMFLOAT3& pos);
	void SetPosition(float x, float y, float z);
	void AdjustPosition(const XMVECTOR& pos);
	void AdjustPosition(const XMFLOAT3& pos);
	void AdjustPosition(float x, float y, float z);
	void SetRotation(const XMVECTOR& rot);
	void SetRotation(const XMFLOAT3& rot);
	void SetRotation(float x, float y, float z);
	void AdjustRotation(const XMVECTOR& rot);
	void AdjustRotation(const XMFLOAT3& rot);
	void AdjustRotation(float x, float y, float z);
	void SetOrientation(const XMQUATERNION& orient);
	void AdjustOrientation(const XMQUATERNION& orient, float dt);
	void SetLookAtPos(XMFLOAT3 lookAtPos);
	void SetLookAtPos(float x, float y, float z);

	const XMVECTOR& GetForwardVector(bool omitY = false);
	const XMVECTOR& GetLeftVector(bool omitY = false);
	const XMVECTOR& GetRightVector(bool omitY = false);
	const XMVECTOR& GetBackwardVector(bool omitY = false);
	const XMVECTOR& GetUpVector(bool omitY = false);
	const XMVECTOR& GetDownVector(bool omitY = false);

	const XMFLOAT3 GetForwardVectorFloat(bool omitY = false);
	const XMFLOAT3 GetLeftVectorFloat(bool omitY = false);
	const XMFLOAT3 GetRightVectorFloat(bool omitY = false);
	const XMFLOAT3 GetBackwardVectorFloat(bool omitY = false);
	const XMFLOAT3 GetUpVectorFloat(bool omitY = false);
	const XMFLOAT3 GetDownVectorFloat(bool omitY = false);

	bool& GetLockedPos(int dimension);
	void SetLockedPos(bool lockVal, int dimension);
	bool& GetLockedRot(int dimension);
	void SetLockedRot(bool lockVal, int dimension);
	std::string& GetName();
	void SetName(std::string name);
	XMFLOAT3& GetVelocity();
	void SetVelocity(XMFLOAT3 v);
	float& GetAcceleration();
	void SetAcceleration(float a);

protected:
	virtual void UpdateMatrix();
	void UpdateDirectionVectors();

	XMVECTOR posVector = XMVECTOR();
	XMVECTOR rotVector = XMVECTOR();
	XMQUATERNION orientation = XMQuaternionIdentity();
	XMFLOAT3 pos = XMFLOAT3(0.0f,0.0f,0.0f);
	XMFLOAT3 rot = XMFLOAT3(0.0f,0.0f,0.0f);

	const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_DOWN_VECTOR = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	XMVECTOR forwardVector = XMVECTOR();
	XMVECTOR upVector = XMVECTOR();
	XMVECTOR downVector	= XMVECTOR();
	XMVECTOR leftVector	= XMVECTOR();
	XMVECTOR rightVector = XMVECTOR();
	XMVECTOR backwardVector = XMVECTOR();

	XMVECTOR forwardVector_noY = XMVECTOR();
	XMVECTOR upVector_noY = XMVECTOR();
	XMVECTOR downVector_noY	= XMVECTOR();
	XMVECTOR leftVector_noY	= XMVECTOR();
	XMVECTOR rightVector_noY = XMVECTOR();
	XMVECTOR backwardVector_noY = XMVECTOR();

	XMFLOAT3 velocity = XMFLOAT3(0.005f,0.005f,0.005f);
	float acceleration = 0.0f;
	float mass = 0.0f;

	std::string objectName = "Null Object";
	bool lockedPos[3] = { false,false,false };
	bool lockedRot[3] = { false,false,false };
};
