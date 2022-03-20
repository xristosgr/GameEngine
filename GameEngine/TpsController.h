#pragma once
#include "Entity.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Camera.h"
#include "SoundComponent.h"

class TpsController
{
public:
	TpsController();

	void Intitialize();
	void MouseMovement(float& dt, Entity& entity, Keyboard& keyboard, Mouse& mouse, Camera& camera);
	void Movement(float& dt, float gravity, Entity& entity, Keyboard& keyboard, Mouse& mouse, Camera& camera);

	void SetCharacterRotation(Entity& entity, Camera& camera);
	void Update();
private:
	enum RotationEnum
	{
		UP = 0,
		RIGHT_UP = 1,
		RIGHT = 2,
		RIGHT_DOWN = 3,
		DOWN = 4,
		LEFT_DOWN = 5,
		LEFT = 6,
		LEFT_UP = 7,
	};
	RotationEnum currRotation;
	RotationEnum prevRotation;

	bool rotateLeft = false;
	bool rotateUp = false;
	bool rotateRight = false;

	AppTimer timer;
	double val = 0.0;
	bool canJump = true;
	bool isJumping = false;
	bool jumpkeyIsPressed = false;
	//float gravity = -0.1f;

private:
	bool canPressSpace = true;
	float CharacterRotY = 0.0f;

	float lastCamRot = 0.0f;
	bool isMoving = false;
	bool isFalling = false;
	XMFLOAT3 cameraPrevPos;
	XMVECTOR vPrevLookAt;
	XMVECTOR vPrevPos;
	XMVECTOR vLerpLookAt;
	XMVECTOR vLerpPos;

	XMVECTOR vLerpPrevPos;
	XMVECTOR vLerpCamPos;
	SoundComponent sound;

};

