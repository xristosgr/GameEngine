#pragma once
#include "CubeShape.h"
#include "PhysicsComponent.h"

class CollisionObject
{
public:
	CollisionObject();
	void Initialize(ID3D11Device* device);
	void CreatePhysicsComponent(physx::PxPhysics& physics, physx::PxScene& scene);
	void Draw(ID3D11DeviceContext* deviceContext, Camera& camera, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	void DrawGUI(std::string name);
public:
	CubeShape cube;
	PhysicsComponent physicsComponent;
	std::string entityName;
	bool bRender;

	DirectX::XMFLOAT3 offsetPos;
};

