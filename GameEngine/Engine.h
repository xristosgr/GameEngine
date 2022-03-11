#pragma once
#include "WindowContainer.h"
#include "AppTimer.h"
#include "Renderer.h"
#include "PhysicsHandler.h"
#include <future>
#include "SaveSystem.h"
#include "CollisionObject.h"
#include "FpsController.h"
#include "GridClass.h"
#include <thread>
#include "NavMeshClass.h"

class Engine : virtual WindowContainer
{
public:
	Engine();
	bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	bool ProcessMessages();
	void Update(int width, int height);
	void RenderFrame(float& dt, float& fps);

private:
	void AddEntity();
	void AddPhysicsComp(Entity& entity);
	void AddLight();
	void AddPointLight();
	void AddCollisionObject();
	void ObjectsHandler(float& dt);
	void AIHandler(float& dt);
protected:
	AppTimer timer;

private:
	std::vector<Entity> entities;
	std::vector<Entity*> AIEntities;
	std::vector<Light> lights;
	std::vector<Light> pointlights;
	std::vector<CollisionObject> collisionObjects;

	Camera camera;
	Renderer renderer;
	PhysicsHandler physicsHandler;

	int width, height;

	std::future<void> asyncAddEntity;
	std::future<void> asyncLoadPhysics;
	SaveSystem saveSystem;

	FpsController playerController;

	Entity* player;

	AIController enemyController;
	std::vector<NavMeshClass> navMeshes;

	GridClass grid;
	bool isNavMeshCreated = false;
	std::thread grid_test;

	std::future<void> rayCastNavMesh_async;
	std::vector< std::future<void>> async_navMesh;


	bool initScene = true;

	bool bModelsLoaded = false;
};