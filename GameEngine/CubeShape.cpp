#include "CubeShape.h"

CubeShape::CubeShape()
{

	scale = DirectX::XMFLOAT3(1.f, 1.0f, 1.0f);
	rot = DirectX::XMFLOAT3(0, 0, 0);
	pos = DirectX::XMFLOAT3(0, 0, 0);
}

CubeShape::~CubeShape()
{
}

void CubeShape::Initialize(ID3D11Device* device)
{
	Vertex v[] =
	{
		Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f ), // +Y (top face)
		Vertex(1.0f, 1.0f, -1.0f,0.0f, 1.0f, 0.0f, 1.0f, 0.0f),
		Vertex(1.0f, 1.0f,  1.0f,0.0f, 1.0f, 0.0f, 1.0f, 1.0f),
		Vertex(-1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f),
	
		Vertex(-1.0f, -1.0f,  1.0f,0.0f, -1.0f, 0.0f, 0.0f, 0.0f ), // -Y (bottom face)
		Vertex(1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f ),
		Vertex(1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f ),
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f,0.0f, 1.0f),
	
		Vertex(1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f), // +X (right face)
		Vertex(1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f),
		Vertex(1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f),
		Vertex(1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f),
	
		Vertex(-1.0f,  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f), // -X (left face)
		Vertex(-1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f),
		Vertex(-1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f),
		Vertex(-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f),
	
		Vertex(-1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f), // +Z (front face)
		Vertex(1.0f,  1.0f, 1.0f , 0.0f, 0.0f, 1.0f, 1.0f, 0.0f),
		Vertex(1.0f, -1.0f, 1.0f , 0.0f, 0.0f, 1.0f, 1.0f, 1.0f),
		Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f),
	
		Vertex(1.0f,  1.0f, -1.0f ,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f), // -Z (back face)
		Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f),
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f),
		Vertex(1.0f, -1.0f, -1.0f ,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f),
	};
	
	DWORD indices[] =
	{
		0, 1, 2,
		0, 2, 3,
	
		4, 5, 6,
		4, 6, 7,
	
		8, 9, 10,
		8, 10, 11,
	
		12, 13, 14,
		12, 14, 15,
	
		16, 17, 18,
		16, 18, 19,
	
		20, 21, 22,
		20, 22, 23
	};

	

	HRESULT hr = this->vertexBuffer.Initialize(device, v, ARRAYSIZE(v));
	COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");

	hr = this->indexBuffer.Initialize(device, indices, ARRAYSIZE(indices));
	COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh.");

}

void CubeShape::CreateTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string& filePath)
{
	__super::CreateTexture(device, deviceContext, filePath);
}

void CubeShape::Draw(ID3D11DeviceContext* deviceContext, Camera& camera, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	__super::Draw(deviceContext, camera, cb_vs_vertexshader);
}

void CubeShape::Draw(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX& viewMatrix, DirectX::XMMATRIX& projectionMatrix, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	__super::Draw(deviceContext, viewMatrix, projectionMatrix, cb_vs_vertexshader);
}

void CubeShape::DrawGUI(std::string name)
{
	__super::DrawGUI(name);
}
