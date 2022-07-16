#pragma once
#include <d3d11.h>
#include "ErrorLogger.h"
#include<vector>
#include<DirectXMath.h>
#include "ModelLoader.h"
#include "Camera.h"
#include "RenderTexture.h"

class EnvironmentProbe
{
public:
	EnvironmentProbe();
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContex, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader,int width, int height);
	Camera* GetCamera();
	void UpdateCamera();
	void Draw(Camera& camera);
	void DrawGui(std::string name);

	DirectX::XMMATRIX viewMatrices[6];
	DirectX::XMMATRIX projectionMatrices[6];
	float fovs[6];

	Camera camera[6];
	DirectX::XMFLOAT3 direction[6];
	DirectX::XMFLOAT3 cameraPos;
	bool recalculate;

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 prevPos;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 rot;

	ModelLoader model;

	//RenderTexture probeMaps[6];
	RenderTexture environmentCubeMap;

	std::vector <ID3D11Texture2D*> cubeTex;

	std::vector <ID3D11ShaderResourceView*> cubeResourceView;
	int index;
};

