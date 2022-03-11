#include "PhysicsCharacterController.h"
#include "ErrorLogger.h"

PhysicsCharacterController::PhysicsCharacterController()
{
}

void PhysicsCharacterController::CreateCharacterController(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxControllerManager& manager, physx::PxExtendedVec3 pos, const char* name)
{
	physx::PxTransform relativePose(physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 0, 1)));

	auto aMaterial = physics.createMaterial(0.0f, 0.0f, 0.0f);
	physx::PxCapsuleControllerDesc controllerDesc;
	controllerDesc.position = physx::PxExtendedVec3(pos.x, pos.y, pos.z);
	controllerDesc.slopeLimit = 0.15f;
	controllerDesc.scaleCoeff = 0.8f;
	controllerDesc.contactOffset = 0.000001f;
	controllerDesc.stepOffset = 0.03f;
	controllerDesc.invisibleWallHeight = 0.0f;
	controllerDesc.maxJumpHeight = 0.0f;
	controllerDesc.radius = 0.2;
	controllerDesc.height = 1.0f;
	controllerDesc.density = 1.0f;
	controllerDesc.material = aMaterial;
	controllerDesc.climbingMode = physx::PxCapsuleClimbingMode::eEASY;
	controllerDesc.upDirection = physx::PxVec3(0, 1, 0);
	controllerDesc.volumeGrowth = 1.5f;

	
	controllerDesc.behaviorCallback = nullptr;

	if (!controllerDesc.isValid())
	{
		ErrorLogger::Log("controllerDesc failed!");
		return;
	}
	physx::PxController* controller = manager.createController(controllerDesc);
	//manager.getScene().addActor(*manager.getController(0)->getActor());

	if (!controller)
		ErrorLogger::Log("controller failed!");

	manager.getController(manager.getNbControllers() - 1)->getActor()->setName(name);

	physx::PxShape* _shape;
	manager.getController(manager.getNbControllers() - 1)->getActor()->getShapes(&_shape, manager.getController(0)->getActor()->getNbShapes());
	_shape->setLocalPose(relativePose);
	_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);

}

void PhysicsCharacterController::UpdateController(physx::PxVec3& disp, float& dt)
{
}
