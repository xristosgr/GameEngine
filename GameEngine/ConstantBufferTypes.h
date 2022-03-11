#pragma once

#include <DirectXMath.h>

#define NO_LIGHTS 25

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
};

struct CB_VS_blurWeights
{
	DirectX::XMFLOAT4 weight1234;
	DirectX::XMFLOAT4 weight5678;
	float weight9;
	DirectX::XMFLOAT3 padding1;
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
};

struct CB_PS_PCFshader
{
	int pcfLevel;
	double bias;
	bool enableShadows;
};

struct CB_PS_lightCull
{
	DirectX::XMFLOAT4 Radius[NO_LIGHTS];
    DirectX::XMFLOAT4 cutOff[NO_LIGHTS];
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

struct CB_PS_lightMatrixBuffer
{
	DirectX::XMMATRIX viewProjMatrix;
};
