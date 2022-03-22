#include "Camera.h"
#include <string>


using namespace DirectX;

Camera::Camera()
{
	this->pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->posVector = DirectX::XMLoadFloat3(&this->pos);
	this->rot = DirectX::XMFLOAT3(0.f, 0.0f, 0.0f);
	this->rotVector = DirectX::XMLoadFloat3(&this->rot);
	UpdateViewMatrix();
}

void Camera::PerspectiveFov(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	//float fovRadians = XM_PI / 2.0f;
	float fovRadians = (fovDegrees / 360.0f) * DirectX::XM_2PI;


	this->projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

void Camera::OrthographicFov(int screenWidth, int screenHeight, float nearZ, float farZ)
{
	this->projectionMatrix = DirectX::XMMatrixOrthographicLH(screenWidth, screenHeight, nearZ, farZ);
}

DirectX::XMMATRIX& Camera::GetViewMatrix()
{
	return this->viewMatrix;
}


DirectX::XMMATRIX& Camera::GetProjectionMatrix()
{
	return this->projectionMatrix;
}

const DirectX::XMVECTOR& Camera::GetPositionVector() const
{
	return this->posVector;
}

const DirectX::XMFLOAT3& Camera::GetPositionFloat3() const
{
	return this->pos;
}

const DirectX::XMVECTOR& Camera::GetRotationVector() const
{
	return this->rotVector;
}

const DirectX::XMFLOAT3& Camera::GetRotationFloat3() const
{
	return this->rot;
}

void Camera::SetPosition(const DirectX::XMVECTOR& pos)
{
	XMStoreFloat3(&this->pos, pos);
	this->posVector = pos;
	this->UpdateViewMatrix();

}
void Camera::SetPosition(DirectX::XMFLOAT3& pos)
{
	this->pos = pos;

	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateViewMatrix();

}
void Camera::SetPosition(float x, float y, float z)
{
	this->pos = DirectX::XMFLOAT3(x, y, z);
	this->posVector = DirectX::XMLoadFloat3(&this->pos);
	this->UpdateViewMatrix();
}

void Camera::AdjustPosition(const DirectX::XMVECTOR& pos)
{
	this->posVector += pos;
	DirectX::XMStoreFloat3(&this->pos, this->posVector);
	this->UpdateViewMatrix();
}

void Camera::AdjustPosition(float x, float y, float z)
{

	this->pos.x += x;
	this->pos.z += z;
	this->pos.y += y;

	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateViewMatrix();
}

void Camera::SetRotation(const DirectX::XMVECTOR& rot)
{

	XMStoreFloat3(&this->rot, rot);
	this->rotVector = rot;
	this->UpdateViewMatrix();
}

void Camera::SetRotation(float x, float y, float z)
{
	this->rot = DirectX::XMFLOAT3(x, y, z);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

void Camera::AdjustRotation(const DirectX::XMVECTOR& rot, bool constraint)
{
	if (constraint)
	{
		if (this->rot.x > 1.4f)
		{
			this->rot.x = 1.4f;
		}
		if (this->rot.x < -1.4f)
		{
			this->rot.x = -1.4f;
		}
	}
	this->rotVector += rot;
	XMStoreFloat3(&this->rot, this->rotVector);
	this->UpdateViewMatrix();
}

void Camera::AdjustRotation(float x, float y, float z, bool constraint)
{
	if (constraint)
	{
		if (this->rot.x > 1.3f)
		{
			this->rot.x = 1.3f;
		}
		if (this->rot.x < -1.3f)
		{
			this->rot.x = -1.3f;
		}
	}

	this->rot.x += x;
	this->rot.y += y;
	this->rot.z += z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

void Camera::SetLookAtPos(DirectX::XMFLOAT3 lookAtPos)
{
	if (lookAtPos.x == this->pos.x && lookAtPos.y == this->pos.y && lookAtPos.z == this->pos.z)
	{
		return;
	}
	lookAtPos.x = this->pos.x - lookAtPos.x;
	lookAtPos.y = this->pos.y - lookAtPos.y;
	lookAtPos.z = this->pos.z - lookAtPos.z;

	pitch = 0.0f;
	if (lookAtPos.y != 0.0f)
	{
		const float distance = sqrt(pow(lookAtPos.x, 2) + pow(lookAtPos.z, 2));
		pitch = atan(lookAtPos.y / distance);
	}

	yaw = 0.0f;
	if (lookAtPos.x != 0.0f)
	{
		yaw = atan(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0)
	{
		yaw += DirectX::XM_PI;
	}

	this->SetRotation(pitch, yaw, 0.0f);
}

const DirectX::XMVECTOR& Camera::GetForwardVector()
{
	return this->vec_forward;
}

const DirectX::XMVECTOR& Camera::GetRightVector()
{
	return this->vec_right;
}

const DirectX::XMVECTOR& Camera::GetLeftVector()
{
	return this->vec_left;
}

const DirectX::XMVECTOR& Camera::GetBackwardVector()
{
	return this->vec_backward;
}

void Camera::UpdateViewMatrix()
{
	DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z);
	camTarget = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, camRotationMatrix);
	camTarget += this->posVector;
	upDir = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, camRotationMatrix);
	this->viewMatrix = DirectX::XMMatrixLookAtLH(this->posVector, camTarget, upDir);

	//XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, 0.0f);

	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, 0.0f);
	this->vec_forward = DirectX::XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, rotationMatrix);
	this->vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, rotationMatrix);
	this->vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, rotationMatrix);
	this->vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, rotationMatrix);
}