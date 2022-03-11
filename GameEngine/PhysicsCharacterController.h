#pragma once
#include <PxPhysicsAPI.h>
#include <string>
class PhysicsCharacterController
{
public:
	PhysicsCharacterController();
	void CreateCharacterController(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxControllerManager& manager, physx::PxExtendedVec3 pos, const char* name);
	void UpdateController(physx::PxVec3& disp, float& dt);

};

