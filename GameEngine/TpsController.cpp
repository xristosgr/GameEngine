#include "TpsController.h"

TpsController::TpsController()
{
	isJumping = false;
	timer.Start();
	currRotation = RotationEnum::UP;
	prevPos = XMFLOAT2(0, 0);

	//gravity = -0.2f;
}

void TpsController::MouseMovement(float& dt, Entity& entity, Keyboard& keyboard, Mouse& mouse, Camera& camera)
{
	const float cameraSpeed = 0.007f;

	XMFLOAT4 rightFloat4;
	XMStoreFloat4(&rightFloat4, camera.GetRightVector());
	XMFLOAT4 forwardFloat4;
	XMStoreFloat4(&forwardFloat4, camera.GetForwardVector());
	isMouseMoving = false;
	while (!mouse.EventBufferIsEmpty())
	{
		if (camera.PossessCharacter)
		{
			MouseEvent me = mouse.ReadEvent();

			OutputDebugStringA("TRUE!!!\n");
			//OutputDebugStringA(("X = " + std::to_string(static_cast<float>(me.GetPosX()))).c_str());
			//OutputDebugStringA(("  |Y = " + std::to_string(static_cast<float>(me.GetPosY())) + "\n").c_str());

			if (prevPos.x != static_cast<float>(me.GetPosX()) || prevPos.y != static_cast<float>(me.GetPosY()))
			{
				isMouseMoving = true;
			}
			prevPos = XMFLOAT2(static_cast<float>(me.GetPosX()), static_cast<float>(me.GetPosY()));
			
			if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
			{
				if (static_cast<float>(me.GetPosY()) < 0.0)
				{
					if (CharacterRotY > -1.3)
						CharacterRotY -= cameraSpeed;
				}
				else if (static_cast<float>(me.GetPosY()) > 0.0)
				{
					if (CharacterRotY < 3.0)
						CharacterRotY += cameraSpeed;
				};
				
				camera.AdjustPosition(rightFloat4.x * -cameraSpeed * static_cast<float>(me.GetPosX()), forwardFloat4.y * -cameraSpeed * static_cast<float>(me.GetPosY()), rightFloat4.z * -cameraSpeed * static_cast<float>(me.GetPosX()));
			}
		}
	}
}

void TpsController::Movement(float& dt, float gravity, Entity& entity, Keyboard& keyboard, Mouse& mouse, Camera& camera)
{
	if (!entity.physicsComponent.aActor)
		return;

	entity.isMovingLeft = false;

	XMFLOAT4 forwardDir;
	XMStoreFloat4(&forwardDir, camera.vec_forward);
	XMFLOAT4 rightDir;
	XMStoreFloat4(&rightDir, camera.vec_right);


	float velocity = 5.0;
	float moveX = 0.0f;
	float moveZ = 0.0f;

	//entity.physicsComponent.trans.q = physx::PxQuat((float4_forward.z), physx::PxVec3(0, 1, 0));

	entity.physicsComponent.trans = entity.physicsComponent.aActor->getGlobalPose();
	//Rotate with camera
	//entity.physicsComponent.trans.q = physx::PxQuat((camera.rot.y), physx::PxVec3(0, 1, 0));
	SetCharacterRotation(entity, camera);

	if (entity.isMovingFront || entity.isMovingLeft || entity.isMovingRight)
	{
		entity.physicsComponent.trans.q = physx::PxQuat((camera.rot.y + entity.rotationDir), physx::PxVec3(0, 1, 0));
		lastCamRot = camera.rot.y;
	}
		
	else
		entity.physicsComponent.trans.q = physx::PxQuat((lastCamRot + entity.rotationDir), physx::PxVec3(0, 1, 0));
	

	entity.physicsComponent.aActor->setGlobalPose(entity.physicsComponent.trans);

	entity.matrix_rotate = XMMatrixRotationRollPitchYaw(entity.rot.x, entity.rot.y, entity.rot.z);

	//entity.matrix_rotate *= XMMatrixRotationAxis(XMVECTOR{ 0, 1, 0 }, camera.rot.y);
	//entity.matrix_rotate *= XMMatrixRotationAxis(XMVECTOR{ 0, entity.physicsComponent.trans.q.y, 0 }, entity.physicsComponent.trans.q.w);

	if (keyboard.KeyIsPressed(VK_F8))
	{
		camera.PossessCharacter = true;
	}
	if (keyboard.KeyIsPressed(VK_F9))
	{
		camera.PossessCharacter = false;
	}


	entity.pos = XMFLOAT3(entity.physicsComponent.trans.p.x, entity.physicsComponent.trans.p.y, entity.physicsComponent.trans.p.z);
	if (camera.PossessCharacter)
	{

		if ((!entity.isMovingRight && !entity.isMovingLeft) || isMouseMoving)
		{
			camera.SetLookAtPos(XMFLOAT3(entity.physicsComponent.trans.p.x, entity.physicsComponent.trans.p.y + 0.7f, entity.physicsComponent.trans.p.z));
			cameraPrevPos = XMFLOAT3((-2.4f * std::sin(camera.yaw)), CharacterRotY, (-2.4f * std::cos(camera.yaw)));
			camera.SetPosition(XMVECTOR{ entity.pos.x + (-2.4f * std::sin(camera.yaw)),entity.pos.y + CharacterRotY,entity.pos.z + (-2.4f * std::cos(camera.yaw)) });
		}
		else
		{
			camera.SetPosition(XMVECTOR{ entity.pos.x + cameraPrevPos.x,entity.pos.y + cameraPrevPos.y, entity.pos.z + cameraPrevPos.z });
		}
		

		if (keyboard.KeyIsPressed('S') && (keyboard.KeyIsPressed('A')))
		{
			currRotation = RotationEnum::LEFT_DOWN;
			entity.isMovingRight = true;
			entity.isMovingLeft = true;

			moveX = -velocity * forwardDir.x - velocity * rightDir.x;
			moveZ = -velocity * forwardDir.z - velocity * rightDir.z;



			entity.model.currAnim = 1;
		}
		else if (keyboard.KeyIsPressed('S') && (keyboard.KeyIsPressed('D')))
		{
			currRotation = RotationEnum::RIGHT_DOWN;
			entity.isMovingRight = true;

			moveX = -velocity * forwardDir.x + velocity * rightDir.x;
			moveZ = -velocity * forwardDir.z + velocity * rightDir.z;

			entity.model.currAnim = 1;
		}
		else if (keyboard.KeyIsPressed('W') && (keyboard.KeyIsPressed('A')))
		{
			currRotation = RotationEnum::LEFT_UP;
			entity.isMovingRight = true;
			entity.isMovingLeft = true;
			moveX = velocity * forwardDir.x - velocity * rightDir.x;
			moveZ = velocity * forwardDir.z - velocity * rightDir.z;


			entity.model.currAnim = 1;
		}
		else if (keyboard.KeyIsPressed('W') && (keyboard.KeyIsPressed('D')))
		{
			currRotation = RotationEnum::RIGHT_UP;
			entity.isMovingRight = true;

			moveX = velocity * forwardDir.x + velocity * rightDir.x;
			moveZ = velocity * forwardDir.z + velocity * rightDir.z;

			entity.model.currAnim = 1;
		}
		else
		{
			entity.isMovingRight = false;
			if (keyboard.KeyIsPressed('W'))
			{
				currRotation = RotationEnum::UP;
				entity.isMovingFront = true;

				moveX = velocity * forwardDir.x;
				moveZ = velocity * forwardDir.z;

			}
			else if (keyboard.KeyIsPressed('S'))
			{
				currRotation = RotationEnum::DOWN;
				entity.isMovingFront = true;

				moveX = -velocity * forwardDir.x;
				moveZ = -velocity * forwardDir.z;


				entity.model.currAnim = 1;

			}
			else
			{
				entity.isMovingFront = false;
			}


			if (keyboard.KeyIsPressed('D'))
			{

				currRotation = RotationEnum::RIGHT;
				entity.isMovingRight = true;

				moveX = velocity * rightDir.x;
				moveZ = velocity * rightDir.z;



			}
			else if (keyboard.KeyIsPressed('A'))
			{
				currRotation = RotationEnum::LEFT;
				entity.isMovingRight = true;
				entity.isMovingLeft = true;


				moveX = -velocity * rightDir.x;
				moveZ = -velocity * rightDir.z;


				entity.model.currAnim = 1;

			}
			else
			{
				entity.isMovingRight = false;
			}


			if ((!entity.isMovingFront && !entity.isMovingRight) || entity.isFalling)
			{
				entity.model.currAnim = 0;



			}
			else if (entity.isMovingFront || entity.isMovingRight)
			{


			}
		}



		if (!entity.isFalling)
		{
			if (canPressSpace)
			{
				if (keyboard.KeyIsPressed(VK_SPACE))
				{
					canPressSpace = false;
					timer.Restart();
					isJumping = true;
					entity.physicsComponent.aActor->addForce(physx::PxVec3(moveX, 300.0f, moveZ), physx::PxForceMode::eIMPULSE);
				}
			}

			if (!keyboard.KeyIsPressed(VK_SPACE))
			{
				//OutputDebugStringA("FREED!!!!!!!\n");
				canPressSpace = true;
			}
		}


		if (timer.GetMilisecondsElapsed() > 2.0f * dt)
		{
			if (!entity.isFalling && isJumping)
			{
				isJumping = false;
			}
		}
		if (!isJumping)
		{
			if (!entity.isFalling)
			{
				isJumping = false;
				entity.physicsComponent.aActor->setLinearVelocity(physx::PxVec3(moveX, 0.0f, moveZ));
			}
		}

		if (entity.isFalling)
		{
			entity.physicsComponent.aActor->addForce(physx::PxVec3(moveX*30, 0, moveZ*30), physx::PxForceMode::eFORCE);
		}
		//OutputDebugStringA(("X = " + std::to_string(moveX)+ " |Y = " + std::to_string(moveZ) + "\n").c_str());

		if (entity.isFalling)
		{
			entity.model.SetAnimIndex(2, true, 10.0f);
		}
		else
		{
			if (moveX != 0 || moveZ != 0)
			{
				entity.model.SetAnimIndex(1, true, 5.0f);
			}
			else
			{
				entity.model.SetAnimIndex(0,true,5.0f);
			}
		}
		
		entity.model.Update();

		entity.physicsComponent.aActor->setGlobalPose(entity.physicsComponent.trans);
	}
}



void TpsController::SetCharacterRotation(Entity& entity, Camera& camera)
{
	float rotSpeed = 0.15f;
	switch (currRotation)
	{
		case UP:
			entity.rotationDir = 3.13f;
			break;
		case DOWN:
			entity.rotationDir = 6.28f;
			break;
		case LEFT:
			entity.rotationDir = 1.59f;
			break;
		case RIGHT:
			entity.rotationDir = 4.87f;
			break;
		case RIGHT_UP:
			entity.rotationDir = 3.73f;
			break;
		case RIGHT_DOWN:
			entity.rotationDir = 5.47f;
			break;
		case LEFT_UP:
			entity.rotationDir = 2.3f;
			break;
		case LEFT_DOWN:
			entity.rotationDir = 0.68f;
			break;


	}


	//switch (currRotation)
	//{


	//case UP:
	//{
	//	rotateRight = false;
	//	rotateUp = true;
	//	rotateLeft = true;
	//	if (entity.rotationDir < 3.0)
	//	{
	//		entity.rotationDir += rotSpeed;
	//	}
	//	else if (entity.rotationDir > 3.2)
	//	{
	//		entity.rotationDir -= rotSpeed;
	//	}
	//	else
	//	{
	//		entity.rotationDir = 3.1;
	//	}
	//	break;
	//}
	//case DOWN:
	//{
	//	rotateLeft = false;
	//
	//	if (!rotateRight)
	//	{
	//		if (rotateUp == true)
	//		{
	//			if (entity.rotationDir < 6.2)
	//			{
	//				entity.rotationDir += rotSpeed;
	//			}
	//			else
	//			{
	//				entity.rotationDir = 6.3;
	//				rotateUp = false;
	//			}
	//
	//		}
	//		else
	//		{
	//			if (entity.rotationDir <= 2)
	//			{
	//				if (entity.rotationDir >= 0.0f && entity.rotationDir < 6.2)
	//				{
	//					entity.rotationDir -= rotSpeed;
	//				}
	//				else
	//				{
	//					entity.rotationDir = 6.3;
	//				}
	//			}
	//			else if (entity.rotationDir > 4.0f)
	//			{
	//				if (entity.rotationDir >= 5.0f && entity.rotationDir < 6.3)
	//				{
	//					entity.rotationDir += rotSpeed;
	//				}
	//				else
	//				{
	//					//entity.rotationDir = 0.0f;
	//				}
	//			}
	//			else
	//			{
	//
	//				if (entity.rotationDir < 6.2)
	//				{
	//					entity.rotationDir += rotSpeed;
	//				}
	//				else if (entity.rotationDir > 6.4)
	//				{
	//					entity.rotationDir -= rotSpeed;
	//				}
	//				else
	//				{
	//					entity.rotationDir = 6.3;
	//				}
	//			}
	//
	//		}
	//	}
	//	else
	//	{
	//		if (entity.rotationDir < 6.2)
	//		{
	//			entity.rotationDir += rotSpeed;
	//		}
	//		else
	//		{
	//			entity.rotationDir = 6.3;
	//		}
	//
	//	}
	//
	//	break;
	//}
	//
	//
	//case LEFT:
	//	rotateRight = true;
	//	rotateUp = false;
	//	rotateLeft = true;
	//	if (entity.rotationDir < 4.7)
	//	{
	//		entity.rotationDir += rotSpeed;
	//	}
	//	else if (entity.rotationDir > 4.9)
	//	{
	//		entity.rotationDir -= rotSpeed;
	//	}
	//	else
	//	{
	//		entity.rotationDir = 4.8;
	//	}
	//	break;
	//case RIGHT:
	//	rotateRight = false;
	//	rotateUp = false;
	//	//rotateLeft = false;
	//	if (!rotateLeft)
	//	{
	//		if (entity.rotationDir > 2 && entity.rotationDir <= 6.6f)
	//			entity.rotationDir = 0.0f;
	//		if (entity.rotationDir < 0.7)
	//		{
	//			entity.rotationDir += rotSpeed;
	//		}
	//		else if (entity.rotationDir > 1.5)
	//		{
	//			entity.rotationDir -= rotSpeed;
	//		}
	//		else
	//		{
	//			entity.rotationDir = 1.4;
	//
	//			rotateLeft = true;
	//		}
	//
	//	}
	//	else
	//	{
	//		if (entity.rotationDir < 1.3)
	//		{
	//			entity.rotationDir += rotSpeed;
	//		}
	//		else if (entity.rotationDir > 1.5)
	//		{
	//			entity.rotationDir -= rotSpeed;
	//		}
	//		else
	//		{
	//			entity.rotationDir = 1.4;
	//		}
	//	}
	//
	//	break;

	//case RIGHT_UP:
	//	rotateRight = false;
	//	rotateUp = false;
	//	rotateLeft = true;
	//	if (entity.rotationDir < 2.1)
	//	{
	//		entity.rotationDir += rotSpeed;
	//	}
	//	else if (entity.rotationDir > 2.3)
	//	{
	//		entity.rotationDir -= rotSpeed;
	//	}
	//	else
	//	{
	//		entity.rotationDir = 2.2;
	//	}
	//	break;
	//case RIGHT_DOWN:
	//	rotateRight = false;
	//	rotateUp = false;
	//	rotateLeft = false;
	//	if (!rotateLeft)
	//	{
	//		if (entity.rotationDir > 2 && entity.rotationDir <= 6.6f)
	//			entity.rotationDir = 0.0f;
	//		if (entity.rotationDir < 0.7)
	//		{
	//			entity.rotationDir += rotSpeed;
	//		}
	//		else if (entity.rotationDir > 0.9)
	//		{
	//			entity.rotationDir -= rotSpeed;
	//		}
	//		else
	//		{
	//			entity.rotationDir = 0.8;
	//
	//			rotateLeft = true;
	//		}
	//
	//	}
	//	else
	//	{
	//		if (entity.rotationDir < 0.7)
	//		{
	//			entity.rotationDir += rotSpeed;
	//		}
	//		else if (entity.rotationDir > 0.9)
	//		{
	//			entity.rotationDir -= rotSpeed;
	//		}
	//		else
	//		{
	//			entity.rotationDir = 0.8;
	//		}
	//	}
	//
	//	break;
	//case LEFT_UP:
	//	rotateRight = false;
	//	rotateUp = false;
	//	rotateLeft = true;
	//	if (entity.rotationDir < 3.9)
	//	{
	//		entity.rotationDir += rotSpeed;
	//	}
	//	else if (entity.rotationDir > 4.1)
	//	{
	//		entity.rotationDir -= rotSpeed;
	//	}
	//	else
	//	{
	//		entity.rotationDir = 4.0;
	//	}
	//	break;
	//case LEFT_DOWN:
	//	rotateRight = false;
	//	rotateUp = false;
	//	rotateLeft = true;
	//	if (entity.rotationDir < 5.4)
	//	{
	//		entity.rotationDir += rotSpeed;
	//	}
	//	else if (entity.rotationDir > 5.6)
	//	{
	//		entity.rotationDir -= rotSpeed;
	//	}
	//	else
	//	{
	//		entity.rotationDir = 5.5;
	//	}
	//	break;
	//}
}