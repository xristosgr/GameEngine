#pragma once

#include <DirectXMath.h>

class Camera
{
public:
	Camera();
	void PerspectiveFov(float fovDegrees, float aspectRatio, float nearZ, float farZ);
	void OrthographicFov(int screenWidth, int screenHeight, float nearZ, float farZ);

	DirectX::XMMATRIX& GetViewMatrix();
	DirectX::XMMATRIX& GetBaseViewMatrix();
	DirectX::XMMATRIX& GetProjectionMatrix();

	const DirectX::XMVECTOR& GetPositionVector() const;
	const DirectX::XMFLOAT3& GetPositionFloat3() const;
	const DirectX::XMVECTOR& GetRotationVector() const;
	const DirectX::XMFLOAT3& GetRotationFloat3() const;

	void SetPosition(const DirectX::XMVECTOR& pos);
	void SetPosition(DirectX::XMFLOAT3& pos);
	void SetPosition(float x, float y, float z);
	void AdjustPosition(const DirectX::XMVECTOR& pos);
	void AdjustPosition(float x, float y, float z);
	void SetRotation(const DirectX::XMVECTOR& rot);
	void SetRotation(float x, float y, float z);
	void AdjustRotation(const DirectX::XMVECTOR& rot, bool constraint = false);
	void AdjustRotation(float x, float y, float z, bool constraint = false);
	void SetLookAtPos(DirectX::XMFLOAT3 lookAtPos);
	const DirectX::XMVECTOR& GetForwardVector();
	const DirectX::XMVECTOR& GetRightVector();
	const DirectX::XMVECTOR& GetLeftVector();
	const DirectX::XMVECTOR& GetBackwardVector();

	DirectX::XMVECTOR camTarget;

	DirectX::XMFLOAT4 dir;
	DirectX::XMFLOAT3 rot;
	float yaw = 0.0f;
	float pitch = 0.0f;
	bool PossessCharacter = false;
	DirectX::XMFLOAT3 pos;

private:
	void UpdateViewMatrix();

	DirectX::XMVECTOR posVector;
	DirectX::XMVECTOR rotVector;


	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;

	const DirectX::XMVECTOR DEFAULT_FORWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_UP_VECTOR = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_BACKWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_LEFT_VECTOR = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_RIGHT_VECTOR = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

public:
	DirectX::XMVECTOR vec_forward;
	DirectX::XMVECTOR vec_left;
	DirectX::XMVECTOR vec_right;
	DirectX::XMVECTOR vec_backward;
	DirectX::XMVECTOR upDir;


};