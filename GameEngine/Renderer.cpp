#include "Renderer.h"
#include <fstream>
#include <stdlib.h>
#include <ScreenGrab.h>
#include <DirectXHelpers.h>
#include <wincodec.h>

Renderer::Renderer()
{
	timer.Start();

	rgb[0] = 0.3f;
	rgb[1] = 0.2f;
	rgb[2] = 0.1f;
	rgb[3] = 1.0f;


	hasTexture = true;
	isAnimated = false;
	isFileOpen = false;
	bAddEntity = false;
	runPhysics = false;
	bAddLight = false;
	bAddPointLight = false;
	bAddCollisionObject = false;
	bClear = false;
	bCreateGrid = false;
	bRenderNavMesh = false;
	bRenderNavMeshBounds = false;
	bConvertCordinates = false;
	bModelsLoaded = false;
	bRenderCollision = false;
	copyLight = false;
	copyPointLight = false;
	vSync = 1;
}

bool Renderer::Initialize(HWND hwnd, Camera& camera, int width, int height, std::vector<Entity>& entities, std::vector<Light>& lights, std::vector<Light>& pointLights)
{
	this->windowWidth = width;
	this->windowHeight = height;

	if (!gfx11.Initialize(hwnd, camera, width, height))
		return false;

	gfxGui.Initialize(hwnd, gfx11.device.Get(), gfx11.deviceContext.Get());




	//float rgb[4];
	
	gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState.Get(), 0);
	gfx11.deviceContext->PSSetSamplers(0, 1, gfx11.samplerState_Wrap.GetAddressOf());
	gfx11.deviceContext->RSSetState(gfx11.rasterizerState.Get());
	gfx11.deviceContext->OMSetBlendState(gfx11.blendState.Get(), NULL, 0xFFFFFFFF);

	gfx11.deviceContext->RSSetViewports(1, &gfx11.viewport);
	gfx11.deviceContext->OMSetRenderTargets(1, gfx11.renderTargetView.GetAddressOf(), gfx11.depthStencilView.Get());
	camera.PerspectiveFov(70.0f, static_cast<float>(gfx11.windowWidth / gfx11.windowHeight) * (16.0f / 9.0f), 0.1f, 10000.0f);
	gfx11.deviceContext->ClearRenderTargetView(gfx11.renderTargetView.Get(), rgb);
	gfx11.deviceContext->ClearDepthStencilView(gfx11.depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	ViewMatrix2D = camera.GetViewMatrix();

	return true;
}

void Renderer::InitScene(std::vector<Entity>& entities, std::vector<Light>& lights, std::vector<Light>& pointLights, Camera& camera)
{
	//INIT CONSTANT BUFFERS/////////////////////////////
	///////////////////////////////////////////////////
	HRESULT hr = gfx11.cb_vs_vertexshader.Initialize(gfx11.device, gfx11.deviceContext);
	hr = gfx11.cb_vs_lightsShader.Initialize(gfx11.device, gfx11.deviceContext);
	hr = gfx11.cb_vs_blurWeights.Initialize(gfx11.device, gfx11.deviceContext);
	hr = gfx11.cb_vs_windowParams.Initialize(gfx11.device, gfx11.deviceContext);

	hr = gfx11.cb_ps_lightsShader.Initialize(gfx11.device, gfx11.deviceContext);
	hr = gfx11.cb_ps_PCFshader.Initialize(gfx11.device, gfx11.deviceContext);
	hr = gfx11.cb_ps_lightCull.Initialize(gfx11.device, gfx11.deviceContext);
	hr = gfx11.cb_ps_screenEffectBuffer.Initialize(gfx11.device, gfx11.deviceContext);
	hr = gfx11.cb_ps_pbrBuffer.Initialize(gfx11.device, gfx11.deviceContext);
	hr = gfx11.cb_ps_lightMatrixBuffer.Initialize(gfx11.device, gfx11.deviceContext);

	hr = gfx11.cb_ps_pointLightCull.Initialize(gfx11.device, gfx11.deviceContext);
	hr = gfx11.cb_ps_pointLightsShader.Initialize(gfx11.device, gfx11.deviceContext);

	gfx11.deviceContext->VSSetConstantBuffers(0, 1, gfx11.cb_vs_vertexshader.GetBuffer().GetAddressOf());
	gfx11.deviceContext->VSSetConstantBuffers(1, 1, gfx11.cb_vs_lightsShader.GetBuffer().GetAddressOf());
	gfx11.deviceContext->VSSetConstantBuffers(2, 1, gfx11.cb_vs_blurWeights.GetBuffer().GetAddressOf());
	gfx11.deviceContext->VSSetConstantBuffers(3, 1, gfx11.cb_vs_windowParams.GetBuffer().GetAddressOf());

	gfx11.deviceContext->PSSetConstantBuffers(0, 1, gfx11.cb_ps_lightsShader.GetBuffer().GetAddressOf());
	gfx11.deviceContext->PSSetConstantBuffers(1, 1, gfx11.cb_ps_PCFshader.GetBuffer().GetAddressOf());
	gfx11.deviceContext->PSSetConstantBuffers(2, 1, gfx11.cb_ps_lightCull.GetBuffer().GetAddressOf());
	gfx11.deviceContext->PSSetConstantBuffers(3, 1, gfx11.cb_ps_screenEffectBuffer.GetBuffer().GetAddressOf());
	gfx11.deviceContext->PSSetConstantBuffers(4, 1, gfx11.cb_ps_pbrBuffer.GetBuffer().GetAddressOf());
	gfx11.deviceContext->PSSetConstantBuffers(5, 1, gfx11.cb_ps_lightMatrixBuffer.GetBuffer().GetAddressOf());
	gfx11.deviceContext->PSSetConstantBuffers(6, 1, gfx11.cb_ps_pointLightsShader.GetBuffer().GetAddressOf());
	gfx11.deviceContext->PSSetConstantBuffers(7, 1, gfx11.cb_ps_pointLightCull.GetBuffer().GetAddressOf());
	//////////////////////////////////////////////////////
	/////////////////////////////////////////////////////
	
	gfx11.renderTexture.Initialize(gfx11.device.Get(), gfx11.windowWidth, gfx11.windowHeight);

	for (int i = 0; i < entities.size(); ++i)
	{
		if (!entities[i].isDeleted)
		{
			if (i == 0)
			{
				entities[i].model.loadAsync = false;
			}
			else
			{
				entities[i].model.loadAsync = true;
			}
			entities[i].Intitialize(entities[i].filePath, gfx11.device.Get(), gfx11.deviceContext.Get(), gfx11.cb_vs_vertexshader, entities[i].isAnimated);
		}

	}

	rect.Initialize(gfx11.device.Get(),gfx11.windowWidth,gfx11.windowHeight);
	rectBloom.Initialize(gfx11.device.Get(), gfx11.windowWidth, gfx11.windowHeight);
	rectSmall.Initialize(gfx11.device.Get(), windowWidth, gfx11.windowHeight);
	debugCube.Initialize(gfx11.device.Get());


	for (int i = 0; i < lights.size(); ++i)
	{
		lights[i].Initialize(gfx11.device.Get(), gfx11.deviceContext.Get(), gfx11.cb_vs_vertexshader);
		lights[i].m_shadowMap.Initialize(gfx11.device.Get(), 1024, 1024);
		lights[i].SetupCamera(gfx11.windowWidth, gfx11.windowHeight);
	}
	for (int i = 0; i < pointLights.size(); ++i)
	{
		pointLights[i].lightType = 0;
		pointLights[i].Initialize(gfx11.device.Get(), gfx11.deviceContext.Get(), gfx11.cb_vs_vertexshader);
	}

	environmentProbe.Initialize(gfx11.device.Get(), gfx11.deviceContext.Get(), gfx11.cb_vs_vertexshader, 512, 512);
	environmentProbe.UpdateCamera();

	//PBR
	brdfTexture.Initialize(gfx11.device.Get(), 512, 512);
	for (int i = 0; i < 6; ++i)
	{
		IrradianceConvCubeTextures[i].Initialize(gfx11.device.Get(), 32, 32);
	}




	std::vector<ID3D11Resource*> _tempTexts;
	envTextures[0].CreateTextureWIC(gfx11.device.Get(), "probeMaps1.jpg");
	envTextures[1].CreateTextureWIC(gfx11.device.Get(), "probeMaps0.jpg");
	envTextures[2].CreateTextureWIC(gfx11.device.Get(), "probeMaps2.jpg");
	envTextures[3].CreateTextureWIC(gfx11.device.Get(), "probeMaps3.jpg");
	envTextures[4].CreateTextureWIC(gfx11.device.Get(), "probeMaps5.jpg");
	envTextures[5].CreateTextureWIC(gfx11.device.Get(), "probeMaps4.jpg");
	for (int i = 0; i < 6; ++i)
	{
		_tempTexts.push_back(envTextures[i].texture.Get());
		envTextures[i].texture.Get()->Release();
		//envTextures[i].textureView.Get()->Release();
	}
	environmentProbe.environmentCubeMap.CubeMapTexture(gfx11.device.Get(), gfx11.deviceContext.Get(), _tempTexts);
	
	_tempTexts.clear();
	irradianceTextures[0].CreateTextureWIC(gfx11.device.Get(), "IrradianceConv1.jpg");
	irradianceTextures[1].CreateTextureWIC(gfx11.device.Get(), "IrradianceConv0.jpg");
	irradianceTextures[2].CreateTextureWIC(gfx11.device.Get(), "IrradianceConv2.jpg");
	irradianceTextures[3].CreateTextureWIC(gfx11.device.Get(), "IrradianceConv3.jpg");
	irradianceTextures[4].CreateTextureWIC(gfx11.device.Get(), "IrradianceConv5.jpg");
	irradianceTextures[5].CreateTextureWIC(gfx11.device.Get(), "IrradianceConv4.jpg");
	for (int i = 0; i < 6; ++i)
	{
		_tempTexts.push_back(irradianceTextures[i].texture.Get());
		irradianceTextures[i].texture.Get()->Release();
		//irradianceTextures[i].textureView.Get()->Release();
	}
	irradianceCubeMap.CubeMapTexture(gfx11.device.Get(), gfx11.deviceContext.Get(), _tempTexts);
	_tempTexts.clear();

	brdfText.CreateTextureWIC(gfx11.device.Get(), "BrdfTexture.jpg");



	//Bloom
	BloomHorizontalBlurTexture.Initialize(gfx11.device.Get(), 640, 360);
	BloomVerticalBlurTexture.Initialize(gfx11.device.Get(), 640, 360);
	bloomRenderTexture.Initialize(gfx11.device.Get(), windowWidth, windowHeight);

	//Volumetric light
	volumetricLightTexture.Initialize(gfx11.device.Get(), 640, 360);
	cameraDepthTexture.Initialize(gfx11.device.Get(), 640, 360);
}

//****************RENDER ENTITIES***************************************
void Renderer::RenderEntitiesAndLights(std::vector<Entity>& entities, std::vector<Light>& lights, std::vector<Light>& pointLights, Camera& camera)
{
	gfx11.deviceContext->VSSetConstantBuffers(0, 1, gfx11.cb_vs_vertexshader.GetBuffer().GetAddressOf());
	gfx11.deviceContext->RSSetState(gfx11.rasterizerState.Get());

	std::vector< ID3D11ShaderResourceView*> ShadowTextures;
	ShadowTextures.resize(lights.size());
	int index = 0;
	for (int j = 0; j < ShadowTextures.size(); ++j)
	{

		ShadowTextures[index] = lights[j].m_shadowMap.shaderResourceView;
		index++;



	}
	gfx11.deviceContext->PSSetShaderResources(3, 1, &environmentProbe.environmentCubeMap.shaderResourceView);
	if (brdfText.GetTextureResourceView())
		gfx11.deviceContext->PSSetShaderResources(4, 1, brdfText.GetTextureResourceViewAddress());
	else
		gfx11.deviceContext->PSSetShaderResources(4, 1, &brdfTexture.shaderResourceView);
	gfx11.deviceContext->PSSetShaderResources(5, 1, &irradianceCubeMap.shaderResourceView);
	gfx11.deviceContext->PSSetShaderResources(6, ShadowTextures.size(), ShadowTextures.data());
	
	gfx11.deviceContext->PSSetShader(gfx11.pbrPS.GetShader(), nullptr, 0);

	gfx11.deviceContext->OMSetBlendState(gfx11.blendState.Get(), NULL, 0xFFFFFFFF);
	for (int i = 0; i < entities.size(); ++i)
	{
		if (!entities[i].model.isTransparent)
		{
			if (entities[i].model.isAnimated)
			{
				gfx11.deviceContext->IASetInputLayout(gfx11.animVS.GetInputLayout());
				gfx11.deviceContext->VSSetShader(gfx11.animVS.GetShader(), nullptr, 0);
			}
			else
			{
				gfx11.deviceContext->IASetInputLayout(gfx11.pbrVS.GetInputLayout());
				gfx11.deviceContext->VSSetShader(gfx11.pbrVS.GetShader(), nullptr, 0);
			}

			entities[i].Draw(camera, camera.GetViewMatrix(), camera.GetProjectionMatrix());
		}
		
	}

	gfx11.deviceContext->OMSetBlendState(gfx11.AdditiveBlendState.Get(), NULL, 0xFFFFFFFF);
	gfx11.deviceContext->PSSetShader(gfx11.transparentPbrPS.GetShader(), nullptr, 0);
	for (int i = 0; i < entities.size(); ++i)
	{
		if (entities[i].model.isTransparent)
		{
			if (entities[i].model.isAnimated)
			{
				gfx11.deviceContext->IASetInputLayout(gfx11.animVS.GetInputLayout());
				gfx11.deviceContext->VSSetShader(gfx11.animVS.GetShader(), nullptr, 0);
			}
			else
			{
				gfx11.deviceContext->IASetInputLayout(gfx11.pbrVS.GetInputLayout());
				gfx11.deviceContext->VSSetShader(gfx11.pbrVS.GetShader(), nullptr, 0);
			}

			entities[i].Draw(camera, camera.GetViewMatrix(), camera.GetProjectionMatrix());
		}
	}


	for (int i = 0; i < lights.size(); ++i)
	{

		gfx11.deviceContext->OMSetBlendState(gfx11.blendState.Get(), NULL, 0xFFFFFFFF);
		gfx11.deviceContext->IASetInputLayout(gfx11.testVS.GetInputLayout());
		gfx11.deviceContext->VSSetShader(gfx11.testVS.GetShader(), nullptr, 0);
		gfx11.deviceContext->PSSetShader(gfx11.lightPS.GetShader(), nullptr, 0);

		lights[i].Draw(camera);
	}

	for (int i = 0; i < pointLights.size(); ++i)
	{

		gfx11.deviceContext->OMSetBlendState(gfx11.blendState.Get(), NULL, 0xFFFFFFFF);
		gfx11.deviceContext->IASetInputLayout(gfx11.testVS.GetInputLayout());
		gfx11.deviceContext->VSSetShader(gfx11.testVS.GetShader(), nullptr, 0);
		gfx11.deviceContext->PSSetShader(gfx11.lightPS.GetShader(), nullptr, 0);

		pointLights[i].Draw(camera);
	}

	gfx11.deviceContext->OMSetBlendState(gfx11.blendState.Get(), NULL, 0xFFFFFFFF);
	gfx11.deviceContext->PSSetShader(gfx11.testPS.GetShader(), nullptr, 0);
	//cube.Draw(gfx11.deviceContext.Get(), camera, gfx11.cb_vs_vertexshader);
}
//*************************************************************************


void Renderer::RenderSceneToTexture(RenderTexture& texture, Camera& camera, std::vector<Entity>& entities, std::vector<Light>& lights, std::vector<Light>& pointLights,  std::vector< CollisionObject>& collisionObjects)
{
	UpdateBuffers(lights,pointLights, camera);

	//float rgb[4] = { 0,0,0,1 };
	texture.SetRenderTarget(gfx11.deviceContext.Get(), texture.m_depthStencilView);
	texture.ClearRenderTarget(gfx11.deviceContext.Get(), texture.m_depthStencilView, rgb[0], rgb[1], rgb[2], rgb[3]);

	if (bRenderCollision)
	{
		gfx11.deviceContext->IASetInputLayout(gfx11.testVS.GetInputLayout());
		gfx11.deviceContext->VSSetShader(gfx11.testVS.GetShader(), nullptr, 0);
		gfx11.deviceContext->PSSetShader(gfx11.colorPS.GetShader(), nullptr, 0);
		gfx11.deviceContext->RSSetState(gfx11.rasterizerStateFront.Get());
		for (int i = 0; i < collisionObjects.size(); ++i)
		{
			collisionObjects[i].Draw(gfx11.deviceContext.Get(), camera, gfx11.cb_vs_vertexshader);
		}
	}

	gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState.Get(), 0);
	gfx11.deviceContext->PSSetSamplers(0, 1, gfx11.samplerState_Wrap.GetAddressOf());
	gfx11.deviceContext->RSSetState(gfx11.rasterizerState.Get());
	gfx11.deviceContext->OMSetBlendState(gfx11.blendState.Get(), NULL, 0xFFFFFFFF);

	RenderEntitiesAndLights(entities, lights,pointLights, camera);

	
}

void Renderer::UpdateBuffers(std::vector<Light>& lights, std::vector<Light>& pointLights, Camera& camera)
{
	XMFLOAT4 blurWeight1234 = XMFLOAT4(30.0f, 22.5f, 15.0f, 7.5f);
	XMFLOAT4 blurWeight5678 = XMFLOAT4(0.0f, 7.5f, 15.0f, 22.5f);
	float blurWeight9 = 30.0f;

	gfx11.cb_vs_blurWeights.data.weight1234 = blurWeight1234;
	gfx11.cb_vs_blurWeights.data.weight5678 = blurWeight5678;
	gfx11.cb_vs_blurWeights.data.weight9 = blurWeight9;
	gfx11.cb_vs_blurWeights.data.padding1 = XMFLOAT3(0.0f, 0.0f, 0.0f);

	gfx11.cb_vs_windowParams.data.window_width = windowWidth;
	gfx11.cb_vs_windowParams.data.window_height = windowHeight;

	for (int i = 0; i < lights.size(); ++i)
	{
		gfx11.cb_vs_lightsShader.data.lightProjectionMatrix[i] = lights[i].camera.GetProjectionMatrix();
		gfx11.cb_vs_lightsShader.data.lightViewMatrix[i] = lights[i].camera.GetViewMatrix();

		gfx11.cb_ps_lightsShader.data.dynamicLightColor[i] = DirectX::XMFLOAT4(lights[i].lightColor.x, lights[i].lightColor.y, lights[i].lightColor.z, 1.0f);
		gfx11.cb_ps_lightsShader.data.dynamicLightPosition[i] = DirectX::XMFLOAT4(lights[i].pos.x, lights[i].pos.y, lights[i].pos.z, 1.0f);
		gfx11.cb_ps_lightsShader.data.SpotlightDir[i] = DirectX::XMFLOAT4(lights[i].SpotDir.x, lights[i].SpotDir.y, lights[i].SpotDir.z, 1.0f);

		gfx11.cb_ps_lightsShader.data.lightType[i].x = lights[i].lightType;
		gfx11.cb_ps_lightsShader.data.lightType[i].y = 0;
		gfx11.cb_ps_lightsShader.data.lightType[i].z = 0;
		gfx11.cb_ps_lightsShader.data.lightType[i].w = 0;


		gfx11.cb_ps_lightCull.data.Radius[i].x = lights[i].radius;
		gfx11.cb_ps_lightCull.data.Radius[i].y = lights[i].radius;
		gfx11.cb_ps_lightCull.data.Radius[i].z = lights[i].radius;
		gfx11.cb_ps_lightCull.data.Radius[i].w = lights[i].radius;

		gfx11.cb_ps_lightCull.data.cutOff[i].x = lights[i].cutOff;
		gfx11.cb_ps_lightCull.data.cutOff[i].y = lights[i].cutOff;
		gfx11.cb_ps_lightCull.data.cutOff[i].z = lights[i].cutOff;
		gfx11.cb_ps_lightCull.data.cutOff[i].w = lights[i].cutOff;
	}

	for (int i = 0; i < pointLights.size(); ++i)
	{
		gfx11.cb_ps_pointLightsShader.data.dynamicLightColor[i] = DirectX::XMFLOAT4(pointLights[i].lightColor.x, pointLights[i].lightColor.y, pointLights[i].lightColor.z, 1.0f);
		gfx11.cb_ps_pointLightsShader.data.dynamicLightPosition[i] = DirectX::XMFLOAT4(pointLights[i].pos.x, pointLights[i].pos.y, pointLights[i].pos.z, 1.0f);

		gfx11.cb_ps_pointLightCull.data.Radius[i].x = pointLights[i].radius;
		gfx11.cb_ps_pointLightCull.data.Radius[i].y = pointLights[i].radius;
		gfx11.cb_ps_pointLightCull.data.Radius[i].z = pointLights[i].radius;
		gfx11.cb_ps_pointLightCull.data.Radius[i].w = pointLights[i].radius;

		gfx11.cb_ps_pointLightCull.data.cutOff[i].x = pointLights[i].cutOff;
		gfx11.cb_ps_pointLightCull.data.cutOff[i].y = pointLights[i].cutOff;
		gfx11.cb_ps_pointLightCull.data.cutOff[i].z = pointLights[i].cutOff;
		gfx11.cb_ps_pointLightCull.data.cutOff[i].w = pointLights[i].cutOff;
	}
	
	gfx11.cb_ps_lightsShader.data.cameraPos.x = camera.pos.x;
	gfx11.cb_ps_lightsShader.data.cameraPos.y = camera.pos.y;
	gfx11.cb_ps_lightsShader.data.cameraPos.z = camera.pos.z;
	gfx11.cb_ps_lightsShader.data.cameraPos.w = 1.0f;

	gfx11.cb_ps_PCFshader.data.bias = 0.00008f;
	gfx11.cb_ps_PCFshader.data.enableShadows = true;
	gfx11.cb_ps_PCFshader.data.pcfLevel = 2;



	gfx11.cb_ps_screenEffectBuffer.data.gamma = gamma;
	gfx11.cb_ps_screenEffectBuffer.data.bloomBrightness = bloomBrightness;
	gfx11.cb_ps_screenEffectBuffer.data.bloomStrength = bloomStrengh;

	gfx11.cb_ps_lightMatrixBuffer.data.viewProjMatrix = XMMatrixTranspose(camera.GetViewMatrix());

	gfx11.cb_vs_vertexshader.UpdateBuffer();
	gfx11.cb_vs_lightsShader.UpdateBuffer();
	gfx11.cb_vs_blurWeights.UpdateBuffer();
	gfx11.cb_vs_windowParams.UpdateBuffer();
	gfx11.cb_ps_lightsShader.UpdateBuffer();
	gfx11.cb_ps_PCFshader.UpdateBuffer();
	gfx11.cb_ps_lightCull.UpdateBuffer();
	gfx11.cb_ps_screenEffectBuffer.UpdateBuffer();
	gfx11.cb_ps_pbrBuffer.UpdateBuffer();
	gfx11.cb_ps_lightMatrixBuffer.UpdateBuffer();
	gfx11.cb_ps_pointLightsShader.UpdateBuffer();
	gfx11.cb_ps_pointLightCull.UpdateBuffer();
}

//********************************PBR*********************************

//**********************************************************************************


void Renderer::Render(Camera& camera, std::vector<Entity>& entities, PhysicsHandler& physicsHandler, std::vector<Light>& lights, std::vector<Light>& pointLights, std::vector< CollisionObject>& collisionObjects, GridClass& grid, std::vector<NavMeshClass>& navMeshes, physx::PxScene& scene)
{
	//for(int i=0; i<lights.size();++i)
	//	lights[i].UpdateCamera();

	//float rgb[4];
	//rgb[0] = 0.5f;
	//rgb[1] = 0.5f;
	//rgb[2] = 0.5f;
	//rgb[3] = 1.0f;

	//SHADOWS////
	HRESULT hr;

	int depthBias = 40;
	double slopeBias = 1;
	float clamp = 1.0f;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> shadowRasterizerState;
	CD3D11_RASTERIZER_DESC shadowRasterizerDesc(D3D11_DEFAULT);
	
	shadowRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	shadowRasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	shadowRasterizerDesc.DepthBias = depthBias;
	shadowRasterizerDesc.DepthBiasClamp = clamp;
	shadowRasterizerDesc.SlopeScaledDepthBias = slopeBias;
	hr = gfx11.device->CreateRasterizerState(&shadowRasterizerDesc, shadowRasterizerState.GetAddressOf());
	COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");
	
	gfx11.deviceContext->RSSetState(shadowRasterizerState.Get());

	for (int i = 0; i < lights.size(); ++i)
	{
		lights[i].UpdateCamera();

		if(lights[i].bShadow)
			shadowsRenderer.RenderShadows(gfx11, entities, lights[i], camera, i);
	}

	////////////
	environmentProbe.UpdateCamera();
	if (environmentProbe.recalculate)
	{
		environmentProbe.environmentCubeMap.m_renderTargetTexture->Release();
		for (int i = 0; i < 6; ++i)
		{
			//float rgb[4] = { 0.0f,0.0f,0.0f,1.0f };
			gfx11.deviceContext->RSSetViewports(1, &environmentProbe.probeMaps[i].m_viewport);
			gfx11.deviceContext->OMSetRenderTargets(1, gfx11.renderTargetView.GetAddressOf(), gfx11.depthStencilView.Get());
			gfx11.deviceContext->ClearRenderTargetView(gfx11.renderTargetView.Get(), rgb);
			gfx11.deviceContext->ClearDepthStencilView(gfx11.depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
			RenderToEnvProbe(environmentProbe.probeMaps[i], environmentProbe.camera[i], entities, lights, pointLights);
			//RenderSceneToTexture(environmentProbe.probeMaps[i], environmentProbe.camera[i], entities, lights, collisionObjects);
		}
		environmentProbe.cubeTex.push_back(environmentProbe.probeMaps[1].m_renderTargetTexture);
		environmentProbe.cubeTex.push_back(environmentProbe.probeMaps[0].m_renderTargetTexture);
		environmentProbe.cubeTex.push_back(environmentProbe.probeMaps[2].m_renderTargetTexture);
		environmentProbe.cubeTex.push_back(environmentProbe.probeMaps[3].m_renderTargetTexture);
		environmentProbe.cubeTex.push_back(environmentProbe.probeMaps[5].m_renderTargetTexture);
		environmentProbe.cubeTex.push_back(environmentProbe.probeMaps[4].m_renderTargetTexture);
		
		hr = DirectX::SaveWICTextureToFile(gfx11.deviceContext.Get(), environmentProbe.probeMaps[0].m_renderTargetTexture,GUID_ContainerFormatJpeg, L"probeMaps0.JPG",nullptr,nullptr,true);
		hr = DirectX::SaveWICTextureToFile(gfx11.deviceContext.Get(), environmentProbe.probeMaps[1].m_renderTargetTexture,GUID_ContainerFormatJpeg, L"probeMaps1.JPG",nullptr,nullptr,true);
		hr = DirectX::SaveWICTextureToFile(gfx11.deviceContext.Get(), environmentProbe.probeMaps[2].m_renderTargetTexture,GUID_ContainerFormatJpeg, L"probeMaps2.JPG",nullptr,nullptr,true);
		hr = DirectX::SaveWICTextureToFile(gfx11.deviceContext.Get(), environmentProbe.probeMaps[3].m_renderTargetTexture,GUID_ContainerFormatJpeg, L"probeMaps3.JPG",nullptr,nullptr,true);
		hr = DirectX::SaveWICTextureToFile(gfx11.deviceContext.Get(), environmentProbe.probeMaps[4].m_renderTargetTexture,GUID_ContainerFormatJpeg, L"probeMaps4.JPG",nullptr,nullptr,true);
		hr = DirectX::SaveWICTextureToFile(gfx11.deviceContext.Get(), environmentProbe.probeMaps[5].m_renderTargetTexture,GUID_ContainerFormatJpeg, L"probeMaps5.JPG",nullptr,nullptr,true);

			//COM_ERROR_IF_FAILED(hr, "Failed to save texture.");
			//ErrorLogger::Log(hr, "Failed to save texture.");
		
		std::vector<ID3D11Resource*> _tempTexts;
		envTextures[0].CreateTextureWIC(gfx11.device.Get(), "probeMaps1.jpg");
		envTextures[1].CreateTextureWIC(gfx11.device.Get(), "probeMaps0.jpg");
		envTextures[2].CreateTextureWIC(gfx11.device.Get(), "probeMaps2.jpg");
		envTextures[3].CreateTextureWIC(gfx11.device.Get(), "probeMaps3.jpg");
		envTextures[4].CreateTextureWIC(gfx11.device.Get(), "probeMaps5.jpg");
		envTextures[5].CreateTextureWIC(gfx11.device.Get(), "probeMaps4.jpg");
		for (int i = 0; i < 6; ++i)
		{
			_tempTexts.push_back(envTextures[i].texture.Get());
			envTextures[i].texture.Get()->Release();
			//envTextures[i].GetTextureResourceView()->Release();
			//envTextures[i].textureView.Get()->Release();
		}
		environmentProbe.environmentCubeMap.CubeMapTexture(gfx11.device.Get(), gfx11.deviceContext.Get(), _tempTexts);
		for (int i = 0; i < 6; ++i)
		{
			_tempTexts[i]->Release();
			envTextures[i].GetTextureResourceView()->Release();
		}
	
		_tempTexts.clear();
		//environmentProbe.environmentCubeMap.CubeMapTexture(gfx11.device.Get(), gfx11.deviceContext.Get(), environmentProbe.cubeTex);
		environmentProbe.cubeTex.clear();

		PbrRender(camera);

		environmentProbe.recalculate = false;
	}

	gfx11.deviceContext->RSSetViewports(1, &gfx11.viewport);
	gfx11.deviceContext->OMSetRenderTargets(1, gfx11.renderTargetView.GetAddressOf(), gfx11.depthStencilView.Get());
	gfx11.deviceContext->ClearRenderTargetView(gfx11.renderTargetView.Get(), rgb);
	gfx11.deviceContext->ClearDepthStencilView(gfx11.depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	RenderSceneToTexture(gfx11.renderTexture, camera, entities, lights,pointLights, collisionObjects);

	//////////////BLOOM///////////////////////////////////////
	if (enablePostProccess)
	{
		gfx11.deviceContext->RSSetViewports(1, &gfx11.viewport);
		gfx11.deviceContext->OMSetRenderTargets(1, gfx11.renderTargetView.GetAddressOf(), gfx11.depthStencilView.Get());
		gfx11.deviceContext->ClearRenderTargetView(gfx11.renderTargetView.Get(), rgb);
		gfx11.deviceContext->ClearDepthStencilView(gfx11.depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		BloomRender(camera);

		gfx11.deviceContext->RSSetViewports(1, &gfx11.viewport);
		gfx11.deviceContext->OMSetRenderTargets(1, gfx11.renderTargetView.GetAddressOf(), gfx11.depthStencilView.Get());
		gfx11.deviceContext->ClearRenderTargetView(gfx11.renderTargetView.Get(), rgb);
		gfx11.deviceContext->ClearDepthStencilView(gfx11.depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		//VolumeLightRender(entities, lights,pointLights, camera);
	}

	////////////////////////////////
	// 
	//////////////////////////////////////////////////////
	gfx11.deviceContext->RSSetViewports(1, &gfx11.viewport);
	gfx11.deviceContext->OMSetRenderTargets(1, gfx11.renderTargetView.GetAddressOf(), gfx11.depthStencilView.Get());
	gfx11.deviceContext->ClearRenderTargetView(gfx11.renderTargetView.Get(), rgb);
	gfx11.deviceContext->ClearDepthStencilView(gfx11.depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	gfx11.deviceContext->PSSetShader(gfx11.postProccessPS.GetShader(), nullptr, 0);
	rect.SetRenderTexture(gfx11.deviceContext.Get(), gfx11.renderTexture);
	gfx11.deviceContext->PSSetShaderResources(1, 1, &BloomHorizontalBlurTexture.shaderResourceView);
	gfx11.deviceContext->PSSetShaderResources(2, 1, &volumetricLightTexture.shaderResourceView);
	gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState2D.Get(), 0);
	gfx11.deviceContext->IASetInputLayout(gfx11.vs2D.GetInputLayout());
	gfx11.deviceContext->VSSetShader(gfx11.vs2D.GetShader(), nullptr, 0);
	rect.Draw(gfx11.deviceContext.Get(), camera, gfx11.cb_vs_vertexshader);
	gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState.Get(), 0);
	//////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////

	if (debugEnabled)
	{
		gfx11.deviceContext->PSSetShader(gfx11.testPS.GetShader(), nullptr, 0);
		rectSmall.pos = XMFLOAT3(2.88, -1.56, 2.878);
		gfx11.deviceContext->PSSetShaderResources(0, 1, &volumetricLightTexture.shaderResourceView);
		gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState2D.Get(), 0);
		gfx11.deviceContext->IASetInputLayout(gfx11.vs2D.GetInputLayout());
		gfx11.deviceContext->VSSetShader(gfx11.vs2D.GetShader(), nullptr, 0);
	
		//rectBloom.rot = XMFLOAT3(0, 0, 7.846);
		//rectBloom.scale = XMFLOAT3(1, -1, 1);
		//rectBloom.Draw(gfx11.deviceContext.Get(), camera, gfx11.cb_vs_vertexshader);
		rectSmall.Draw(gfx11.deviceContext.Get(), camera, gfx11.cb_vs_vertexshader);

		//debugCube.SetRenderTexture(gfx11.deviceContext.Get(), environmentProbe.environmentCubeMap);
		gfx11.deviceContext->PSSetShader(gfx11.cubeMapPS.GetShader(), nullptr, 0);
		gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState.Get(), 0);
		gfx11.deviceContext->IASetInputLayout(gfx11.pbrVS.GetInputLayout());
		gfx11.deviceContext->VSSetShader(gfx11.pbrVS.GetShader(), nullptr, 0);
		debugCube.pos = XMFLOAT3(0, 0, 0);
		//debugCube.Draw(gfx11.deviceContext.Get(), camera, gfx11.cb_vs_vertexshader);
		
		gfx11.deviceContext->PSSetShader(gfx11.colorPS.GetShader(), nullptr, 0);
		//environmentProbe.Draw(camera);
	}
	
	/////////////////////////////////////////////////////////////////////////////////


	//Physics Debug Draw
	//////////////////////
	//////////////////////
	gfx11.deviceContext->IASetInputLayout(gfx11.testVS.GetInputLayout());
	gfx11.deviceContext->VSSetShader(gfx11.testVS.GetShader(), nullptr, 0);
	gfx11.deviceContext->PSSetShader(gfx11.colorPS.GetShader(), nullptr, 0);
	const physx::PxRenderBuffer& rb = physicsHandler.aScene->getRenderBuffer();

	

	for (physx::PxU32 i = 0; i < rb.getNbLines(); ++i)
	{
		const physx::PxDebugLine& line = rb.getLines()[i];
		physicsDebugDraw.DebugDraw(gfx11.device.Get(), gfx11.deviceContext.Get(), &gfx11.cb_vs_vertexshader, line, camera);
	}

	gfx11.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	if (bRenderNavMeshBounds)
	{
		grid.DrawBounds(gfx11.deviceContext.Get(), camera, gfx11.cb_vs_vertexshader);
		grid.DrawGrid(gfx11.deviceContext.Get(), camera, gfx11.cb_vs_vertexshader, grid.nodes);
	}
	if (bRenderNavMesh)
	{
		if (grid.hasFinished)
		{
			for(int i=0;i< navMeshes.size(); ++i)
				grid.DrawGrid(gfx11.deviceContext.Get(), camera, gfx11.cb_vs_vertexshader, navMeshes[i].validNodes);
		}
			
	}
	gfx11.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	/////////////////////
	////////////////////
	
	//GUI
	////////////////////////////////////
	////////////////////////////////////
	gfxGui.BeginRender();


	static bool show_app_metrics = false;
	static bool show_app_console = false;
	static bool show_app_log = false;
	static bool show_app_style_editor = false;
	static bool show_lights = false;
	static bool show_objects = true;
	static bool show_particles = false;
	static bool show_general = false;


	if (show_app_metrics) { ImGui::ShowMetricsWindow(&show_app_metrics); }

	bool open = false, save = false;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Engine"))
		{
			if (ImGui::MenuItem("Save", NULL))
				save = true;
			ImGui::MenuItem("Lights", NULL, &show_lights);
			ImGui::MenuItem("Objects", NULL, &show_objects);
			ImGui::MenuItem("Particles", NULL, &show_particles);
			ImGui::MenuItem("General", NULL, &show_general);


			ImGui::EndMenu();
		}
		ImGui::SameLine();
		if (ImGui::BeginMenu("Files"))
		{
			if (ImGui::MenuItem("Open", NULL))
				open = true;

			ImGui::EndMenu();
		}
		ImGui::SameLine();
		if (ImGui::BeginMenu("Debug"))
		{
			ImGui::MenuItem("EnablePostProccess", NULL, &enablePostProccess);
			ImGui::MenuItem("Console", NULL, &show_app_console);
			ImGui::MenuItem("Log", NULL, &show_app_log);
			//ImGui::Checkbox("showDebugWindow", &showDebugWindow);

			ImGui::EndMenu();
		}
		ImGui::SameLine();

		if (ImGui::BeginMenu("Tools"))
		{
			ImGui::MenuItem("Metrics", NULL, &show_app_metrics);
			ImGui::MenuItem("Style Editor", NULL, &show_app_style_editor);

			ImGui::EndMenu();
		}
		ImGui::SameLine();

		ImGui::NewLine();

		if (open)
			ImGui::OpenPopup("Open File");
		//if (save)
		//	ImGui::OpenPopup("Save File");

		/* Optional third parameter. Support opening only compressed rar/zip files.
		 * Opening any other file will show error, return false and won't close the dialog.
		 */
		if (file_dialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(400, 200), "*.*,.obj,.dae,.gltf,.fbx,.glb"))
		{
			std::cout << file_dialog.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
			std::cout << file_dialog.selected_path << std::endl;    // The absolute path to the selected file
			//f << file_dialog.selected_path;

			std::fstream f;
			f = std::fstream(file_dialog.selected_path.c_str());
			if (f.good())
				inName = file_dialog.selected_path;

			OutputDebugStringA(("NAME = " + inName + "\n").c_str());

			isFileOpen = true;
		}

		
		
		//ImGui::Text(inName.c_str());

		ImGui::EndMainMenuBar();
	}


	//cube.DrawGUI("cube");
	rectBloom.DrawGUI("rectBloom");
	//rectSmall.DrawGUI("rectSmall");
	//debugCube.DrawGUI("debugCube");
	ImGui::Begin("World");

	if (ImGui::IsWindowHovered() || ImGui::IsAnyItemHovered() || ImGui::IsMouseDragging(0) || ImGui::IsMouseClicked(0))
	{
		physicsHandler.isMouseHover = true;
		//OutputDebugStringA("VALID!!!!!\n");
	}
	else
	{
		physicsHandler.isMouseHover = false;
		//OutputDebugStringA("INVALID!!!!!\n");
	}

	if (ImGui::CollapsingHeader("Options"))
	{
		ImGui::Checkbox("runPhysics", &runPhysics);
		ImGui::Checkbox("Possess", &camera.PossessCharacter);
		ImGui::Checkbox("enablePostProccess", &enablePostProccess);
		ImGui::Checkbox("debugEnabled", &debugEnabled);
		ImGui::Checkbox("renderCollision", &bRenderCollision);
		ImGui::DragInt("vSync", &vSync);
		ImGui::DragFloat("bloomStrengh", &bloomStrengh,0.1f);
		ImGui::DragFloat("bloomBrightness", &bloomBrightness, 0.1f);
		ImGui::DragFloat("gamma", &gamma, 0.1f);
		
	}


	if (ImGui::CollapsingHeader("Lights"))
	{
		static int listbox_light_current = 0;
		std::vector<const char*> lightNames;
		for (int i = 0; i < lights.size(); ++i)
		{

			lights[i].name = "light" + std::to_string(i);
			lightNames.push_back(lights[i].name.c_str());
		}
		ImGui::ListBox("Lights", &listbox_light_current, lightNames.data(), lightNames.size());
		for (int i = 0; i < lights.size(); ++i)
		{
			if (lightNames[listbox_light_current] == lights[i].name.c_str())
			{
				selectedLight = i;
				lights[i].DrawGui("light");
			}
		}
		if(ImGui::Button("Add"))
		{
			bAddLight = true;
		}
		if (ImGui::Button("Copy"))
		{
			copyLight = true;
		}
	}

	if (ImGui::CollapsingHeader("pointLights"))
	{
		static int listbox_light_current = 0;
		std::vector<const char*> lightNames;
		for (int i = 0; i < pointLights.size(); ++i)
		{

			pointLights[i].name = "pointLight" + std::to_string(i);
			lightNames.push_back(pointLights[i].name.c_str());
		}
		ImGui::ListBox("pointLights", &listbox_light_current, lightNames.data(), lightNames.size());
		for (int i = 0; i < pointLights.size(); ++i)
		{
			if (lightNames[listbox_light_current] == pointLights[i].name.c_str())
			{
				selectedPointLight = i;
				pointLights[i].DrawGui("pointLight");
			}
		}
		if (ImGui::Button("Add"))
		{
			bAddPointLight = true;
		}
		if (ImGui::Button("Copy"))
		{
			copyPointLight = true;
		}
	}
	
	if (ImGui::CollapsingHeader("Entities"))
	{
		static int listbox_item_current = 0;
		std::vector<const char*> objNames;
		for (int i = 0; i < entities.size(); ++i)
		{
			entities[i].entityName = "Entity" + std::to_string(i);
			objNames.push_back(entities[i].entityName.c_str());
		}
		ImGui::ListBox("Objects", &listbox_item_current, objNames.data(), objNames.size());

		for (int i = 0; i < entities.size(); ++i)
		{
			if (entities[i].isDeleted)
				continue;

			if (entities[i].physicsComponent.aActor)
			{
				physx::PxShape* _shape = nullptr;
				entities[i].physicsComponent.aActor->getShapes(&_shape, entities[i].physicsComponent.aActor->getNbShapes());

				if (_shape)
				{
					if (_shape->getFlags().isSet(physx::PxShapeFlag::eVISUALIZATION))
					{
						listbox_item_current = i;
					}
				}

			}
			else if (entities[i].physicsComponent.aStaticActor)
			{
				physx::PxShape* _shape = nullptr;
				entities[i].physicsComponent.aStaticActor->getShapes(&_shape, entities[i].physicsComponent.aStaticActor->getNbShapes());

				if (_shape->getFlags().isSet(physx::PxShapeFlag::eVISUALIZATION))
				{
					listbox_item_current = i;
				}
			}
			if (objNames[listbox_item_current] == entities[i].entityName)
			{
				//ImGui::BeginChild("Entity",ImVec2(0.5,0.5),true);
				entities[i].DrawGui(scene);
				//ImGui::EndChild();
			}
		}

	}
	if (ImGui::CollapsingHeader("Collision objects"))
	{
		static int listbox_item_current = 0;
		std::vector<const char*> objNames;
		for (int i = 0; i < collisionObjects.size(); ++i)
		{
			collisionObjects[i].entityName = "collisionObject" + std::to_string(i);
			objNames.push_back(collisionObjects[i].entityName.c_str());
		}
		ImGui::ListBox("collisionObjects", &listbox_item_current, objNames.data(), objNames.size());

		for (int i = 0; i < collisionObjects.size(); ++i)
		{
			//if (entities[i].physicsComponent.aActor)
			//{
			//	physx::PxShape* _shape = nullptr;
			//	entities[i].physicsComponent.aActor->getShapes(&_shape, entities[i].physicsComponent.aActor->getNbShapes());
			//
			//	if (_shape)
			//	{
			//		if (_shape->getFlags().isSet(physx::PxShapeFlag::eVISUALIZATION))
			//		{
			//			listbox_item_current = i;
			//		}
			//	}
			//
			//}
			//else if (entities[i].physicsComponent.aStaticActor)
			//{
			//	physx::PxShape* _shape = nullptr;
			//	entities[i].physicsComponent.aStaticActor->getShapes(&_shape, entities[i].physicsComponent.aStaticActor->getNbShapes());
			//
			//	if (_shape->getFlags().isSet(physx::PxShapeFlag::eVISUALIZATION))
			//	{
			//		listbox_item_current = i;
			//	}
			//}
			if (objNames[listbox_item_current] == collisionObjects[i].entityName)
			{
				collisionObjects[i].DrawGUI(objNames[listbox_item_current]);
			}
		}
	}
	if (ImGui::CollapsingHeader("AI"))
	{
		grid.DrawGUI();
		ImGui::Checkbox("renderNavMesh", &bRenderNavMesh);
		ImGui::Checkbox("renderNavMeshBounds", &bRenderNavMeshBounds);
		if (ImGui::Button("Create grid"))
		{
			bCreateGrid = true;
		}
	}
	if (ImGui::CollapsingHeader("EnvironmentProbe"))
	{
		environmentProbe.DrawGui("Probe1");
	}

	if (ImGui::Button("AddCollisionObject"))
	{
		bAddCollisionObject = true;
	}
	

	if (ImGui::Button("CLEAR!!!!"))
	{
		bClear = true;
	}
	ImGui::End();


	
	if (isFileOpen)
	{
		ImGui::Begin("Import");
		if (ImGui::IsWindowHovered() || ImGui::IsAnyItemHovered() || ImGui::IsMouseDragging(0))
		{
			physicsHandler.isMouseHover = true;
			//OutputDebugStringA("VALID!!!!!\n");
		}
		else
		{
			physicsHandler.isMouseHover = false;
			//OutputDebugStringA("INVALID!!!!!\n");
		}
		ImGui::Checkbox("Textures", &hasTexture);
		ImGui::Checkbox("Animated", &isAnimated);
		ImGui::Checkbox("ConvertCordinates", &bConvertCordinates);
		if (ImGui::Button("Add"))
		{
			bAddEntity = true;
			isFileOpen = false;
		}
		ImGui::End();
	}

	gfxGui.EndRender();
	/////////////////////////////////////
	/////////////////////////////////////
	gfx11.swapchain->Present(vSync, NULL);
}






//********************************PBR************************************************
//***********************************************************************************
void Renderer::BrdfRender(Camera& camera, RenderTexture& texture)
{
	texture.SetRenderTarget(gfx11.deviceContext.Get(), texture.m_depthStencilView);
	texture.ClearRenderTarget(gfx11.deviceContext.Get(), texture.m_depthStencilView, 0, 0, 0, 1);

	gfx11.deviceContext->PSSetShader(gfx11.brdfPS.GetShader(), nullptr, 0);
	gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState.Get(), 0);
	gfx11.deviceContext->IASetInputLayout(gfx11.vs2D.GetInputLayout());
	gfx11.deviceContext->VSSetShader(gfx11.vs2D.GetShader(), nullptr, 0);
	this->gfx11.deviceContext->RSSetState(this->gfx11.rasterizerState.Get());
	rect.Draw(this->gfx11.deviceContext.Get(), camera, this->gfx11.cb_vs_vertexshader);

	HRESULT hr = DirectX::SaveWICTextureToFile(gfx11.deviceContext.Get(), texture.m_renderTargetTexture, GUID_ContainerFormatJpeg, L"BrdfTexture.JPG", nullptr, nullptr, true);
}
void Renderer::IrradianceConvolutionRender(Camera& camera)
{
	//environmentProbe.UpdateCamera(512, 512);
	debugCube.pos = environmentProbe.pos;
	//environmentProbe.pos = debugCube.pos;

	irradianceCubeMap.m_renderTargetTexture->Release();
	
	for (int i = 0; i < 6; ++i)
	{
		gfx11.deviceContext->RSSetViewports(1, &IrradianceConvCubeTextures[i].m_viewport);
		IrradianceConvCubeTextures[i].SetRenderTarget(gfx11.deviceContext.Get(), IrradianceConvCubeTextures[i].m_depthStencilView);
		IrradianceConvCubeTextures[i].ClearRenderTarget(gfx11.deviceContext.Get(), IrradianceConvCubeTextures[i].m_depthStencilView, 0, 0, 0, 1);

		gfx11.deviceContext->PSSetShader(gfx11.irradianceConvPS.GetShader(), nullptr, 0);
		gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState.Get(), 0);
		gfx11.deviceContext->IASetInputLayout(gfx11.pbrVS.GetInputLayout());
		gfx11.deviceContext->VSSetShader(gfx11.pbrVS.GetShader(), nullptr, 0);
		this->gfx11.deviceContext->RSSetState(this->gfx11.rasterizerState.Get());
		gfx11.deviceContext->PSSetSamplers(0, 1, gfx11.samplerState_Wrap.GetAddressOf());
		gfx11.deviceContext->PSSetSamplers(1, 1, gfx11.samplerState_Clamp.GetAddressOf());

		XMMATRIX viewMatrix = environmentProbe.viewMatrices[i];
		XMMATRIX projectionMatrix = environmentProbe.projectionMatrices[i];

		debugCube.SetRenderTexture(gfx11.deviceContext.Get(), environmentProbe.environmentCubeMap);
		debugCube.Draw(gfx11.deviceContext.Get(), viewMatrix, projectionMatrix, gfx11.cb_vs_vertexshader);

	}
	environmentProbe.cubeTex.push_back(IrradianceConvCubeTextures[1].m_renderTargetTexture);
	environmentProbe.cubeTex.push_back(IrradianceConvCubeTextures[0].m_renderTargetTexture);
	environmentProbe.cubeTex.push_back(IrradianceConvCubeTextures[2].m_renderTargetTexture);
	environmentProbe.cubeTex.push_back(IrradianceConvCubeTextures[3].m_renderTargetTexture);
	environmentProbe.cubeTex.push_back(IrradianceConvCubeTextures[5].m_renderTargetTexture);
	environmentProbe.cubeTex.push_back(IrradianceConvCubeTextures[4].m_renderTargetTexture);

	HRESULT hr = DirectX::SaveWICTextureToFile(gfx11.deviceContext.Get(), IrradianceConvCubeTextures[0].m_renderTargetTexture, GUID_ContainerFormatJpeg, L"IrradianceConv0.JPG",nullptr,nullptr,true);
	hr = DirectX::SaveWICTextureToFile(gfx11.deviceContext.Get(), IrradianceConvCubeTextures[1].m_renderTargetTexture, GUID_ContainerFormatJpeg, L"IrradianceConv1.JPG", nullptr,nullptr,true);
	hr = DirectX::SaveWICTextureToFile(gfx11.deviceContext.Get(), IrradianceConvCubeTextures[2].m_renderTargetTexture, GUID_ContainerFormatJpeg, L"IrradianceConv2.JPG", nullptr,nullptr,true);
	hr = DirectX::SaveWICTextureToFile(gfx11.deviceContext.Get(), IrradianceConvCubeTextures[3].m_renderTargetTexture, GUID_ContainerFormatJpeg, L"IrradianceConv3.JPG", nullptr,nullptr,true);
	hr = DirectX::SaveWICTextureToFile(gfx11.deviceContext.Get(), IrradianceConvCubeTextures[4].m_renderTargetTexture, GUID_ContainerFormatJpeg, L"IrradianceConv4.JPG", nullptr,nullptr,true);
	hr = DirectX::SaveWICTextureToFile(gfx11.deviceContext.Get(), IrradianceConvCubeTextures[5].m_renderTargetTexture, GUID_ContainerFormatJpeg, L"IrradianceConv5.JPG", nullptr,nullptr,true);
	
	std::vector<ID3D11Resource*> _tempTexts;
	irradianceTextures[0].CreateTextureWIC(gfx11.device.Get(), "IrradianceConv1.jpg");
	irradianceTextures[1].CreateTextureWIC(gfx11.device.Get(), "IrradianceConv0.jpg");
	irradianceTextures[2].CreateTextureWIC(gfx11.device.Get(), "IrradianceConv2.jpg");
	irradianceTextures[3].CreateTextureWIC(gfx11.device.Get(), "IrradianceConv3.jpg");
	irradianceTextures[4].CreateTextureWIC(gfx11.device.Get(), "IrradianceConv5.jpg");
	irradianceTextures[5].CreateTextureWIC(gfx11.device.Get(), "IrradianceConv4.jpg");
	for (int i = 0; i < 6; ++i)
	{
		_tempTexts.push_back(irradianceTextures[i].texture.Get());
		irradianceTextures[i].texture.Get()->Release();
		
		//irradianceTextures[i].textureView.Get()->Release();
	}
	irradianceCubeMap.CubeMapTexture(gfx11.device.Get(), gfx11.deviceContext.Get(), _tempTexts);

	for (int i = 0; i < 6; ++i)
	{
		_tempTexts[i]->Release();
		irradianceTextures[i].GetTextureResourceView()->Release();
	}
	_tempTexts.clear();
	//irradianceCubeMap.CubeMapTexture(gfx11.device.Get(), gfx11.deviceContext.Get(), environmentProbe.cubeTex);
	environmentProbe.cubeTex.clear();
}

void Renderer::PbrRender(Camera& camera)
{
	BrdfRender(camera, brdfTexture);
	IrradianceConvolutionRender(camera);
	//PrifilterRender(camera, prefilterCubeMap);
}

void Renderer::RenderToEnvProbe(RenderTexture& texture, Camera& camera, std::vector<Entity>& entities, std::vector<Light>& lights, std::vector<Light>& pointLights)
{
	UpdateBuffers(lights,pointLights, camera);

	//float rgb[4] = { 0.0f,0.0f,0.0f,1.0f };
	texture.SetRenderTarget(gfx11.deviceContext.Get(), texture.m_depthStencilView);
	texture.ClearRenderTarget(gfx11.deviceContext.Get(), texture.m_depthStencilView, rgb[0], rgb[1], rgb[2], rgb[3]);

	gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState.Get(), 0);
	gfx11.deviceContext->PSSetSamplers(0, 1, gfx11.samplerState_Wrap.GetAddressOf());
	gfx11.deviceContext->RSSetState(gfx11.rasterizerState.Get());
	gfx11.deviceContext->OMSetBlendState(gfx11.blendState.Get(), NULL, 0xFFFFFFFF);


	std::vector< ID3D11ShaderResourceView*> ShadowTextures;
	ShadowTextures.resize(lights.size());
	int index = 0;
	for (int j = 0; j < ShadowTextures.size(); ++j)
	{

		ShadowTextures[index] = lights[j].m_shadowMap.shaderResourceView;
		index++;
	}
	gfx11.deviceContext->PSSetShaderResources(6, ShadowTextures.size(), ShadowTextures.data());

	///////////////////////////////////////////////////////
	//////////////////////////////////////////////////////
	gfx11.deviceContext->VSSetConstantBuffers(0, 1, gfx11.cb_vs_vertexshader.GetBuffer().GetAddressOf());
	gfx11.deviceContext->RSSetState(gfx11.rasterizerState.Get());

	gfx11.deviceContext->PSSetShader(gfx11.envProbePS.GetShader(), nullptr, 0);
	gfx11.deviceContext->OMSetBlendState(gfx11.blendState.Get(), NULL, 0xFFFFFFFF);
	for (int i = 0; i < entities.size(); ++i)
	{
		gfx11.deviceContext->IASetInputLayout(gfx11.pbrVS.GetInputLayout());
		gfx11.deviceContext->VSSetShader(gfx11.pbrVS.GetShader(), nullptr, 0);
		entities[i].Draw(camera, camera.GetViewMatrix(), camera.GetProjectionMatrix());
	}

	///////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

}

//********************************PBR************************************************
//***********************************************************************************



//********************************BLOOM************************************************
//***********************************************************************************
void Renderer::BloomRender(Camera& camera)
{
	rectBloom.pos = XMFLOAT3(0, 0, 0);
	gfx11.deviceContext->RSSetViewports(1, &bloomRenderTexture.m_viewport);
	bloomRenderTexture.SetRenderTarget(gfx11.deviceContext.Get(), bloomRenderTexture.m_depthStencilView);
	bloomRenderTexture.ClearRenderTarget(gfx11.deviceContext.Get(), bloomRenderTexture.m_depthStencilView, 0, 0, 0, 1.0f);

	gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState2D.Get(), 0);
	gfx11.deviceContext->VSSetShader(gfx11.vs2D.GetShader(), NULL, 0);
	gfx11.deviceContext->IASetInputLayout(gfx11.vs2D.GetInputLayout());
	gfx11.deviceContext->PSSetShader(gfx11.bloomLightPS.GetShader(), NULL, 0);
	gfx11.deviceContext->PSSetShaderResources(0, 1, &gfx11.renderTexture.shaderResourceView);

	rect.Draw(gfx11.deviceContext.Get(), camera, gfx11.cb_vs_vertexshader);



	gfx11.deviceContext->RSSetViewports(1, &gfx11.viewport);
	gfx11.deviceContext->OMSetRenderTargets(1, gfx11.renderTargetView.GetAddressOf(), gfx11.depthStencilView.Get());
	gfx11.deviceContext->ClearRenderTargetView(gfx11.renderTargetView.Get(), rgb);
	gfx11.deviceContext->ClearDepthStencilView(gfx11.depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//Vertical bloom
	gfx11.deviceContext->RSSetViewports(1, &BloomVerticalBlurTexture.m_viewport);
	BloomVerticalBlurTexture.SetRenderTarget(gfx11.deviceContext.Get(), BloomVerticalBlurTexture.m_depthStencilView);
	BloomVerticalBlurTexture.ClearRenderTarget(gfx11.deviceContext.Get(), BloomVerticalBlurTexture.m_depthStencilView, 0, 0, 0, 1.0f);

	gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState2D.Get(), 0);
	gfx11.deviceContext->VSSetShader(gfx11.verticalBlurVS.GetShader(), NULL, 0);
	gfx11.deviceContext->IASetInputLayout(gfx11.verticalBlurVS.GetInputLayout());
	gfx11.deviceContext->PSSetShader(gfx11.verticalBlurPS.GetShader(), NULL, 0);
	gfx11.deviceContext->PSSetShaderResources(0, 1, &bloomRenderTexture.shaderResourceView);

	rectBloom.Draw(gfx11.deviceContext.Get(), camera, gfx11.cb_vs_vertexshader);


	gfx11.deviceContext->RSSetViewports(1, &gfx11.viewport);
	gfx11.deviceContext->OMSetRenderTargets(1, gfx11.renderTargetView.GetAddressOf(), gfx11.depthStencilView.Get());
	gfx11.deviceContext->ClearRenderTargetView(gfx11.renderTargetView.Get(), rgb);
	gfx11.deviceContext->ClearDepthStencilView(gfx11.depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//Horizontal Bloom
	gfx11.deviceContext->RSSetViewports(1, &BloomHorizontalBlurTexture.m_viewport);
	BloomHorizontalBlurTexture.SetRenderTarget(gfx11.deviceContext.Get(), BloomHorizontalBlurTexture.m_depthStencilView);
	BloomHorizontalBlurTexture.ClearRenderTarget(gfx11.deviceContext.Get(), BloomHorizontalBlurTexture.m_depthStencilView, 0, 0, 0, 1.0f);

	gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState2D.Get(), 0);
	gfx11.deviceContext->VSSetShader(gfx11.horizontalBlurVS.GetShader(), NULL, 0);
	gfx11.deviceContext->IASetInputLayout(gfx11.horizontalBlurVS.GetInputLayout());
	gfx11.deviceContext->PSSetShader(gfx11.horizontalBlurPS.GetShader(), NULL, 0);
	gfx11.deviceContext->PSSetShaderResources(0, 1, &BloomVerticalBlurTexture.shaderResourceView);
	rectBloom.Draw(gfx11.deviceContext.Get(), camera, gfx11.cb_vs_vertexshader);
}

//********************************BLOOM************************************************
//***********************************************************************************


void Renderer::RenderEntitiesSimple(std::vector<Entity>& entities,std::vector<Light>& lights, std::vector<Light>& pointLights, PixelShader& psShader, Camera& camera)
{
	gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState.Get(), 0);
	gfx11.deviceContext->PSSetSamplers(0, 1, gfx11.samplerState_Wrap.GetAddressOf());
	gfx11.deviceContext->RSSetState(gfx11.rasterizerState.Get());
	gfx11.deviceContext->OMSetBlendState(gfx11.blendState.Get(), NULL, 0xFFFFFFFF);


	///////////////////////////////////////////////////////
	//////////////////////////////////////////////////////
	gfx11.deviceContext->VSSetConstantBuffers(0, 1, gfx11.cb_vs_vertexshader.GetBuffer().GetAddressOf());
	gfx11.deviceContext->RSSetState(gfx11.rasterizerState.Get());

	//gfx11.deviceContext->IASetInputLayout(gfx11.depthVS.GetInputLayout());
	//gfx11.deviceContext->VSSetShader(gfx11.depthVS.GetShader(), nullptr, 0);
	//gfx11.deviceContext->PSSetShader(gfx11.depthPS.GetShader(), nullptr, 0);
	gfx11.deviceContext->OMSetBlendState(gfx11.blendState.Get(), NULL, 0xFFFFFFFF);
	gfx11.deviceContext->PSSetShader(psShader.GetShader(), NULL, 0);
	for (int i = 0; i < entities.size(); ++i)
	{
		if (!entities[i].isAnimated)
		{
			gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState.Get(), 0);
			gfx11.deviceContext->VSSetShader(gfx11.depthVS.GetShader(), NULL, 0);
			gfx11.deviceContext->IASetInputLayout(gfx11.depthVS.GetInputLayout());
		}
		else
		{
			gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState.Get(), 0);
			gfx11.deviceContext->VSSetShader(gfx11.depthAnimVS.GetShader(), NULL, 0);
			gfx11.deviceContext->IASetInputLayout(gfx11.depthAnimVS.GetInputLayout());
		}

		entities[i].Draw(camera, camera.GetViewMatrix(), camera.GetProjectionMatrix());
	}
	
}

void Renderer::VolumeLightRender(std::vector<Entity>& entities, std::vector<Light>& pointLights, std::vector<Light>& lights, Camera& camera)
{
	gfx11.deviceContext->RSSetViewports(1, &cameraDepthTexture.m_viewport);
	cameraDepthTexture.SetRenderTarget(gfx11.deviceContext.Get(), cameraDepthTexture.m_depthStencilView);
	cameraDepthTexture.ClearRenderTarget(gfx11.deviceContext.Get(), cameraDepthTexture.m_depthStencilView, 0, 0, 0, 1.0f);
	
	RenderEntitiesSimple(entities,lights,pointLights, gfx11.depthPS, camera);

	gfx11.deviceContext->IASetInputLayout(gfx11.depthVS.GetInputLayout());
	gfx11.deviceContext->VSSetShader(gfx11.depthVS.GetShader(), nullptr, 0);
	gfx11.deviceContext->PSSetShader(gfx11.lightPS.GetShader(), nullptr, 0);
	for (int i = 0; i < lights.size(); ++i)
	{
		lights[i].scale = DirectX::XMFLOAT3(0.05, 0.05, 0.05);
	
		lights[i].Draw(camera);
		lights[i].scale = DirectX::XMFLOAT3(0.01, 0.01, 0.01);
	}

	//gfx11.deviceContext->RSSetViewports(1, &volumeGPassTexture.m_viewport);
	//volumeGPassTexture.SetRenderTarget(gfx11.deviceContext.Get(), volumeGPassTexture.m_depthStencilView);
	//volumeGPassTexture.ClearRenderTarget(gfx11.deviceContext.Get(), volumeGPassTexture.m_depthStencilView, 0, 0, 0, 1.0f);
	//
	//RenderEntitiesSimple(entities, lights, gfx11.volumeGPassPS, camera);
	//
	//gfx11.deviceContext->IASetInputLayout(gfx11.depthVS.GetInputLayout());
	//gfx11.deviceContext->VSSetShader(gfx11.depthVS.GetShader(), nullptr, 0);
	//gfx11.deviceContext->PSSetShader(gfx11.lightPS.GetShader(), nullptr, 0);
	//for (int i = 0; i < lights.size(); ++i)
	//{
	//	lights[i].scale = DirectX::XMFLOAT3(0.05, 0.05, 0.05);
	//
	//	lights[i].Draw(camera);
	//	lights[i].scale = DirectX::XMFLOAT3(0.01, 0.01, 0.01);
	//}

	gfx11.deviceContext->RSSetViewports(1, &gfx11.viewport);
	gfx11.deviceContext->OMSetRenderTargets(1, gfx11.renderTargetView.GetAddressOf(), gfx11.depthStencilView.Get());
	gfx11.deviceContext->ClearRenderTargetView(gfx11.renderTargetView.Get(), rgb);
	gfx11.deviceContext->ClearDepthStencilView(gfx11.depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	
	gfx11.deviceContext->RSSetViewports(1, &volumetricLightTexture.m_viewport);
	volumetricLightTexture.SetRenderTarget(gfx11.deviceContext.Get(), volumetricLightTexture.m_depthStencilView);
	volumetricLightTexture.ClearRenderTarget(gfx11.deviceContext.Get(), volumetricLightTexture.m_depthStencilView, 0, 0, 0, 1.0f);
	
	gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState2D.Get(), 0);
	gfx11.deviceContext->VSSetShader(gfx11.volumetricLightVS.GetShader(), NULL, 0);
	gfx11.deviceContext->IASetInputLayout(gfx11.volumetricLightVS.GetInputLayout());
	gfx11.deviceContext->PSSetShader(gfx11.volumetricLightPS.GetShader(), NULL, 0);
	gfx11.deviceContext->PSSetShaderResources(0, 1, &cameraDepthTexture.shaderResourceView);
	gfx11.deviceContext->PSSetShaderResources(1, 1, &lights[0].m_shadowMap.shaderResourceView);
	
	//lights[0].DrawVolume(camera);
	rect.Draw(gfx11.deviceContext.Get(), camera, gfx11.cb_vs_vertexshader);
}
