#pragma once
#include "ModelLoader.h"
#include "Camera.h"
#include "CubeShape.h"
class Sky
{
public:
	Sky();
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	void DrawGui(std::string name);
	void Draw(ID3D11DeviceContext* deviceContext, Camera& camera, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);

	//CubeShape cube;
	ModelLoader model;
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 rot;
	DirectX::XMFLOAT3 scale;

	DirectX::XMFLOAT3 color;
	DirectX::XMFLOAT4 apexColor;
	DirectX::XMFLOAT4 centerColor;
};

