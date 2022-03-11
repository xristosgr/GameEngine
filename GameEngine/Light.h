#pragma once
#include "DX11Includes.h"
#include "ConstantBufferTypes.h"
#include "ConstantBuffer.h"
#include "CubeShape.h"
#include "ModelLoader.h"

class Light
{
public:
	Light();
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	void SetupCamera(int windowWidth, int windowHeight);
	void UpdateCamera();
	void DrawGui(std::string name);
	void Draw(Camera& camera);

	void DrawVolume(Camera& camera);

	XMMATRIX lightViewMatrix;
	XMMATRIX lightProjectionMatrix;
	XMFLOAT3 posOffset;
	Camera camera;

	DirectX::XMFLOAT3 lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

	DirectX::XMFLOAT3 specularColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float specularPower = 1.0f;
	DirectX::XMFLOAT3 direction;
	DirectX::XMFLOAT3 SpotDir;
	DirectX::XMFLOAT3 offset;

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 rot;
	DirectX::XMFLOAT3 scale;

	DirectX::XMFLOAT3 conePos;
	DirectX::XMFLOAT3 coneRot;
	DirectX::XMFLOAT3 coneScale;

	float lightStrenth = 1.0f;
	float attenuation_a = 1.0f;
	float attenuation_b = 0.1f;
	float attenuation_c = 0.1f;
	float lightAttenuation = 1.0f;

	float radius;
	float cutOff;
	float lightType;

	bool isLightEnabled = true;
	float fov = 125.50f;
	float dimensions = 2;
	float nearZ = 0.1f;
	float farZ = 1000.0f;
	bool bShadow = true;


	RenderTexture m_shadowMap;

	std::string name = "Light attributes";

	//CubeShape cube;

	ModelLoader sphere;
	ModelLoader volume;
private:
	ID3D11DeviceContext* deviceContext;
	ID3D11Device* device;
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
};

