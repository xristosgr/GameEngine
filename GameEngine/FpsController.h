#pragma once
#include "Entity.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Camera.h"

class FpsController
{
public:
	FpsController();

	void MouseMovement(float& dt, Entity& entity, Keyboard& keyboard, Mouse& mouse, Camera& camera);
	void Movement(float& dt, float gravity, Entity* entity, Keyboard& keyboard, Mouse& mouse, Camera& camera);

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
};

