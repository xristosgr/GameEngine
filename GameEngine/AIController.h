#pragma once

#include "Entity.h"
#include <PxPhysicsAPI.h>

class AIController
{
public:
	AIController();
	void MoveTo(float& dt, Entity* start, Entity* end, float& gravity);

	physx::PxVec3 v1;
	physx::PxVec3 v2;

private:

	physx::PxVec3 MoveDirectlyTo(float& dt, physx::PxExtendedVec3& v1, physx::PxExtendedVec3& v2);
	physx::PxVec3 Move(float& dt, Entity* start, float& gravity);

	float _force = 0.5f;
	float radius = 15.0f;
};

