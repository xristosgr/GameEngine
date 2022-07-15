#pragma once
#include <d3d11.h>
#include "ErrorLogger.h"
#include <vector>
#include <DirectXMath.h>
#include "Texture.h"
#include "Camera.h"
class RenderTexture
{
public:
	RenderTexture();
	bool Initialize(ID3D11Device* device, int width, int height, DXGI_FORMAT format, bool bMapData = false, D3D11_SUBRESOURCE_DATA* data = nullptr);
	bool InitializeShadow(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, DXGI_FORMAT format, bool bMapData = false, D3D11_SUBRESOURCE_DATA* data = nullptr);
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, DXGI_FORMAT format, bool bMapData = false, D3D11_SUBRESOURCE_DATA* data = nullptr);
	bool InitializeCustom(ID3D11Device* device, int textureWidth, int textureHeight, DXGI_FORMAT format, float* data);
	void SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView);
	void ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float, float, float, float, bool bClearStencil = false);

	ID3D11ShaderResourceView* GetShaderResourceView();
	ID3D11ShaderResourceView* shaderResourceView;


	bool CreateCubeMap(ID3D11Device* device, ID3D11DeviceContext* deviceContext, unsigned int& width, unsigned int& height, DXGI_FORMAT format, unsigned int& maxMipLevels);
	void CreateCubeMapMipLevels(ID3D11Device* device, ID3D11DeviceContext* deviceContext, unsigned int& width, unsigned int& height, unsigned int& mipLevel);
	bool RenderCubeMapFaces(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int& index, ID3D11DepthStencilView* depthView, float* rgba, bool copyDepth, bool bSave = false);
	

	bool CubeMapTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, DXGI_FORMAT format, std::vector<ID3D11Texture2D*>& textureViews);
	bool CubeMapTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<ID3D11Resource*>& textureViews);
	bool CubeMapTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<ID3D11Texture2D*>& textureViews, float width, float height, DXGI_FORMAT format, UINT mip);
	void SetRenderTargets(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, int count);

	void ClearRenderTargets(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView,int count, float* rgb);

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

private:
	D3D11_TEXTURE2D_DESC texElementDesc;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetDesc;

};

