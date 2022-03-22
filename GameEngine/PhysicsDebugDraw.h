#pragma once
#include "DX11Includes.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"
#include "Camera.h"
#include "Vertex.h"
#include "Mesh.h"
#include <PxPhysicsAPI.h>
class PhysicsDebugDraw
{
public:
	PhysicsDebugDraw();
	void DebugDraw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>* cb_vs_vertexshader, const physx::PxDebugLine& line, Camera& camera);

	Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> mDeviceContext;
	std::vector<Vertex> vertices;
	Mesh* mesh;
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;
	ConstantBuffer<CB_VS_vertexshader>* m_cb_vs_vertexshader;
};

