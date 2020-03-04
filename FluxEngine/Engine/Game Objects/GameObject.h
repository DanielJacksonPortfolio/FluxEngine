#pragma once
#include "Model Data/Model.h"

static const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
static const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
static const XMVECTOR DEFAULT_DOWN_VECTOR = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
static const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
static const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
static const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

static int OBJECT_COUNT = 0;

class GameObject
{
public:
	virtual bool Init(XMVECTOR position, XMVECTOR orientation, std::string name = "");
	virtual void Update(float dt) { };

	const XMVECTOR& GetPosition() const;
	const XMQUATERNION& GetOrientation() const;
	const XMMATRIX& GetOrientationMatrix() const;

	XMMATRIX GetRotationMatrix() const;

	void SetPosition(const XMVECTOR& pos);
	void SetPosition(const XMFLOAT3& pos);
	void SetPosition(float x, float y, float z);

	void AdjustPosition(const XMVECTOR& pos);
	void AdjustPosition(const XMFLOAT3& pos);
	void AdjustPosition(float x, float y, float z);

	void SetOrientation(const XMQUATERNION& orient);
	void SetOrientation(float roll, float pitch, float yaw);
	void SetOrientation(XMMATRIX rotationMatrix);

	//void SlerpOrientation(const XMQUATERNION& orient, float dt);

	void RotateByAxis(XMVECTOR axis, float angle);

	//void SetLookAtPos(XMFLOAT3 lookAtPos);
	//void SetLookAtPos(float x, float y, float z);

	const XMVECTOR& GetForwardVector(bool omitY = false);
	const XMVECTOR& GetLeftVector(bool omitY = false);
	const XMVECTOR& GetRightVector(bool omitY = false);
	const XMVECTOR& GetBackwardVector(bool omitY = false);
	const XMVECTOR& GetUpVector(bool omitY = false);
	const XMVECTOR& GetDownVector(bool omitY = false);


	bool& GetLockedPos(int dimension);
	void SetLockedPos(bool lockVal, int dimension);
	std::string& GetName();
	void SetName(std::string name);

	XMVECTOR& GetLVelocity() { return this->linearVelocity; }
	void SetLVelocity(XMVECTOR v) { this->linearVelocity = v; }

protected:
	virtual void UpdateMatrix();
	void UpdateDirectionVectors();

	XMVECTOR position = XMVECTOR();
	XMQUATERNION orientation = XMQuaternionIdentity();
	XMMATRIX rotationMatrix = {};

	XMVECTOR forwardVector = XMVECTOR();
	XMVECTOR upVector = XMVECTOR();
	XMVECTOR downVector	= XMVECTOR();
	XMVECTOR leftVector	= XMVECTOR();
	XMVECTOR rightVector = XMVECTOR();
	XMVECTOR backwardVector = XMVECTOR();

	std::string name = "";
	bool lockedPos[3] = { false,false,false };

	XMVECTOR linearVelocity = { 0.0f ,0.0f ,0.0f ,0.0f };
};
