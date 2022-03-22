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
#include "RenderTexture.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include <string>

class Shapes
{
public:

	virtual void CreateTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string& filePath);
	virtual void SetRenderTexture(ID3D11DeviceContext* deviceContext, RenderTexture& renderTexture);
	virtual void Draw(ID3D11DeviceContext* deviceContext, Camera& camera, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	virtual void Draw(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX& viewMatrix, DirectX::XMMATRIX& projectionMatrix, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	virtual void DrawGUI(std::string name);


public:
	DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0,0,0);
	DirectX::XMFLOAT3 rot = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3(0, 0, 0);


	VertexBuffer<Vertex> vertexBuffer;
	IndexBuffer indexBuffer;

	Texture texture;
public:

};

