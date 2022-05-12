#pragma once
#include "DX11Includes.h"
#include <DirectXMath.h>
#include "VertexBuffer.h"
#include "Vertex.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Camera.h"
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"
#include "AppTimer.h"
class InstancedShape
{
public:
	virtual void Initialize(ID3D11Device* device);
	virtual void CreateTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string& filePath);
	virtual void Draw(ID3D11DeviceContext* deviceContext, Camera& camera, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, ConstantBuffer<CB_VS_instanceShader>& cb_vs_instanceShader);
	int GetInstanceCount();
public:
	struct InstanceType
	{
		DirectX::XMFLOAT3 position;
	};

	VertexBuffer<Vertex> vertexBuffer;
	IndexBuffer indexBuffer;
	ID3D11Buffer* m_instanceBuffer;
	int m_instanceCount;

	Texture texture;

	DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3 rot = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3(1, 1, 1);

private:
	AppTimer timer;
};

