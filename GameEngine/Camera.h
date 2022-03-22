#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
public:
	Camera();
	void PerspectiveFov(float fovDegrees, float aspectRatio, float nearZ, float farZ);
	void OrthographicFov(int screenWidth, int screenHeight, float nearZ, float farZ);

	XMMATRIX& GetViewMatrix();
	XMMATRIX& GetBaseViewMatrix();
	XMMATRIX& GetProjectionMatrix();

	const XMVECTOR& GetPositionVector() const;
	const XMFLOAT3& GetPositionFloat3() const;
	const XMVECTOR& GetRotationVector() const;
	const XMFLOAT3& GetRotationFloat3() const;

	void SetPosition(const XMVECTOR& pos);
	void SetPosition(XMFLOAT3& pos);
	void SetPosition(float x, float y, float z);
	void AdjustPosition(const XMVECTOR& pos);
	void AdjustPosition(float x, float y, float z);
	void SetRotation(const XMVECTOR& rot);
	void SetRotation(float x, float y, float z);
	void AdjustRotation(const XMVECTOR& rot, bool constraint = false);
	void AdjustRotation(float x, float y, float z, bool constraint = false);
	void SetLookAtPos(XMFLOAT3 lookAtPos);
	const XMVECTOR& GetForwardVector();
	const XMVECTOR& GetRightVector();
	const XMVECTOR& GetLeftVector();
	const XMVECTOR& GetBackwardVector();

	XMVECTOR camTarget;

	XMFLOAT4 dir;
	XMFLOAT3 rot;
	float yaw = 0.0f;
	float pitch = 0.0f;
	bool PossessCharacter = false;
	XMFLOAT3 pos;

private:
	void UpdateViewMatrix();

	XMVECTOR posVector;
	XMVECTOR rotVector;


	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;

	const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

public:
	XMVECTOR vec_forward;
	XMVECTOR vec_left;
	XMVECTOR vec_right;
	XMVECTOR vec_backward;
	XMVECTOR upDir;


};