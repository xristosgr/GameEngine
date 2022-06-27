#pragma once
#include "WindowContainer.h"
#include "AppTimer.h"
#include "Renderer.h"
#include "PhysicsHandler.h"
#include <future>
#include "SaveSystem.h"
#include "CollisionObject.h"
#include "FpsController.h"
#include "TpsController.h"
#include "GridClass.h"
#include <thread>
#include "NavMeshClass.h"

#include <algorithm>
#include "Camera.h"
#include "Sky.h"
#include"SoundComponent.h"

class Engine : virtual WindowContainer
{
public:
	Engine();
	template<class T>
	Engine(T& lhs, T& rhs);

	bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	bool ProcessMessages();
	void Update(int width, int height);
	void RenderFrame(float& dt, float& fps);

private:
	void AddEntity(std::string& _inName, bool& isAnimated, bool& bConvertCordinates);
	void AddPhysicsComp(Entity& entity);
	void AddLight();
	void AddPointLight();
	void AddCollisionObject();
	void ObjectsHandler(float& dt);
	void AIHandler(float& dt);
	void CopyPasteEntity();
	void CopyPasteLight();
	void CopyPastePointLight();

	void PlayerLogic(float& dt);
	void GameSounds();

protected:
	AppTimer timer;

private:
	Sky sky;
	std::vector<Entity> entities;

	std::vector<Entity*> AIEntities;
	std::vector<Light> lights;
	std::vector<Light> pointlights;
	std::vector<CollisionObject> collisionObjects;
	std::vector<SoundComponent*> sounds;

	Camera camera;
	Renderer renderer;
	PhysicsHandler physicsHandler;

	int width, height;

	std::future<void> asyncAddEntity;
	std::future<void> asyncLoadPhysics;
	SaveSystem saveSystem;

	TpsController tpsPlayerController;
	FpsController fpsPlayerController;

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



	bool copyEntity = false;
	bool pasteEntity = false;
	bool bCanCopy = true;
	bool bCanPaste = true;

	struct CopiedEntityData
	{
		physx::PxTransform trans;
		std::string FilePath;
		std::vector<std::string> AnimFilePaths;
		bool isTransparent;
		bool isAnimated;
		bool isAi;
		bool isTextured;
		bool bConvertCordinates;
		physx::PxReal mass;
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 scale;
		DirectX::XMFLOAT3 rot;
		DirectX::XMFLOAT3 frustumScale;
		bool isCharacter;
		bool isPlayer;
		bool isWalkable;
		bool isObstacle;
		bool bRender;
		DirectX::XMFLOAT3 offsetPos;
		DirectX::XMFLOAT3 modelPos;
		physx::PxQuat physics_rot;
		physx::PxVec3 physics_scale;
		bool isfrustumEnabled;
		bool isEmissive;
		DirectX::XMFLOAT3 emissiveColor;
		PhysicsShapeEnum physicsShapeEnum;
	};
	struct CopiedLightData
	{
		bool isLightEnabled;
		bool bShadow;
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 scale;
		DirectX::XMFLOAT4 lightColor;
		DirectX::XMFLOAT3 direction;
		DirectX::XMFLOAT3 SpotDir;
		float radius;
		float cutOff;
		float lightType;
		float nearZ;
		float farZ;
		float fov;
		float dimensions;
	};

	CopiedEntityData copiedEntityData;
	CopiedLightData copiedLightData;
	CopiedLightData copiedPointLightData;



	SoundComponent backGroundSound;

private:
		DirectX::XMFLOAT3 camTempPos, camTempRot;
		bool bPbrRenderCamFix = true;
};
