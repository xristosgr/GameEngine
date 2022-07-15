#include "PhysicsHandler.h"
#include "ErrorLogger.h"


PhysicsHandler::PhysicsHandler()
{
	timer.Start();
	isMouseHover = false;
}

PhysicsHandler::~PhysicsHandler()
{
	ShutDown();
}

void PhysicsHandler::Initialize(Camera& camera)
{
	static physx::PxDefaultErrorCallback gDefaultErrorCallback;
	static physx::PxDefaultAllocator gDefaultAllocatorCallback;

	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	if (!mFoundation)
		ErrorLogger::Log("PxCreateFoundation failed!");

	bool recordMemoryAllocations = true;
	mPvd = PxCreatePvd(*mFoundation);
	const char* PVD_HOST = "Host";
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	if (!transport)
		ErrorLogger::Log("PxDefaultPvdSocketTransportCreate failed!");

	physx::PxTolerancesScale scale;
	scale.length = 100;        // typical length of an object
	scale.speed = 981;         // typical speed of an object, gravity*1s is a reasonable choice
	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, physx::PxTolerancesScale(), recordMemoryAllocations, mPvd);

	if (!mPhysics)
		ErrorLogger::Log("PxPhysics failed!");

	physx::PxCookingParams params(mPhysics->getTolerancesScale());
	//params.meshPreprocessParams = physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
	//params.meshPreprocessParams = physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
	params.meshPreprocessParams = physx::PxMeshPreprocessingFlag::eWELD_VERTICES;
	params.meshPreprocessParams = physx::PxMeshPreprocessingFlag::eFORCE_32BIT_INDICES;
	params.suppressTriangleMeshRemapTable = true;
	params.areaTestEpsilon = 0.1f;
	params.convexMeshCookingType = physx::PxConvexMeshCookingType::eQUICKHULL;
	params.meshWeldTolerance = 1.0f;
	params.planeTolerance = 0.001f;
	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, params);
	if (!mCooking)
		ErrorLogger::Log("PxCreateCooking failed!");

	if (!PxInitExtensions(*mPhysics, mPvd))
		ErrorLogger::Log("PxInitExtensions failed!");

	physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	if (!sceneDesc.isValid())
		ErrorLogger::Log("PxSceneDesc failed!");

	aScene = mPhysics->createScene(sceneDesc);

	if (!aScene)
		ErrorLogger::Log("createScene failed!");

	//manager = PxCreateControllerManager(*aScene);
	//
	//if (!manager)
	//	ErrorLogger::Log("PxCreateControllerManager failed!");


	aScene->setVisualizationCullingBox(physx::PxBounds3(physx::PxVec3(camera.pos.x - 40.0f, camera.pos.y - 40.0f, camera.pos.z - 40.0f), physx::PxVec3(camera.pos.x + 40.0f, camera.pos.y + 40.0f, camera.pos.z + 40.0f)));
	aScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
	//aScene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 2.0f);
	aScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 2.0f);
	aScene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
	aScene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
	//aScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_AABBS, 2.0f);
	//aScene->setVisualizationParameter(physx::PxVisualizationParameter::eCULL_BOX, 2.0f);
}

void PhysicsHandler::CreatePhysicsComponents(std::vector<Entity>& entities, std::vector<CollisionObject>& collisionObject)
{
	for (int i = 0; i < entities.size(); ++i)
	{
		if (entities[i].physicsComponent.physicsShapeEnum != PhysicsShapeEnum::NONE && !entities[i].physicsComponent.isCharacter)
		{
			entities[i].CreatePhysicsComponent(*mPhysics, *aScene, mCooking);
		}

		if (entities[i].physicsComponent.isCharacter)
		{
			entities[i].physicsComponent.CreateController(*mPhysics, *aScene, physx::PxVec3(entities[i].pos.x, entities[i].pos.y, entities[i].pos.z), entities[i].entityName);
		}
	}

	for (int i = 0; i < collisionObject.size(); ++i)
	{
		collisionObject[i].CreatePhysicsComponent(*mPhysics, *aScene);
	}
}

void PhysicsHandler::MouseRayCast(std::vector<Entity>& entities, Camera& camera, Mouse& mouse, Keyboard& keyboard, int& width, int& height, int& selected_list_object)
{
	float pointX, pointY;
	DirectX::XMMATRIX viewMatrix, inverseViewMatrix;
	DirectX::XMFLOAT3 direction;

	pointX = ((2.0f * (float)mouse.GetPosX()) / (float)width) - 1.0f;
	pointY = (((2.0f * (float)mouse.GetPosY()) / (float)height) - 1.0f) * -1.0f;

	DirectX::XMFLOAT4X4 projection;
	DirectX::XMStoreFloat4x4(&projection, camera.GetProjectionMatrix());
	pointX = pointX / projection._11;
	pointY = pointY / projection._22;

	viewMatrix = camera.GetViewMatrix();
	inverseViewMatrix = DirectX::XMMatrixInverse(nullptr, viewMatrix);

	DirectX::XMFLOAT4X4 invView;
	DirectX::XMStoreFloat4x4(&invView, inverseViewMatrix);

	direction.x = (pointX * invView._11) + (pointY * invView._21) + invView._31;
	direction.y = (pointX * invView._12) + (pointY * invView._22) + invView._32;
	direction.z = (pointX * invView._13) + (pointY * invView._23) + invView._33;

	physx::PxVec3 origin = physx::PxVec3(camera.pos.x, camera.pos.y, camera.pos.z);

	physx::PxVec3 unitDir;
	unitDir = physx::PxVec3(direction.x, direction.y, direction.z);


	unitDir.normalize();

	physx::PxReal maxDistance = 100000.0f;
	if (width == 1920 && height == 1080)
		maxDistance = 1000000.0f;
	physx::PxRaycastBuffer hit;

	bool status = aScene->raycast(origin, unitDir, maxDistance, hit);

	
	if (mouse.IsLeftDown())
	{
		if (status)
		{
			selected_list_object = -1;
			//std::string s = "X = " + std::to_string(hit.block.position.x) + " |Y = " + std::to_string(hit.block.position.y) + " |Z = " + std::to_string(hit.block.position.z) + "\n";
			//OutputDebugStringA(s.c_str());

			physx::PxShape* _shape = nullptr;
			hit.block.actor->getShapes(&_shape, hit.block.actor->getNbShapes());

			_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);

			for (int i = 0; i < entities.size(); ++i)
			{
				if (!entities[i].isDeleted)
				{
					if (entities[i].physicsComponent.aActor)
					{
						if (entities[i].physicsComponent.aActor->getName() == hit.block.actor->getName())
						{
							entities[i].isSelected = true;
						}
					}
					else if (entities[i].physicsComponent.aStaticActor)
					{
						if (entities[i].physicsComponent.aStaticActor->getName() == hit.block.actor->getName())
						{
							entities[i].isSelected = true;
						}
					}
				}
				
			}
		}
	}
	if (mouse.IsRightDown())
	{
		if (status)
		{
			selected_list_object = -1;

			physx::PxShape* _shape = nullptr;
			hit.block.actor->getShapes(&_shape, hit.block.actor->getNbShapes());
		    _shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);

			for (int i = 0; i < entities.size(); ++i)
			{
				if (!entities[i].isDeleted)
				{
					if (entities[i].physicsComponent.aActor)
					{
						if (entities[i].physicsComponent.aActor->getName() == hit.block.actor->getName())
						{
							entities[i].isSelected = false;
							//bMousePicked = false;
						}
					}
					else if (entities[i].physicsComponent.aStaticActor)
					{
						if (entities[i].physicsComponent.aStaticActor->getName() == hit.block.actor->getName())
						{
							entities[i].isSelected = false;
							//bMousePicked = false;
						}
					}
				}
				
			}
		}
	}
}


void PhysicsHandler::FallCheck(Entity* character)
{
	if (!character)
		return;


	physx::PxVec3 origin = physx::PxVec3(character->physicsComponent.trans.p.x, character->physicsComponent.trans.p.y - 0.46, character->physicsComponent.trans.p.z);

	physx::PxVec3 unitDir;
	unitDir = physx::PxVec3(0, -1, 0);

	unitDir.normalize();
	physx::PxReal maxDistance = 0.42f;
	physx::PxRaycastBuffer hit;

	physx::PxQueryFilterData filterData(physx::PxQueryFlag::eSTATIC);

	bool status = aScene->raycast(origin, unitDir, maxDistance, hit, physx::PxHitFlag::eDEFAULT, filterData);
	
	if (status)
	{
		//OutputDebugStringA(hit.block.actor->getName());

		character->isFalling = false;
	}
	else
	{
		character->isFalling = true;
	}
	
	//OutputDebugStringA(("FALLING = " + std::to_string(character->isFalling) + "\n").c_str());
	//OutputDebugStringA(character->entityName.c_str());
	//OutputDebugStringA("\n");
	//OutputDebugStringA(("	FALLING = " + std::to_string(character->isFalling) + "\n").c_str());
	
	hit.finalizeQuery();


}

void PhysicsHandler::NavMeshRayCast(GridClass& grid, std::vector<Entity>& entities, std::vector<CollisionObject>& collisionObjects)
{
	for (int i = 0; i < grid.nodes.size(); ++i)
	{
		physx::PxVec3 origin[9];

		origin[0] = physx::PxVec3(grid.nodes[i].pos.x, grid.nodes[i].pos.y, grid.nodes[i].pos.z);
		origin[1] = physx::PxVec3(grid.nodes[i].pos.x + 0.6, grid.nodes[i].pos.y, grid.nodes[i].pos.z - 0.6);
		origin[2] = physx::PxVec3(grid.nodes[i].pos.x - 0.6, grid.nodes[i].pos.y, grid.nodes[i].pos.z + 0.6);
		origin[3] = physx::PxVec3(grid.nodes[i].pos.x + 0.6, grid.nodes[i].pos.y, grid.nodes[i].pos.z + 0.6);
		origin[4] = physx::PxVec3(grid.nodes[i].pos.x - 0.6, grid.nodes[i].pos.y, grid.nodes[i].pos.z - 0.6);

		origin[5] = physx::PxVec3(grid.nodes[i].pos.x, grid.nodes[i].pos.y, grid.nodes[i].pos.z - 0.6);
		origin[6] = physx::PxVec3(grid.nodes[i].pos.x, grid.nodes[i].pos.y, grid.nodes[i].pos.z + 0.6);

		origin[7] = physx::PxVec3(grid.nodes[i].pos.x + 0.6, grid.nodes[i].pos.y, grid.nodes[i].pos.z);
		origin[8] = physx::PxVec3(grid.nodes[i].pos.x - 0.6, grid.nodes[i].pos.y, grid.nodes[i].pos.z);


		physx::PxVec3 unitDir;
		unitDir = physx::PxVec3(0, -1, 0);

		unitDir.normalize();
		physx::PxReal maxDistance = 1.0f;
		physx::PxRaycastBuffer hit[9];

		physx::PxHitFlags _hitFlags;
		_hitFlags.set(physx::PxHitFlag::eDEFAULT);

		physx::PxQueryFilterData _QueryFilterData;
		_QueryFilterData.flags.set(physx::PxQueryFlag::eSTATIC);
		bool status[9];

		for (int j = 0; j < 9; ++j)
		{
			status[j] = aScene->raycast(origin[j], unitDir, maxDistance, hit[j], _hitFlags, _QueryFilterData);
		}
	
		for (int j = 0; j < entities.size(); ++j)
		{
			for (int k = 0; k < 9; ++k)
			{
				if (status[k])
				{
					if (entities[j].physicsComponent.aStaticActor)
					{
						if (hit[k].block.actor->getName() == entities[j].physicsComponent.aStaticActor->getName())
						{
							if (entities[j].isWalkable)
							{
								grid.nodes[i].isValidPath = true;
							}
							else
							{
								grid.nodes[i].isValidPath = false;
							}
							if (entities[j].isObstacle)
							{
								grid.nodes[i].isValidPath = false;
							}
						}
					}
					hit[k].finalizeQuery();
				}
				else
				{
					grid.nodes[i].isValidPath = false;
				}
			}
		}

		for (int j = 0; j < collisionObjects.size(); ++j)
		{

			for (int k = 0; k < 9; ++k)
			{
				if (status[k])
				{
					if (collisionObjects[j].physicsComponent.aStaticActor)
					{
						if (hit[k].block.actor->getName() == collisionObjects[j].physicsComponent.aStaticActor->getName())
						{
							grid.nodes[i].isValidPath = false;

						}
					}
					hit[k].finalizeQuery();
				}
				else
				{
					grid.nodes[i].isValidPath = false;
				}
			}	
		}
	}
	finishNavMeshRayCast = true;
	//OutputDebugStringA("FINISH RAY NAVMESSH!!\n");
}

void PhysicsHandler::LineOfSightToPlayer(Entity* character, Entity* player)
{
	if (!character || !player)
		return;
	character->physicsComponent.trans = character->physicsComponent.aActor->getGlobalPose();

	character->physicsComponent.hasLineOfSight = false;
	auto _transChar = character->physicsComponent.trans;
	
	player->physicsComponent.trans = player->physicsComponent.aActor->getGlobalPose();
	auto _transPlayer = player->physicsComponent.trans;
	float angle = atan2(_transPlayer.p.x - _transChar.p.x, _transPlayer.p.z - _transChar.p.z);

	float _rotX = std::sin(angle);
	float _rotZ = std::cos(angle);
	physx::PxVec3 origin;
	origin = physx::PxVec3(character->physicsComponent.trans.p.x + _rotX, character->physicsComponent.trans.p.y - 0.1f, character->physicsComponent.trans.p.z+ _rotZ);

	physx::PxVec3 unitDir;
	unitDir = physx::PxVec3(_rotX, 0, _rotZ);

	unitDir.normalize();


	physx::PxReal maxDistance = 100.0f;
	physx::PxRaycastBuffer hit;
	const physx::PxRaycastHit* _rayHit;


	bool status = aScene->raycast(origin, unitDir, maxDistance, hit);

	if (status)
	{
		//OutputDebugStringA(hit.block.actor->getName());
		if (hit.block.actor->getName() == player->entityName)
			character->physicsComponent.hasLineOfSight = true;
		else
			character->physicsComponent.hasLineOfSight = false;
	}
	else
	{
		character->physicsComponent.hasLineOfSight = false;
	}


	//OutputDebugStringA(std::to_string(character->physicsComponent.hasLineOfSight).c_str());
	//OutputDebugStringA("\n");
	hit.finalizeQuery();
}


bool PhysicsHandler::advance(float& dt,float& fps, Camera& camera)
{
	aScene->setVisualizationCullingBox(physx::PxBounds3(physx::PxVec3(camera.pos.x - 40.0f, camera.pos.y - 40.0f, camera.pos.z - 40.0f), physx::PxVec3(camera.pos.x + 40.0f, camera.pos.y + 40.0f, camera.pos.z + 40.0f)));
	aScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
	//aScene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 2.0f);
	aScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 2.0f);
	aScene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
	aScene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
	//aScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_AABBS, 2.0f);
	//aScene->setVisualizationParameter(physx::PxVisualizationParameter::eCULL_BOX, 2.0f);
	//aScene->setVisualizationParameter(physx::PxVisualizationParameter::)
	mStepSize = 1.0f/fps;

	//mAccumulator = 0.0f;

	mAccumulator += dt;
	if (mAccumulator < mStepSize)
	{
		//mAccumulator = 0.0f;
		return false;
	}
		

	mAccumulator -= mStepSize;

	aScene->simulate(mStepSize);
	aScene->fetchResults(true);
	
	return true;
}

void PhysicsHandler::ShutDown()
{
	if (mPhysics)
		mPhysics->release();

	PxCloseExtensions();

	if (mFoundation)
		mFoundation->release();
}


