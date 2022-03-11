#pragma once
#include <PxPhysicsAPI.h>
#include"PxDefaultErrorCallback.h"
#include"PxDefaultAllocator.h"
#include"AppTimer.h"
#include<vector>
#include "Entity.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Camera.h"
#include "CollisionObject.h"
#include "GridClass.h"


class PhysicsHandler
{
public:
	PhysicsHandler();
	~PhysicsHandler();

	void Initialize(Camera& camera);
	void CreatePhysicsComponents(std::vector<Entity>& entities, std::vector<CollisionObject>& collisionObject);
	void MouseRayCast(Camera& camera, Mouse& mouse, Keyboard& keyboard, int& width, int& height);
	void FallCheck(std::vector<Entity>& entities, Entity* character);
	void NavMeshRayCast(GridClass& grid, std::vector<Entity>& entities, std::vector<CollisionObject>& collisionObjects);
	void LineOfSightToPlayer(Entity* character, Entity* player, std::vector<Entity>& entities, std::vector<CollisionObject>& collisionObjects);
	bool advance(float& dt, float& fps, Camera& camera);
	void ShutDown();

public:
	physx::PxScene* aScene;
	physx::PxPhysics* mPhysics;
	physx::PxCooking* mCooking;
	//physx::PxControllerManager* manager;
	bool isMouseHover;
	bool finishNavMeshRayCast = false;
private:
	physx::PxFoundation* mFoundation;
	physx::PxPvd* mPvd;

	physx::PxDefaultCpuDispatcher* gDispatcher;


	float mAccumulator = 0.0f;
	float mStepSize = 1.0f / 60.0f;

	AppTimer timer;

};

