#pragma once

#include <DirectXMath.h>

#define NO_LIGHTS 20
#define NO_POINT_LIGHTS 2000

struct CB_VS_vertexshader
{
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;

	DirectX::XMMATRIX bones_transform[100];
};

struct CB_VS_lightsShader
{
	DirectX::XMMATRIX lightViewMatrix[NO_LIGHTS];
	DirectX::XMMATRIX lightProjectionMatrix[NO_LIGHTS];
	unsigned int lightsSize;
};

struct CB_VS_windowParams
{
	float window_width;
	float window_height;
};

struct CB_PS_lightsShader
{
	DirectX::XMFLOAT4 dynamicLightPosition[NO_LIGHTS];
	DirectX::XMFLOAT4 dynamicLightColor[NO_LIGHTS];
	DirectX::XMFLOAT4 SpotlightDir[NO_LIGHTS];
	DirectX::XMFLOAT4 cameraPos;
	DirectX::XMFLOAT4 lightType[NO_LIGHTS];
	//DirectX::XMFLOAT4 acceptedDistShadowAndLight;
	unsigned int lightsSize;
};

struct CB_PS_pointLightsShader
{
	DirectX::XMFLOAT4 dynamicLightPosition[NO_POINT_LIGHTS];
	DirectX::XMFLOAT4 dynamicLightColor[NO_POINT_LIGHTS];
	unsigned int pointLightsSize;
};

//struct CB_PS_PCFshader
//{
//	int pcfLevel;
//	double bias;
//	bool enableShadows;
//};

struct CB_PS_lightCull
{
	DirectX::XMFLOAT4 RadiusAndcutOff[NO_LIGHTS];
};

struct CB_PS_pointLightCull
{
	DirectX::XMFLOAT4 RadiusAndcutOff[NO_POINT_LIGHTS];
};

struct CB_PS_screenEffectBuffer
{
	float gamma;
	float bloomBrightness;
	float bloomStrength;
};

struct CB_PS_pbrBuffer
{
	float roughness;
};

struct CB_PS_materialBuffer
{
	DirectX::XMFLOAT3 emissiveColor;
	float bEmissive;
};
