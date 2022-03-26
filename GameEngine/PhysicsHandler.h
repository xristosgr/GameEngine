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
#include <mutex>

class PhysicsHandler
{
public:
	PhysicsHandler();
	~PhysicsHandler();

	void Initialize(Camera& camera);
	void CreatePhysicsComponents(std::vector<Entity>& entities, std::vector<CollisionObject>& collisionObject);
	void MouseRayCast(std::vector<Entity>& entities, Camera& camera, Mouse& mouse, Keyboard& keyboard, int& width, int& height, int& selected_list_object);
	void FallCheck(Entity* character);
	void NavMeshRayCast(GridClass& grid, std::vector<Entity>& entities, std::vector<CollisionObject>& collisionObjects);
	void LineOfSightToPlayer(Entity* character, Entity* player);
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

	//std::unique_ptr<std::mutex> m_mutex;
};

