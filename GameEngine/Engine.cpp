#include "Engine.h"

Engine::Engine()
{
	player = nullptr;
}

bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	this->width = width;
	this->height = height;

	timer.Start();

	if (!this->render_window.Initialize(this, hInstance, window_title, window_class, width, height))
		return false;

	saveSystem.Load();

	for (int i = 0; i < saveSystem.entitiesCount; ++i)
	{
		entities.push_back(Entity());
	}

	physicsHandler.Initialize(camera);
	saveSystem.LoadEntityData(entities);
	saveSystem.LoadLightData(lights, pointlights);
	saveSystem.LoadCollisionObjectData(collisionObjects);

	if (!renderer.Initialize(this->render_window.GetHWND(), camera, width, height, entities, lights, pointlights))
		return false;

	for (int i = 0; i < collisionObjects.size(); ++i)
	{
		collisionObjects[i].Initialize(renderer.gfx11.device.Get());
	}

	renderer.InitScene(entities, lights,pointlights, camera);
	physicsHandler.CreatePhysicsComponents(entities, collisionObjects);
	
	grid.InitializeBoundsVolume(renderer.gfx11.device.Get());
	return true;
}

bool Engine::ProcessMessages()
{
	return this->render_window.ProcessMessages();
}

void Engine::Update(int width, int height)
{
	ImGuiIO& io = ImGui::GetIO();

	//float dt = timer.GetMilliseconds();

	float dt = 1000.0f / io.Framerate;
	float fps = io.Framerate;
	//OutputDebugStringA(("DT = " + std::to_string(dt) + "\n").c_str());
	timer.Restart();
	

	//while (!keyboard.CharBufferIsEmpty())
	//{
	//	unsigned char ch = keyboard.ReadChar();
	//}
	//
	//while (!keyboard.KeyBufferIsEmpty())
	//{
	//	KeyboardEvent kbe = keyboard.ReadKey();
	//	unsigned char keycode = kbe.GetKeyCode();
	//}
	
	if (keyboard.KeyIsPressed(VK_ESCAPE))
	{
		this->render_window.~RenderWindow();
	}

	float cameraSpeed = 0.001f;


	if (!camera.PossessCharacter)
	{
		while (!mouse.EventBufferIsEmpty())
		{
			MouseEvent me = mouse.ReadEvent();

			if (mouse.IsMiddleDown())
			{
				if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
				{
					camera.AdjustRotation(static_cast<float>(me.GetPosY()) * 0.004f, static_cast<float>(me.GetPosX()) * 0.004f, 0.0f, true);

				}

			}
		}

		if (keyboard.KeyIsPressed(VK_SHIFT))
		{
			cameraSpeed = 0.01;
		}

		if (keyboard.KeyIsPressed('W'))
		{
			camera.AdjustPosition(camera.GetForwardVector() * cameraSpeed * dt);
		}
		if (keyboard.KeyIsPressed('S'))
		{
			camera.AdjustPosition(camera.GetBackwardVector() * cameraSpeed * dt);
		}

		if (keyboard.KeyIsPressed('A'))
		{
			camera.AdjustPosition(camera.GetLeftVector() * cameraSpeed * dt);

		}
		if (keyboard.KeyIsPressed('D'))
		{
			camera.AdjustPosition(camera.GetRightVector() * cameraSpeed * dt);
		}

		if (keyboard.KeyIsPressed(VK_SPACE))
		{
			camera.AdjustPosition(0.0f, cameraSpeed * dt, 0.0f);
		}
		if (keyboard.KeyIsPressed('Q'))
		{
			camera.AdjustPosition(0.0f, -cameraSpeed * dt, 0.0f);
		}

	}
	

	if (keyboard.KeyIsPressed(VK_F5))
	{
		saveSystem.Save(entities, lights, pointlights, collisionObjects);
	}

	RenderFrame(dt, fps);
}

void Engine::RenderFrame(float& dt,float& fps)
{


	float pointX = (float)mouse.GetPosX();
	float pointY = (float)mouse.GetPosY();

	if (renderer.bAddEntity)
	{
		entities.push_back(Entity());
		entities[entities.size() - 1].filePath = renderer.inName;
		asyncAddEntity = std::async(std::launch::async, &Engine::AddEntity, this);
		renderer.bAddEntity = false;
	}
	if (renderer.bAddLight)
	{
		AddLight();
		renderer.bAddLight = false;
	}
	if (renderer.bAddPointLight)
	{
		AddPointLight();
		renderer.bAddPointLight = false;
	}

	if (renderer.bAddCollisionObject)
	{
		AddCollisionObject();
		renderer.bAddCollisionObject = false;
	}

	ObjectsHandler(dt);

	renderer.Render(camera, entities,physicsHandler, lights, pointlights, collisionObjects,grid, navMeshes, *physicsHandler.aScene);
	
	if (physicsHandler.advance(dt, fps, camera))
	{
		if (player)
		{
			playerController.MouseMovement(dt, *player, keyboard, mouse, camera);
			playerController.Movement(dt, physicsHandler.aScene->getGravity().y, *player, keyboard, mouse, camera);
		}

		if (!physicsHandler.isMouseHover)
			physicsHandler.MouseRayCast(camera, mouse, keyboard, this->width, this->height);
		physicsHandler.FallCheck(entities, player);
	}

	AIHandler(dt);

	if (renderer.bClear)
	{
		for (int i = 0; i < entities.size(); ++i)
		{
			entities[i].Clear(*physicsHandler.aScene);
		}
		entities.clear();
		renderer.bClear = false;
	}

}

void Engine::AddEntity()
{
	entities[entities.size() - 1].model.bConvertCordinates = renderer.bConvertCordinates;
	entities[entities.size() - 1].Intitialize(renderer.inName, renderer.gfx11.device.Get(), renderer.gfx11.deviceContext.Get(), renderer.gfx11.cb_vs_vertexshader, renderer.isAnimated);
}

void Engine::AddPhysicsComp(Entity& entity)
{
	if (entity.physicsComponent.aActor)
	{
		physicsHandler.aScene->removeActor(*entity.physicsComponent.aActor);
	}
	else if (entity.physicsComponent.aStaticActor)
	{
		physicsHandler.aScene->removeActor(*entity.physicsComponent.aStaticActor);
	}

	entity.CreatePhysicsComponent(*physicsHandler.mPhysics, *physicsHandler.aScene, physicsHandler.mCooking);
	entity.physicsComponent.bCreatePhysicsComp = false;

}

void Engine::AddLight()
{
	lights.push_back(Light());
	lights[lights.size() - 1].Initialize(renderer.gfx11.device.Get(), renderer.gfx11.deviceContext.Get(), renderer.gfx11.cb_vs_vertexshader);
	lights[lights.size() - 1].m_shadowMap.Initialize(renderer.gfx11.device.Get(), 1024, 1024);
	lights[lights.size() - 1].SetupCamera(renderer.gfx11.windowWidth, renderer.gfx11.windowHeight);
}
void Engine::AddPointLight()
{
	pointlights.push_back(Light());
	pointlights[pointlights.size() - 1].Initialize(renderer.gfx11.device.Get(), renderer.gfx11.deviceContext.Get(), renderer.gfx11.cb_vs_vertexshader);
	pointlights[pointlights.size() - 1].scale = XMFLOAT3(0.02f, 0.02f, 0.02f);
	pointlights[pointlights.size() - 1].cutOff = 0.1f;
	pointlights[pointlights.size() - 1].radius = 5.0f;
	pointlights[pointlights.size() - 1].lightColor = XMFLOAT3(5.0f, 5.0f, 5.0f);
}

void Engine::AddCollisionObject()
{
	collisionObjects.push_back(CollisionObject());
	collisionObjects[collisionObjects.size() - 1].Initialize(renderer.gfx11.device.Get());
	collisionObjects[collisionObjects.size() - 1].CreatePhysicsComponent(*physicsHandler.mPhysics, *physicsHandler.aScene);
}

void Engine::ObjectsHandler(float& dt)
{
	for (int i = 0; i < entities.size(); ++i)
	{
		if (entities[i].isDeleted)
			continue;

		if (entities[i].physicsComponent.isCharacter && entities[i].isPlayer)
		{
			player = &entities[i];
		}
		if (entities[i].physicsComponent.bCreatePhysicsComp)
		{
			AddPhysicsComp(entities[i]);
		}

		if (!renderer.runPhysics)
		{
			if (entities[i].physicsComponent.aActor)
			{
				entities[i].physicsComponent.aActor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, true);
			}
			else if (entities[i].physicsComponent.aStaticActor)
			{
				entities[i].physicsComponent.aStaticActor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, true);
			}
		}
		else
		{
			if (entities[i].physicsComponent.aActor)
			{
				entities[i].physicsComponent.aActor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, false);
			}
			else if (entities[i].physicsComponent.aStaticActor)
			{
				entities[i].physicsComponent.aStaticActor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, false);
			}
		}
		entities[i].physicsComponent.UpdatePhysics(*physicsHandler.mPhysics, *physicsHandler.aScene, physicsHandler.mCooking);

		entities[i].UpdatePhysics();
	}

	for (int i = 0; i < collisionObjects.size(); ++i)
	{

		collisionObjects[i].physicsComponent.UpdatePhysics(*physicsHandler.mPhysics, *physicsHandler.aScene, physicsHandler.mCooking);
		if (renderer.runPhysics)
			collisionObjects[i].bRender = false;
		else
			collisionObjects[i].bRender = true;
		collisionObjects[i].physicsComponent.aStaticActor->setName(collisionObjects[i].entityName.c_str());
	}
}

void Engine::AIHandler(float& dt)
{
	grid.SetupGridBounds();
	if (renderer.bCreateGrid)
	{
		renderer.bCreateGrid = false;
		grid.bCreateMesh = true;
	}

	if (grid.bCreateMesh)
	{
		AIEntities.clear();
		for (int i = 0; i < entities.size(); ++i)
		{
			if (entities[i].isAI)
			{
				AIEntities.push_back(&entities[i]);
			}
		}
		isNavMeshCreated = false;
		grid.isReady = false;

		grid.nodes.clear();
		navMeshes.clear();
		//grid_test.clear();
		navMeshes.resize(AIEntities.size());
		async_navMesh.resize(AIEntities.size());
		grid.Initialize(renderer.gfx11.device, renderer.gfx11.deviceContext, DirectX::XMMatrixIdentity(), renderer.gfx11.cb_vs_vertexshader);
		grid.bCreateMesh = false;
	}
	if (!isNavMeshCreated && grid.isReady)
	{
		physicsHandler.NavMeshRayCast(grid, entities, collisionObjects);
		//rayCastNavMesh_async = std::async(std::launch::async,&PhysicsHandler::NavMeshRayCast, &physicsHandler, std::ref(grid), std::ref(entities), std::ref(collisionObjects));
		//grid_test = std::thread(&PhysicsHandler::NavMeshRayCast, &physicsHandler, std::ref(grid), std::ref(entities), std::ref(collisionObjects));
		
		for (int i = 0; i < navMeshes.size(); ++i)
		{
			async_navMesh[i] = std::async(std::launch::async, &GridClass::CreatePathGrid, &grid, std::ref(navMeshes[i].validNodes));
		}
		isNavMeshCreated = true;
	}
	

	//OutputDebugStringA(("SIZE = " + std::to_string(AIEntities.size()) + "\n").c_str());

	if (renderer.runPhysics)
	{
		for (int i = 0; i < AIEntities.size(); ++i)
		{
			if (AIEntities[i])
			{
				if (AIEntities[i]->physicsComponent.aActor)
				{
					float gravity = physicsHandler.aScene->getGravity().y;
					physicsHandler.FallCheck(entities, AIEntities[i]);
					if (isNavMeshCreated)
					{
						navMeshes[i].CalculatePath(dt, AIEntities[i], player, enemyController, grid, gravity);
						//calculatePath_async = std::async(std::launch::async, &NavMeshClass::CalculatePath, &navMeshes[i], std::ref(dt), AIEntities[i], player, std::ref(enemyController), std::ref(grid), std::ref(gravity));
					}
						
				}
				//physicsHandler.LineOfSightCheck(AIEntities[i], entities, collisionObjects);
				physicsHandler.LineOfSightToPlayer(AIEntities[i], player, entities, collisionObjects);
			}
		}
	}
}
