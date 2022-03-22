#include "RectShape.h"

RectShape::RectShape()
{
	scale = DirectX::XMFLOAT3(1.f, 1.0f, 1.0f);
	rot = DirectX::XMFLOAT3(0, 0,10.99555);
	pos = DirectX::XMFLOAT3(0, 0, 0.4285);
}

RectShape::~RectShape()
{
}

void RectShape::Initialize(ID3D11Device* device, UINT32 windowWidth, UINT32 windowHeight)
{
	//Vertex v[] =
	//{
	//	Vertex(-0.5f,-0.5f,0.0f,0.0f, 0.0f),
	//	Vertex(-0.5f, 0.5f,0.0f,1.0,0.0f),
	//	Vertex( 0.5f,  0.5f,0.0f,1.0f,1.0f),
	//	Vertex( 0.5f, -0.5f,0.0f, 0.0f,1.0f)
	//};
	float fov = 16.0f / 9.0f;
	Vertex v[] =
	{
		Vertex(-1.0f,-1.0f*fov,1.0f,0.0f, 0.0f),
		Vertex(-1.0f, 1.0f*fov,1.0f,1.0,0.0f),
		Vertex( 1.0f,  1.0f*fov,1.0f,1.0f,1.0f),
		Vertex( 1.0f, -1.0f*fov,1.0f, 0.0f,1.0f)
	};

	std::vector<DWORD> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(3);

	HRESULT hr = this->vertexBuffer.Initialize(device, v, ARRAYSIZE(v));
	COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");

	hr = this->indexBuffer.Initialize(device, indices.data(), indices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh.");
}

void RectShape::CreateTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string& filePath)
{

	__super::CreateTexture(device, deviceContext, filePath);
}

void RectShape::SetRenderTexture(ID3D11DeviceContext* deviceContext, RenderTexture& renderTexture)
{
	__super::SetRenderTexture(deviceContext, renderTexture);
}

void RectShape::Draw(ID3D11DeviceContext* deviceContext, Camera& camera, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	__super::Draw(deviceContext, camera, cb_vs_vertexshader);
}

void RectShape::Draw(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX& viewMatrix, DirectX::XMMATRIX& projectionMatrix, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	__super::Draw(deviceContext, viewMatrix, projectionMatrix, cb_vs_vertexshader);
}

void RectShape::DrawGUI(std::string name)
{
	__super::DrawGUI(name);
}
