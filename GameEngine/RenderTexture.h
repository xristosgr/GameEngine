#pragma once
#include <d3d11.h>
#include "ErrorLogger.h"
#include <vector>
#include <DirectXMath.h>
#include "Texture.h"

class RenderTexture
{
public:
	RenderTexture();
	bool Initialize(ID3D11Device* device, int width, int height);
	bool InitializeCustom(ID3D11Device* device, int textureWidth, int textureHeight, float* data);
	void SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView);
	void ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float, float, float, float);

	ID3D11ShaderResourceView* GetShaderResourceView();
	ID3D11ShaderResourceView* shaderResourceView;

	bool CubeMapTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<ID3D11Texture2D*>& textureViews);
	bool CubeMapTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<ID3D11Resource*>& textureViews);
	bool CubeMapTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<ID3D11Texture2D*>& textureViews, float width, float height, UINT mip);
	void SetRenderTargets(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView);

	void ClearRenderTargets(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float red, float green, float blue, float alpha);

public:
	Texture pTexts[6];
	ID3D11Texture2D* pTexture;
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11RenderTargetView* m_renderTargetViews[6];
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11Texture2D* m_texture;
	D3D11_VIEWPORT m_viewport;

	bool init = false;
	bool initCubeMap = false;
};

