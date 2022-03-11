#pragma once
#include<string>
#include <fstream>
#include "Entity.h"
#include "Light.h"
#include "CollisionObject.h"

class SaveSystem
{
public:
	SaveSystem();
	void Save(std::vector<Entity>& entities, std::vector<Light>& lights, std::vector<Light>& pointLights, std::vector<CollisionObject>& collisionObject);
	void Load();
	void LoadEntityData(std::vector<Entity>& entities);
	void LoadLightData(std::vector<Light>& lights, std::vector<Light>& pointLights);
	void LoadCollisionObjectData(std::vector<CollisionObject>& collisionObject);

public:
	unsigned int entitiesCount, lightsCount, pointLightsCount, collisionObjectCount;
};

