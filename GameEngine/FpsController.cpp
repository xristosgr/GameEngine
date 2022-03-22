#include "FpsController.h"

FpsController::FpsController()
{
	isJumping = false;
	timer.Start();
	currRotation = RotationEnum::UP;
	//gravity = -0.2f;
}

void FpsController::MouseMovement(float& dt, Entity& entity, Keyboard& keyboard, Mouse& mouse, Camera& camera)
{
	const float cameraSpeed = 0.015f;
	const float cameraRotSpeed = 0.002f;

	DirectX::XMFLOAT4 rightFloat4;
	DirectX::XMStoreFloat4(&rightFloat4, camera.GetRightVector());
	DirectX::XMFLOAT4 forwardFloat4;
	DirectX::XMStoreFloat4(&forwardFloat4, camera.GetForwardVector());

	while (!mouse.EventBufferIsEmpty())
	{

		if (camera.PossessCharacter)
		{
			MouseEvent me = mouse.ReadEvent();

			if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
			{

				camera.AdjustRotation(static_cast<float>(me.GetPosY()) * cameraRotSpeed, static_cast<float>(me.GetPosX()) * cameraRotSpeed, 0.0f, true);
				camera.AdjustPosition(rightFloat4.x * -cameraSpeed * static_cast<float>(me.GetPosX()), forwardFloat4.y * -cameraSpeed * static_cast<float>(me.GetPosY()), rightFloat4.z * -cameraSpeed * static_cast<float>(me.GetPosX()));
			}
		}
	}
}

void FpsController::Movement(float& dt, float gravity, Entity* entity, Keyboard& keyboard, Mouse& mouse, Camera& camera)
{		
	if (!entity)
		return;

	if (!entity->physicsComponent.aActor)
		return;
	
	entity->isMovingLeft = false;

	DirectX::XMFLOAT4 forwardDir;
	DirectX::XMStoreFloat4(&forwardDir, camera.vec_forward);
	DirectX::XMFLOAT4 rightDir;
	DirectX::XMStoreFloat4(&rightDir, camera.vec_right);


	float velocity = 5.0;
	float moveX = 0.0f;
	float moveZ = 0.0f;


	entity->physicsComponent.trans = entity->physicsComponent.aActor->getGlobalPose();

	entity->physicsComponent.trans.q = physx::PxQuat((camera.rot.y), physx::PxVec3(0, 1, 0));



	entity->matrix_rotate = DirectX::XMMatrixRotationRollPitchYaw(entity->rot.x, entity->rot.y, entity->rot.z);

	entity->matrix_rotate *= DirectX::XMMatrixRotationAxis(DirectX::XMVECTOR{ 0, 1, 0 }, camera.rot.y);

	if (keyboard.KeyIsPressed(VK_F8))
	{
		camera.PossessCharacter = true;
	}
	if (keyboard.KeyIsPressed(VK_F9))
	{
		camera.PossessCharacter = false;
	}


	entity->pos = DirectX::XMFLOAT3(entity->physicsComponent.trans.p.x, entity->physicsComponent.trans.p.y, entity->physicsComponent.trans.p.z);
	if (camera.PossessCharacter)
	{

		camera.SetPosition(DirectX::XMVECTOR{ entity->physicsComponent.trans.p.x, entity->physicsComponent.trans.p.y + 0.4f ,entity->physicsComponent.trans.p.z });



		if (keyboard.KeyIsPressed('S') && (keyboard.KeyIsPressed('A')))
		{
			currRotation = RotationEnum::LEFT_DOWN;
			entity->isMovingRight = true;
			entity->isMovingLeft = true;

			moveX = -velocity * forwardDir.x - velocity * rightDir.x;
			moveZ = -velocity * forwardDir.z - velocity * rightDir.z;


	
			entity->model.currAnim = 1;
		}
		else if (keyboard.KeyIsPressed('S') && (keyboard.KeyIsPressed('D')))
		{
			currRotation = RotationEnum::RIGHT_DOWN;
			entity->isMovingRight = true;

			moveX = -velocity * forwardDir.x + velocity * rightDir.x;
			moveZ = -velocity * forwardDir.z + velocity * rightDir.z;

			entity->model.currAnim = 1;
		}
		else if (keyboard.KeyIsPressed('W') && (keyboard.KeyIsPressed('A')))
		{
			currRotation = RotationEnum::LEFT_UP;
			entity->isMovingRight = true;
			entity->isMovingLeft = true;
			moveX = velocity * forwardDir.x - velocity * rightDir.x;
			moveZ = velocity * forwardDir.z - velocity * rightDir.z;


			entity->model.currAnim = 1;
		}
		else if (keyboard.KeyIsPressed('W') && (keyboard.KeyIsPressed('D')))
		{
			currRotation = RotationEnum::RIGHT_UP;
			entity->isMovingRight = true;

			moveX = velocity * forwardDir.x + velocity * rightDir.x;
			moveZ = velocity * forwardDir.z + velocity * rightDir.z;

			entity->model.currAnim = 1;
		}
		else
		{
			entity->isMovingRight = false;
			if (keyboard.KeyIsPressed('W'))
			{
				currRotation = RotationEnum::UP;
				entity->isMovingFront = true;

				moveX = velocity * forwardDir.x;
				moveZ = velocity * forwardDir.z;

			}
			else if (keyboard.KeyIsPressed('S'))
			{
				currRotation = RotationEnum::DOWN;
				entity->isMovingFront = true;

				moveX = -velocity * forwardDir.x;
				moveZ = -velocity * forwardDir.z;


				entity->model.currAnim = 1;

			}
			else
			{
				entity->isMovingFront = false;
			}


			if (keyboard.KeyIsPressed('D'))
			{

				currRotation = RotationEnum::RIGHT;
				entity->isMovingRight = true;

				moveX = velocity * rightDir.x;
				moveZ = velocity * rightDir.z;



			}
			else if (keyboard.KeyIsPressed('A'))
			{
				currRotation = RotationEnum::LEFT;
				entity->isMovingRight = true;
				entity->isMovingLeft = true;


				moveX = -velocity * rightDir.x;
				moveZ = -velocity * rightDir.z;


				entity->model.currAnim = 1;

			}
			else
			{
				entity->isMovingRight = false;
			}


			if ((!entity->isMovingFront && !entity->isMovingRight) || entity->isFalling)
			{
				entity->model.currAnim = 0;



			}
			else if (entity->isMovingFront || entity->isMovingRight)
			{
				

			}
		}


		
		if (!entity->isFalling)
		{
			if (canPressSpace)
			{
				if (keyboard.KeyIsPressed(VK_SPACE))
				{
					canPressSpace = false;
					timer.Restart();
					isJumping = true;
					entity->physicsComponent.aActor->addForce(physx::PxVec3(moveX, 200.0f, moveZ), physx::PxForceMode::eIMPULSE);
				}
			}
			
			if (!keyboard.KeyIsPressed(VK_SPACE))
			{
				//OutputDebugStringA("FREED!!!!!!!\n");
				canPressSpace = true;
			}
		}
		

		if (timer.GetMilisecondsElapsed() > 5.0f*dt)
		{
			if (!entity->isFalling && isJumping)
			{
				isJumping = false;
			}
		}
		if (!isJumping)
		{
			if (entity->isFalling)
			{
				entity->physicsComponent.aActor->setLinearVelocity(physx::PxVec3(moveX, gravity, moveZ));
			}
			else
			{
				isJumping = false;
				entity->physicsComponent.aActor->setLinearVelocity(physx::PxVec3(moveX, 0.0f, moveZ));
			}
		}
		entity->physicsComponent.aActor->setGlobalPose(entity->physicsComponent.trans);
	}
}
