#pragma once

#include "DX11Includes.h"
#include "Camera.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include <vector>
#include "AdapterReader.h"
#include "Vertex.h"
#include "VertexBuffer.h"
#include "RectShape.h"
#include "CubeShape.h"
#include "InstancedShape.h"
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"
#include "GFXGui.h"
#include <future>
#include "ModelLoader.h"
#include "RenderTexture.h"

class DX11
{
public:
	DX11();
	bool Initialize(HWND hwnd,Camera& camera, int width, int height);
	
private:
	bool InitializeDirectX(HWND hwnd);
	bool InitializeShaders();
	bool initPSShader(PixelShader* pixelShader, Microsoft::WRL::ComPtr<ID3D11Device> device, std::wstring filePath);
	bool initVSShader(VertexShader* vertexShader, Microsoft::WRL::ComPtr<ID3D11Device> device, std::wstring filePath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT* numElements);

public:
	UINT32 windowWidth, windowHeight;


	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	D3D11_VIEWPORT viewport;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState2D;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStateFront;

	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> AdditiveBlendState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> NoBlendState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState_Wrap;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState_Clamp;

	VertexShader vs2D;
	VertexShader testVS;
	VertexShader pbrVS;
	VertexShader volumetricLightVS;
	VertexShader animVS;
	VertexShader depthAnimVS;
	VertexShader depthVS;
	VertexShader horizontalBlurVS;
	VertexShader verticalBlurVS;

	PixelShader testPS;
	PixelShader colorPS;
	PixelShader lightPS;
	PixelShader pbrPS;
	PixelShader transparentPbrPS;
	PixelShader depthPS;
	PixelShader cubeMapPS;
	PixelShader irradianceConvPS;
	PixelShader brdfPS;
	PixelShader prifilterPS;
	PixelShader envProbePS;
	PixelShader horizontalBlurPS;
	PixelShader verticalBlurPS;
	PixelShader bloomLightPS;
	PixelShader volumetricLightPS;
	PixelShader postProccessPS;
	PixelShader volumeGPassPS;

	VertexBuffer<Vertex> vertexBuffer;


	//CONSTANT BUFFERS
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
	ConstantBuffer<CB_VS_lightsShader> cb_vs_lightsShader;
	ConstantBuffer<CB_VS_windowParams> cb_vs_windowParams;

	ConstantBuffer<CB_PS_lightsShader> cb_ps_lightsShader;
	ConstantBuffer<CB_PS_pointLightsShader> cb_ps_pointLightsShader;
	ConstantBuffer<CB_PS_PCFshader> cb_ps_PCFshader;
	ConstantBuffer<CB_PS_lightCull> cb_ps_lightCull;
	ConstantBuffer<CB_PS_pointLightCull> cb_ps_pointLightCull;
	ConstantBuffer<CB_PS_screenEffectBuffer> cb_ps_screenEffectBuffer;
	ConstantBuffer<CB_PS_pbrBuffer> cb_ps_pbrBuffer;
	ConstantBuffer<CB_PS_lightBuffer> cb_ps_lightBuffer;

	RenderTexture renderTexture;
	//RenderTexture shadowTexture;
};

