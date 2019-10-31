#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

class GameObject
{
public:

	const XMVECTOR& GetPositionVector() const;
	const XMFLOAT3& GetPositionFloat3() const;
	const XMVECTOR& GetRotationVector() const;
	const XMFLOAT3& GetRotationFloat3() const;

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
	void SetLookAtPos(XMFLOAT3 lookAtPos);
	void SetLookAtPos(float x, float y, float z);

	const XMVECTOR& GetForwardVector(bool omitY = false);
	const XMVECTOR& GetLeftVector(bool omitY = false);
	const XMVECTOR& GetRightVector(bool omitY = false);
	const XMVECTOR& GetBackwardVector(bool omitY = false);

	const XMFLOAT3& GetForwardVectorFloat(bool omitY = false);
	const XMFLOAT3& GetLeftVectorFloat(bool omitY = false);
	const XMFLOAT3& GetRightVectorFloat(bool omitY = false);
	const XMFLOAT3& GetBackwardVectorFloat(bool omitY = false);

protected:
	virtual void UpdateMatrix();
	void UpdateDirectionVectors();

	XMVECTOR posVector;
	XMVECTOR rotVector;
	XMFLOAT3 pos;
	XMFLOAT3 rot;

	const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	XMVECTOR forwardVector;
	XMVECTOR leftVector;
	XMVECTOR rightVector;
	XMVECTOR backwardVector;

	XMVECTOR forwardVector_noY;
	XMVECTOR leftVector_noY;
	XMVECTOR rightVector_noY;
	XMVECTOR backwardVector_noY;
};
