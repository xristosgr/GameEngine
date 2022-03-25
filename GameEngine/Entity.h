#pragma once
#include "ModelLoader.h"
#include "PhysicsComponent.h"
#include "Frustum.h"
#include "ImGui/ImGuiFileBrowser.h"
#include <fstream>
#include "Camera.h"
#include "Mouse.h"
#include "Keyboard.h"

class Entity
{
public:
	Entity();

	bool Intitialize(const std::string filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContex, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, bool isAnimated);
	//void LoadAnimation();
	void CreatePhysicsComponent(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxCooking* cooking);
	void UpdatePhysics();
	void Update();
	void Draw(Camera& camera, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix);
	void AttachController(physx::PxController& characterController,bool& runPhysics);
	void MouseMove(Mouse& mouse, Keyboard& keyboard,Camera& camera);
	void SetupAttachment(Entity* entity, std::string boneName);
	void SetupAttachment(Entity* entity, bool bSetParent = false);
	void DrawGui(physx::PxScene& scene, std::vector<Entity>& entities);
	void Clear(physx::PxScene& scene);
public:
	std::string entityName = " ";
	PhysicsComponent physicsComponent;

public:
	ModelLoader model;
	Frustum frustum;
	Entity* parent;
public:
	bool isDeleted;

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 rot;
	DirectX::XMFLOAT3 frustumScale;
	DirectX::XMFLOAT3 modelPos;
	DirectX::XMFLOAT3 offsetPos;
	DirectX::XMFLOAT3 emissiveColor;


	DirectX::XMMATRIX matrix_rotate;
	DirectX::XMMATRIX worldMatrix,_worldMatrix;

	std::string filePath;
	std::string _filePath;
	bool isAnimated;
	bool bRender;
	bool isEmissive = false;
	bool isMovingFront = false;
	bool isMovingRight = false;
	bool isMovingLeft = false;
	bool isJumping = false;
	bool isFalling = false;

	bool bCreateController = false;
	bool isfrustumEnabled = true;
	bool isPlayer = false;
	bool isAI = false;

	float offsetY = 1.2f;
	float dirY = 0.0f;
	float maxDist = 0.0f;

	//AI
	physx::PxVec3 locToMove;
	std::vector<physx::PxVec3> locations;

	unsigned int m_index = 0;
	unsigned int m_indexNoSight = 0;

	bool isWalkable = false;
	bool isObstacle = false;

	bool isSelected = false;
	float rotationDir;

	std::string attachedBone;
	std::string parentName;

	bool bFlagForDeletion;
private:

	imgui_addons::ImGuiFileBrowser file_dialog;

	std::string inName;


	DirectX::XMVECTOR _pos, _scale, _rot;


	DirectX::XMMATRIX matrix_scale;
	//DirectX::XMMATRIX matrix_rotate;
	DirectX::XMMATRIX matrix_translate;


};

