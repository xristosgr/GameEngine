#include "AIController.h"

AIController::AIController()
{
}

void AIController::MoveTo(float& dt, Entity* start, Entity* end, float& gravity)
{
	if (!end || !start)
		return;

	if (!start->physicsComponent.aActor || !end->physicsComponent.aActor)
		return;

	_force = 4.0f;
	radius = 8.0f;

	physx::PxVec3 moveVec = Move(dt, start,gravity);
	//physx::PxVec3 moveVec = MoveDirectlyTo(dt, v1, v2);
	float angle;
	if (start->physicsComponent.hasLineOfSight)
	{
		//***Rotate to face player
		angle = atan2(end->pos.x - v1.x, end->pos.z - v1.z);
	}
	else
	{
		//***Rotate to face next node
		angle = atan2(start->locToMove.x - v1.x, start->locToMove.z - v1.z);
	}



	start->physicsComponent.trans.q = physx::PxQuat((angle), physx::PxVec3(0, 1, 0));
	start->physicsComponent.trans.q = start->physicsComponent.trans.q.getNormalized();

	start->matrix_rotate = DirectX::XMMatrixRotationRollPitchYaw(start->rot.x, start->rot.y, start->rot.z);

	start->matrix_rotate *= DirectX::XMMatrixRotationAxis(DirectX::XMVECTOR{ 0,1, 0 }, angle);

	start->physicsComponent.aActor->setGlobalPose(start->physicsComponent.trans);

	//OutputDebugStringA(("FALLING = " + std::to_string(start->isFalling) + "\n").c_str());

	//OutputDebugStringA(("X = " + std::to_string(start->physicsComponent.trans.q.x)).c_str());
	//OutputDebugStringA((" | Y = " + std::to_string(start->physicsComponent.trans.q.y)).c_str());
	//OutputDebugStringA((" | Z = " + std::to_string(start->physicsComponent.trans.q.z) + "\n").c_str());
	//OutputDebugStringA((" | Z = " + std::to_string(start->physicsComponent.trans.q.z) + "\n").c_str());

	if (moveVec.x < -_force)
		moveVec.x = -_force;
	else if (moveVec.x > _force)
		moveVec.x = _force;

	if (moveVec.z < -_force)
		moveVec.z = -_force;
	else if (moveVec.z > _force)
		moveVec.z = _force;

	if (start->isFalling)
	{
		start->physicsComponent.aActor->setLinearVelocity(physx::PxVec3(moveVec.x, gravity, moveVec.z));
	}
	else
	{
		start->physicsComponent.aActor->setLinearVelocity(physx::PxVec3(moveVec.x, 0.0f, moveVec.z));
	}

	if (moveVec.x != 0 || moveVec.z != 0)
	{
		start->model.SetAnimIndex(1,true,4.2f);
	}
	else
	{
		//start->m_index = 0;
		start->model.SetAnimIndex(0, true, 4.2f);
	}
	//start->model.Update();
}

physx::PxVec3 AIController::MoveDirectlyTo(float& dt, physx::PxExtendedVec3& v1, physx::PxExtendedVec3& v2)
{
	physx::PxVec3 force = (v2 - v1) * _force;

	physx::PxVec3 diff = v1 - v2;

	float dist = diff.dot(diff);
	//OutputDebugStringA(("DIST = " + std::to_string(dist) + "\n").c_str());
	if (dist > radius)
	{
		//start->body->activate();
		return physx::PxVec3(force.x, -0.012f * dt, force.z);
	}
	else
	{
		return physx::PxVec3(0.0f, -0.012f * dt, 0.0f);
	}
}

physx::PxVec3 AIController::Move(float& dt, Entity* start,float& gravity)
{
	physx::PxVec3 startLoc = physx::PxVec3(v1.x, v1.y, v1.z);
	physx::PxVec3 force = (start->locToMove - startLoc) * _force;

	physx::PxVec3 diff = v1 - v2;

	float dist = diff.dot(diff);

	if (dist > radius)
	{
		return physx::PxVec3(force.x, gravity, force.z);
	}
	else
	{
		return physx::PxVec3(0.0f, 0, 0.0f);
	}
}
