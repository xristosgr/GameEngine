#pragma once

#include"DX11.h"
#include "Light.h"
const int BUFFER_COUNT = 6;

class GBufferClass
{
public:
	GBufferClass();
	void Initialize(DX11& gfx11, int width, int height);
	void GeometryPass(DX11& gfx11, Camera& camera, ID3D11DepthStencilView* depthView, float* rgb);
	void LightPass(DX11& gfx11,RectShape& rect, Camera& camera, std::vector<Light>& spotLights, std::vector<Light>& pointLights,float& acceptedDist);

private:
	void SetRenderTargets(DX11& gfx11, ID3D11DepthStencilView* depthView);
	void ClearRenderTargets(DX11& gfx11, ID3D11DepthStencilView* depthView, float* rgb);

public:
	RenderTexture albedoTexture, normalTexture, metallicRoughnessTexture, worldPositionTexture, depthTexture, ssao_normalTexture;

	ID3D11Texture2D* m_renderTargetTextureArray[BUFFER_COUNT];
	ID3D11RenderTargetView* m_renderTargetViewArray[BUFFER_COUNT];
	ID3D11ShaderResourceView* m_shaderResourceViewArray[BUFFER_COUNT];
};

