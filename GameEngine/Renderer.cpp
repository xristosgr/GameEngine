#include "Renderer.h"
#include <fstream>
#include <stdlib.h>
#include <ScreenGrab.h>
#include <DirectXHelpers.h>
#include <wincodec.h>

Renderer::Renderer()
{
	timer.Start();

	skyColor = DirectX::XMFLOAT3(0.2f, 0.5f, 1.0f);
	//rgb[0] = 0.0f;
	//rgb[1] = 0.0f;
	//rgb[2] = 0.0f;
	//rgb[3] = 1.0f;

	bEntityDeleted = false;
	save = false;
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
	bEnableShadows = true;
	bGuiEnabled = true;
	switchCameraMode = 0;
	vSync = 0;
	gamma = 2.2f;
	renderDistance = 6000.0f;
	renderShadowDistance = 2700.0f;
	shadowDist = 5.5f;
	acceptedDist = 100.0f;
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
	hr = gfx11.cb_vs_windowParams.Initialize(gfx11.device, gfx11.deviceContext);

	hr = gfx11.cb_ps_lightsShader.Initialize(gfx11.device, gfx11.deviceContext);
	hr = gfx11.cb_ps_PCFshader.Initialize(gfx11.device, gfx11.deviceContext);
	hr = gfx11.cb_ps_lightCull.Initialize(gfx11.device, gfx11.deviceContext);
	hr = gfx11.cb_ps_screenEffectBuffer.Initialize(gfx11.device, gfx11.deviceContext);
	hr = gfx11.cb_ps_pbrBuffer.Initialize(gfx11.device, gfx11.deviceContext);
	hr = gfx11.cb_ps_lightBuffer.Initialize(gfx11.device, gfx11.deviceContext);

	hr = gfx11.cb_ps_pointLightCull.Initialize(gfx11.device, gfx11.deviceContext);
	hr = gfx11.cb_ps_pointLightsShader.Initialize(gfx11.device, gfx11.deviceContext);

	gfx11.deviceContext->VSSetConstantBuffers(0, 1, gfx11.cb_vs_vertexshader.GetBuffer().GetAddressOf());
	gfx11.deviceContext->VSSetConstantBuffers(1, 1, gfx11.cb_vs_lightsShader.GetBuffer().GetAddressOf());
	gfx11.deviceContext->VSSetConstantBuffers(2, 1, gfx11.cb_vs_windowParams.GetBuffer().GetAddressOf());

	gfx11.deviceContext->PSSetConstantBuffers(0, 1, gfx11.cb_ps_lightsShader.GetBuffer().GetAddressOf());
	gfx11.deviceContext->PSSetConstantBuffers(1, 1, gfx11.cb_ps_PCFshader.GetBuffer().GetAddressOf());
	gfx11.deviceContext->PSSetConstantBuffers(2, 1, gfx11.cb_ps_lightCull.GetBuffer().GetAddressOf());
	gfx11.deviceContext->PSSetConstantBuffers(3, 1, gfx11.cb_ps_screenEffectBuffer.GetBuffer().GetAddressOf());
	gfx11.deviceContext->PSSetConstantBuffers(4, 1, gfx11.cb_ps_pbrBuffer.GetBuffer().GetAddressOf());
	gfx11.deviceContext->PSSetConstantBuffers(5, 1, gfx11.cb_ps_lightBuffer.GetBuffer().GetAddressOf());
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
		if (!entities[i].isDeleted)
		{
			if (entities[i].entityName == " ")
			{
				entities[i].entityName = "Entity" + std::to_string(i);
			}
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
	brdfTexture.Initialize(gfx11.device.Get(), 2048, 2048);


	//Bloom
	BloomHorizontalBlurTexture.Initialize(gfx11.device.Get(), 800, 600);
	BloomVerticalBlurTexture.Initialize(gfx11.device.Get(), 800, 600);

	bloomRenderTexture.Initialize(gfx11.device.Get(), windowWidth, windowHeight);

	//Volumetric light
	volumetricLightTexture.Initialize(gfx11.device.Get(), 640, 360);
	cameraDepthTexture.Initialize(gfx11.device.Get(), 800, 600);
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
	//gfx11.deviceContext->PSSetShaderResources(3, 1, &environmentProbe.environmentCubeMap.shaderResourceView);
	gfx11.deviceContext->PSSetShaderResources(3, 1, &prefilterCubeMap.shaderResourceView);
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
		if (entities[i].model.loadAsync)
		{
			if (!entities[i].model._asyncLoad._Is_ready())
			{
				entities[i].model._asyncLoad.wait();
				bHasFinishedLoading = false;
			}
			else
			{
				bHasFinishedLoading = true;
			}
		}

		DirectX::XMFLOAT3 diff = DirectX::XMFLOAT3(camera.GetPositionFloat3().x - entities[i].pos.x, camera.GetPositionFloat3().y - entities[i].pos.y, camera.GetPositionFloat3().z - entities[i].pos.z);
		physx::PxVec3 diffVec = physx::PxVec3(diff.x, diff.y, diff.z);
		float dist = diffVec.dot(diffVec);

		if (dist < renderDistance)
		{
			if (!entities[i].model.isTransparent)
			{	
				if (entities[i].isEmissive)
				{
					gfx11.deviceContext->PSSetShader(gfx11.lightPS.GetShader(), nullptr, 0);

					gfx11.cb_ps_lightBuffer.data.color = entities[i].emissiveColor;
					gfx11.cb_ps_lightBuffer.UpdateBuffer();

				}
				else
				{
					gfx11.deviceContext->PSSetShader(gfx11.pbrPS.GetShader(), nullptr, 0);
				}
				
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
				
				if (entities[i].model.isAttached)
				{
					if (entities[i].parent)
					{
						if (!entities[i].parentName.empty() && (entities[i].parent->entityName == entities[i].parentName))
							entities[i].SetupAttachment(entities[i].parent);
					}
				}

				entities[i].Draw(camera, camera.GetViewMatrix(), camera.GetProjectionMatrix());
			}

		}
	}
		

	gfx11.deviceContext->OMSetBlendState(gfx11.AdditiveBlendState.Get(), NULL, 0xFFFFFFFF);
	gfx11.deviceContext->PSSetShader(gfx11.transparentPbrPS.GetShader(), nullptr, 0);

	for (int i = 0; i < lights.size(); ++i)
	{
		gfx11.cb_ps_lightBuffer.data.color = lights[i].emissionColor;
		gfx11.cb_ps_lightBuffer.UpdateBuffer();

		DirectX::XMFLOAT3 diff = DirectX::XMFLOAT3(camera.GetPositionFloat3().x - lights[i].pos.x, camera.GetPositionFloat3().y - lights[i].pos.y, camera.GetPositionFloat3().z - lights[i].pos.z);
		physx::PxVec3 diffVec = physx::PxVec3(diff.x, diff.y, diff.z);
		float dist = diffVec.dot(diffVec);

		if (dist < renderDistance)
		{
			gfx11.deviceContext->OMSetBlendState(gfx11.blendState.Get(), NULL, 0xFFFFFFFF);
			gfx11.deviceContext->IASetInputLayout(gfx11.testVS.GetInputLayout());
			gfx11.deviceContext->VSSetShader(gfx11.testVS.GetShader(), nullptr, 0);
			gfx11.deviceContext->PSSetShader(gfx11.lightPS.GetShader(), nullptr, 0);

			lights[i].Draw(camera);
		}
	}

	for (int i = 0; i < pointLights.size(); ++i)
	{

		gfx11.cb_ps_lightBuffer.data.color = pointLights[i].emissionColor;
		gfx11.cb_ps_lightBuffer.UpdateBuffer();

		DirectX::XMFLOAT3 diff = DirectX::XMFLOAT3(camera.GetPositionFloat3().x - pointLights[i].pos.x, camera.GetPositionFloat3().y - pointLights[i].pos.y, camera.GetPositionFloat3().z - pointLights[i].pos.z);
		physx::PxVec3 diffVec = physx::PxVec3(diff.x, diff.y, diff.z);
		float dist = diffVec.dot(diffVec);

		if (dist < renderDistance)
		{
			gfx11.deviceContext->OMSetBlendState(gfx11.blendState.Get(), NULL, 0xFFFFFFFF);
			gfx11.deviceContext->IASetInputLayout(gfx11.testVS.GetInputLayout());
			gfx11.deviceContext->VSSetShader(gfx11.testVS.GetShader(), nullptr, 0);
			gfx11.deviceContext->PSSetShader(gfx11.lightPS.GetShader(), nullptr, 0);

			pointLights[i].Draw(camera);
		}
	}

	gfx11.deviceContext->OMSetBlendState(gfx11.blendState.Get(), NULL, 0xFFFFFFFF);
	gfx11.deviceContext->PSSetShader(gfx11.testPS.GetShader(), nullptr, 0);
	//cube.Draw(gfx11.deviceContext.Get(), camera, gfx11.cb_vs_vertexshader);
}
//*************************************************************************


void Renderer::RenderSceneToTexture(RenderTexture& texture, Camera& camera, std::vector<Entity>& entities, std::vector<Light>& lights, std::vector<Light>& pointLights,  std::vector< CollisionObject>& collisionObjects)
{

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
	gfx11.cb_vs_windowParams.data.window_width = (float)windowWidth;
	gfx11.cb_vs_windowParams.data.window_height = (float)windowHeight;

	for (int i = 0; i < lights.size(); ++i)
	{
		gfx11.cb_vs_lightsShader.data.lightProjectionMatrix[i] = lights[i].GetCamera()->GetProjectionMatrix();
		gfx11.cb_vs_lightsShader.data.lightViewMatrix[i] = lights[i].GetCamera()->GetViewMatrix();

		gfx11.cb_ps_lightsShader.data.dynamicLightColor[i] = DirectX::XMFLOAT4(lights[i].lightColor.x, lights[i].lightColor.y, lights[i].lightColor.z, 1.0f);
		gfx11.cb_ps_lightsShader.data.dynamicLightPosition[i] = DirectX::XMFLOAT4(lights[i].pos.x, lights[i].pos.y, lights[i].pos.z, 1.0f);
		gfx11.cb_ps_lightsShader.data.SpotlightDir[i] = DirectX::XMFLOAT4(lights[i].SpotDir.x, lights[i].SpotDir.y, lights[i].SpotDir.z, 1.0f);

		gfx11.cb_ps_lightsShader.data.lightType[i].x = lights[i].lightType;
		DirectX::XMFLOAT3 diff = DirectX::XMFLOAT3(camera.GetPositionFloat3().x - lights[i].pos.x, camera.GetPositionFloat3().y - lights[i].pos.y, camera.GetPositionFloat3().z - lights[i].pos.z);
		physx::PxVec3 diffVec = physx::PxVec3(diff.x, diff.y, diff.z);
		float dist = diffVec.dot(diffVec);
		gfx11.cb_ps_lightsShader.data.lightType[i].y = 0;
		gfx11.cb_ps_lightsShader.data.lightType[i].z = 0;
		gfx11.cb_ps_lightsShader.data.lightType[i].w = 0;

		gfx11.cb_ps_lightCull.data.RadiusAndcutOff[i].x = lights[i].radius;
		gfx11.cb_ps_lightCull.data.RadiusAndcutOff[i].y = lights[i].cutOff;
		gfx11.cb_ps_lightCull.data.RadiusAndcutOff[i].z = 0.0;
		gfx11.cb_ps_lightCull.data.RadiusAndcutOff[i].w = 0.0;
	}
	//gfx11.cb_ps_lightsShader.data.acceptedDistShadow = shadowDist;

	gfx11.cb_ps_lightsShader.data.acceptedDistShadowAndLight.x = shadowDist;
	gfx11.cb_ps_lightsShader.data.acceptedDistShadowAndLight.y = acceptedDist;
	gfx11.cb_vs_lightsShader.data.lightsSize = lights.size();

	for (int i = 0; i < pointLights.size(); ++i)
	{
		gfx11.cb_ps_pointLightsShader.data.dynamicLightColor[i] = DirectX::XMFLOAT4(pointLights[i].lightColor.x, pointLights[i].lightColor.y, pointLights[i].lightColor.z, 1.0f);
		gfx11.cb_ps_pointLightsShader.data.dynamicLightPosition[i] = DirectX::XMFLOAT4(pointLights[i].pos.x, pointLights[i].pos.y, pointLights[i].pos.z, 1.0f);

		gfx11.cb_ps_pointLightCull.data.RadiusAndcutOff[i].x = pointLights[i].radius;
		gfx11.cb_ps_pointLightCull.data.RadiusAndcutOff[i].y = pointLights[i].cutOff;
		gfx11.cb_ps_pointLightCull.data.RadiusAndcutOff[i].z =	0.0f;
		gfx11.cb_ps_pointLightCull.data.RadiusAndcutOff[i].w =	0.0f;

	}
	gfx11.cb_ps_pointLightsShader.data.pointLightsSize = pointLights.size();

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

	gfx11.cb_ps_lightBuffer.data.viewProjMatrix = XMMatrixTranspose(camera.GetViewMatrix());

	gfx11.cb_vs_vertexshader.UpdateBuffer();
	gfx11.cb_vs_lightsShader.UpdateBuffer();
	gfx11.cb_vs_windowParams.UpdateBuffer();
	gfx11.cb_ps_lightsShader.UpdateBuffer();
	gfx11.cb_ps_PCFshader.UpdateBuffer();
	gfx11.cb_ps_lightCull.UpdateBuffer();
	gfx11.cb_ps_screenEffectBuffer.UpdateBuffer();
	gfx11.cb_ps_pbrBuffer.UpdateBuffer();
	gfx11.cb_ps_lightBuffer.UpdateBuffer();
	gfx11.cb_ps_pointLightsShader.UpdateBuffer();
	gfx11.cb_ps_pointLightCull.UpdateBuffer();
}

//********************************PBR*********************************

//**********************************************************************************


void Renderer::Render(Camera& camera, std::vector<Entity>& entities, PhysicsHandler& physicsHandler, std::vector<Light>& lights, std::vector<Light>& pointLights, std::vector< CollisionObject>& collisionObjects, GridClass& grid, std::vector<NavMeshClass>& navMeshes, std::vector<SoundComponent*>& sounds)
{
	//float rgb[4];
	rgb[0] = skyColor.x;
	rgb[1] = skyColor.y;
	rgb[2] = skyColor.z;
	rgb[3] = 1.0f;

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

	if (bEnableShadows)
	{
		for (int i = 0; i < lights.size(); ++i)
		{
			lights[i].UpdateCamera();

			if (lights[i].bShadow)
				shadowsRenderer.RenderShadows(gfx11, entities, lights[i], camera, renderShadowDistance, i);
		}
	}


	////////////
	environmentProbe.UpdateCamera();

	UpdateBuffers(lights, pointLights, camera);

	if (bHasFinishedLoading)
	{
		if (environmentProbe.recalculate)
		{
			gfx11.deviceContext->RSSetViewports(1, &gfx11.viewport);
			gfx11.deviceContext->OMSetRenderTargets(1, gfx11.renderTargetView.GetAddressOf(), gfx11.depthStencilView.Get());
			gfx11.deviceContext->ClearRenderTargetView(gfx11.renderTargetView.Get(), rgb);
			gfx11.deviceContext->ClearDepthStencilView(gfx11.depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

			environmentProbe.prevPos = environmentProbe.pos;
			environmentProbe.UpdateCamera();

			RenderToEnvProbe(environmentProbe, entities, lights, pointLights);
			PbrRender(camera);
			environmentProbe.pos = environmentProbe.prevPos;
			environmentProbe.recalculate = false;
		}
	}
	



	gfx11.deviceContext->RSSetViewports(1, &gfx11.viewport);
	gfx11.deviceContext->OMSetRenderTargets(1, gfx11.renderTargetView.GetAddressOf(), gfx11.depthStencilView.Get());
	gfx11.deviceContext->ClearRenderTargetView(gfx11.renderTargetView.Get(), rgb);
	gfx11.deviceContext->ClearDepthStencilView(gfx11.depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	RenderSceneToTexture(gfx11.renderTexture, camera, entities, lights,pointLights, collisionObjects);


	//////////////BLOOM///////////////////////////////////////
	if (enablePostProccess)
	{
		BloomRender(camera);
	}
		gfx11.deviceContext->RSSetViewports(1, &gfx11.viewport);
		gfx11.deviceContext->OMSetRenderTargets(1, gfx11.renderTargetView.GetAddressOf(), gfx11.depthStencilView.Get());
		gfx11.deviceContext->ClearRenderTargetView(gfx11.renderTargetView.Get(), rgb);
		gfx11.deviceContext->ClearDepthStencilView(gfx11.depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	

	////////////////////////////////
	// 
	//////////////////////////////////////////////////////

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

	if (!runPhysics)
	{
		gfx11.deviceContext->IASetInputLayout(gfx11.testVS.GetInputLayout());
		gfx11.deviceContext->VSSetShader(gfx11.testVS.GetShader(), nullptr, 0);
		gfx11.deviceContext->PSSetShader(gfx11.lightPS.GetShader(), nullptr, 0);
		for (int i = 0; i < sounds.size(); ++i)
		{
			sounds[i]->Draw(gfx11.deviceContext.Get(), camera, gfx11.cb_vs_vertexshader);
		}
	}
	

	if (debugEnabled)
	{
		gfx11.deviceContext->PSSetShader(gfx11.testPS.GetShader(), nullptr, 0);
		rectSmall.pos = DirectX::XMFLOAT3(2.88, -1.56, 2.878);
		//gfx11.deviceContext->PSSetShaderResources(0, 1, &PrefilterCubeTextures[0].shaderResourceView);
		gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState2D.Get(), 0);
		gfx11.deviceContext->IASetInputLayout(gfx11.vs2D.GetInputLayout());
		gfx11.deviceContext->VSSetShader(gfx11.vs2D.GetShader(), nullptr, 0);
		rectSmall.Draw(gfx11.deviceContext.Get(), camera, gfx11.cb_vs_vertexshader);

		//debugCube.SetRenderTexture(gfx11.deviceContext.Get(), environmentProbe.environmentCubeMap);
		//debugCube.SetRenderTexture(gfx11.deviceContext.Get(), irradianceCubeMap);
		debugCube.SetRenderTexture(gfx11.deviceContext.Get(), prefilterCubeMap);
		gfx11.deviceContext->PSSetShader(gfx11.cubeMapPS.GetShader(), nullptr, 0);
		gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState.Get(), 0);
		gfx11.deviceContext->IASetInputLayout(gfx11.pbrVS.GetInputLayout());
		gfx11.deviceContext->VSSetShader(gfx11.pbrVS.GetShader(), nullptr, 0);
		debugCube.pos = DirectX::XMFLOAT3(0, 0, 0);
		debugCube.Draw(gfx11.deviceContext.Get(), camera, gfx11.cb_vs_vertexshader);
		
		gfx11.deviceContext->PSSetShader(gfx11.colorPS.GetShader(), nullptr, 0);
		environmentProbe.Draw(camera);
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

	bool open = false;

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
			ImGui::MenuItem("Gui", NULL, &bGuiEnabled);

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

	if (bGuiEnabled)
	{
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
			ImGui::Checkbox("bEnableShadows", &bEnableShadows);
			ImGui::Checkbox("enablePostProccess", &enablePostProccess);
			ImGui::Checkbox("debugEnabled", &debugEnabled);
			ImGui::Checkbox("renderCollision", &bRenderCollision);
			ImGui::DragInt("vSync", &vSync);
			ImGui::DragFloat("bloomStrengh", &bloomStrengh, 0.1f);
			ImGui::DragFloat("bloomBrightness", &bloomBrightness, 0.1f);
			ImGui::DragFloat("gamma", &gamma, 0.1f);
			ImGui::InputInt("cameraMode", &switchCameraMode);
			ImGui::DragFloat("renderDistance", &renderDistance, 1.0f);
			ImGui::DragFloat("renderShadowDistance", &renderShadowDistance, 1.0f);
			ImGui::DragFloat("shadowDist", &shadowDist, 0.1f);
			ImGui::DragFloat("acceptedDist", &acceptedDist, 0.1f);
			ImGui::DragFloat3("sky color", &skyColor.x, 0.05f);
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
			if (ImGui::Button("Add"))
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

		ImGui::NewLine();
		ImGui::NewLine();

		if (ImGui::CollapsingHeader("Entities"))
		{
			//static int listbox_item_current = -1;
			std::vector<const char*> objNames;
			for (int i = 0; i < entities.size(); ++i)
			{

				//entities[i].entityName = "Entity" + std::to_string(i);
				//if (!entities[i].isDeleted)
				//{
					objNames.push_back(entities[i].entityName.c_str());
				//}
			}

			if (listbox_item_current > objNames.size() - 1)
			{
				listbox_item_current = -1;
			}
			ImGui::ListBox("Objects", &listbox_item_current, objNames.data(), objNames.size());

			if (listbox_item_current > -1)
			{
				for (int i = 0; i < entities.size(); ++i)
				{
					if (!entities[i].isDeleted)
					{
						if (entities[i].entityName == objNames[listbox_item_current])
						{
							if (entities[i].physicsComponent.aActor)
							{
								physx::PxShape* _shape = nullptr;
								entities[i].physicsComponent.aActor->getShapes(&_shape, entities[i].physicsComponent.aActor->getNbShapes());
								if (_shape)
									_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);


							}
							else if (entities[i].physicsComponent.aStaticActor)
							{
								physx::PxShape* _shape = nullptr;
								entities[i].physicsComponent.aStaticActor->getShapes(&_shape, entities[i].physicsComponent.aStaticActor->getNbShapes());
								if(_shape)
									_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);
							}
							entities[i].DrawGui(*physicsHandler.aScene, entities);
						}
						else
						{
							if (entities[i].physicsComponent.aActor)
							{
								physx::PxShape* _shape = nullptr;
								entities[i].physicsComponent.aActor->getShapes(&_shape, entities[i].physicsComponent.aActor->getNbShapes());
								if (_shape)
									_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);
							}
							else if (entities[i].physicsComponent.aStaticActor)
							{
								physx::PxShape* _shape = nullptr;
								entities[i].physicsComponent.aStaticActor->getShapes(&_shape, entities[i].physicsComponent.aStaticActor->getNbShapes());
								if (_shape)
									_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);
							}
						}
					}
					
				}
			}
			else
			{
				for (int i = 0; i < entities.size(); ++i)
				{
					if (!entities[i].isDeleted)
					{
						if (entities[i].isSelected)
						{
							for (int j = 0; j < objNames.size(); ++j)
							{
								if (objNames[j] == entities[i].entityName)
								{
									listbox_item_current = j;
									entities[i].DrawGui(*physicsHandler.aScene, entities);
								}
							}
						}
					}
					
					
				}
			}
		}

		ImGui::NewLine();
		ImGui::NewLine();
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
				if (objNames[listbox_item_current] == collisionObjects[i].entityName)
				{
					collisionObjects[i].DrawGUI(objNames[listbox_item_current]);
				}
			}
			if (ImGui::Button("AddCollisionObject"))
			{
				bAddCollisionObject = true;
			}
		}


		ImGui::NewLine();
		ImGui::NewLine();
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
		ImGui::NewLine();
		ImGui::NewLine();
		if (ImGui::CollapsingHeader("Sounds"))
		{
			for (int i = 0; i < sounds.size(); ++i)
			{
				sounds[i]->cube.DrawGUI("sound" + std::to_string(i));
			}
		}

		ImGui::NewLine();
		ImGui::NewLine();
		if (ImGui::CollapsingHeader("EnvironmentProbe"))
		{
			environmentProbe.DrawGui("Probe1");
		}

		ImGui::NewLine();
		ImGui::NewLine();


		ImGui::NewLine();
		ImGui::NewLine();

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
	gfx11.deviceContext->RSSetViewports(1, &texture.m_viewport);
	texture.SetRenderTarget(gfx11.deviceContext.Get(), texture.m_depthStencilView);
	texture.ClearRenderTarget(gfx11.deviceContext.Get(), texture.m_depthStencilView, 0, 0, 0, 1);

	gfx11.deviceContext->PSSetShader(gfx11.brdfPS.GetShader(), nullptr, 0);
	gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState.Get(), 0);
	gfx11.deviceContext->IASetInputLayout(gfx11.vs2D.GetInputLayout());
	gfx11.deviceContext->VSSetShader(gfx11.vs2D.GetShader(), nullptr, 0);
	this->gfx11.deviceContext->RSSetState(this->gfx11.rasterizerState.Get());
	rect.Draw(this->gfx11.deviceContext.Get(), camera, this->gfx11.cb_vs_vertexshader);

	//HRESULT hr = DirectX::SaveWICTextureToFile(gfx11.deviceContext.Get(), texture.m_renderTargetTexture, GUID_ContainerFormatJpeg, L"BrdfTexture.JPG", nullptr, nullptr, true);
}
void Renderer::IrradianceConvolutionRender(Camera& camera)
{
	debugCube.pos = DirectX::XMFLOAT3(0, 0, 0);
	//environmentProbe.UpdateCamera(512, 512);
	//debugCube.pos = environmentProbe.pos;
	environmentProbe.pos = debugCube.pos;
	environmentProbe.UpdateCamera();

	unsigned int _width = 32;
	unsigned int _height = 32;
	unsigned int maxMipLevels = 1;
	unsigned int mip = 0;
	irradianceCubeMap.CreateCubeMap(gfx11.device.Get(), gfx11.deviceContext.Get(), _width, _height, maxMipLevels);
	irradianceCubeMap.CreateCubeMapMipLevels(gfx11.device.Get(), gfx11.deviceContext.Get(), _width, _height, mip);
	gfx11.deviceContext->RSSetViewports(1, &irradianceCubeMap.m_viewport);

	gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState.Get(), 0);
	gfx11.deviceContext->PSSetSamplers(0, 1, gfx11.samplerState_Wrap.GetAddressOf());
	gfx11.deviceContext->PSSetSamplers(2, 1, gfx11.samplerState_MipMap.GetAddressOf());
	gfx11.deviceContext->RSSetState(gfx11.rasterizerState.Get());
	gfx11.deviceContext->OMSetBlendState(gfx11.blendState.Get(), NULL, 0xFFFFFFFF);



	for (int i = 0; i < 6; ++i)
	{
		irradianceCubeMap.RenderCubeMapFaces(gfx11.device.Get(), gfx11.deviceContext.Get(), i, gfx11.depthStencilView.Get(),rgb, false);
		float roughness = (float)mip / (float)(maxMipLevels - 1);
		gfx11.cb_ps_pbrBuffer.data.roughness = roughness;
		gfx11.cb_ps_pbrBuffer.UpdateBuffer();

		gfx11.deviceContext->PSSetShader(gfx11.irradianceConvPS.GetShader(), nullptr, 0);
		gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState.Get(), 0);
		gfx11.deviceContext->IASetInputLayout(gfx11.pbrVS.GetInputLayout());
		gfx11.deviceContext->VSSetShader(gfx11.pbrVS.GetShader(), nullptr, 0);
		this->gfx11.deviceContext->RSSetState(this->gfx11.rasterizerState.Get());
		gfx11.deviceContext->PSSetSamplers(0, 1, gfx11.samplerState_Wrap.GetAddressOf());
		gfx11.deviceContext->PSSetSamplers(1, 1, gfx11.samplerState_Clamp.GetAddressOf());
		gfx11.deviceContext->PSSetSamplers(2, 1, gfx11.samplerState_MipMap.GetAddressOf());

		DirectX::XMMATRIX viewMatrix = environmentProbe.viewMatrices[i];
		DirectX::XMMATRIX projectionMatrix = environmentProbe.projectionMatrices[i];

		debugCube.SetRenderTexture(gfx11.deviceContext.Get(), environmentProbe.environmentCubeMap);
		gfx11.deviceContext->PSSetShaderResources(0, 1, &environmentProbe.environmentCubeMap.shaderResourceView);
		debugCube.Draw(gfx11.deviceContext.Get(), viewMatrix, projectionMatrix, gfx11.cb_vs_vertexshader);
	}
	
}

void Renderer::PrifilterRender(Camera& camera)
{
	debugCube.pos = DirectX::XMFLOAT3(0, 0, 0);
	
	//debugCube.pos = environmentProbe.pos;

	environmentProbe.pos = debugCube.pos;
	//environmentProbe.pos = debugCube.pos;
	environmentProbe.UpdateCamera();
	

	unsigned int maxMipLevels = 5;

	unsigned int _width = 128;
	unsigned int _height = 128;
	prefilterCubeMap.CreateCubeMap(gfx11.device.Get(), gfx11.deviceContext.Get(), _width, _height, maxMipLevels);

	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		unsigned int mipWidth = static_cast<unsigned int>(_width * std::pow(0.5, mip));
		unsigned int mipHeight = static_cast<unsigned int>(_height * std::pow(0.5, mip));

		prefilterCubeMap.CreateCubeMapMipLevels(gfx11.device.Get(), gfx11.deviceContext.Get(), mipWidth, mipHeight, mip);
		gfx11.deviceContext->RSSetViewports(1, &prefilterCubeMap.m_viewport);

		for (int i = 0; i < 6; ++i)
		{
			prefilterCubeMap.RenderCubeMapFaces(gfx11.device.Get(), gfx11.deviceContext.Get(), i, gfx11.depthStencilView.Get(), rgb,false,true);
			float roughness = (float)mip / (float)(maxMipLevels-1);
			gfx11.cb_ps_pbrBuffer.data.roughness = roughness;
			gfx11.cb_ps_pbrBuffer.UpdateBuffer();

			gfx11.deviceContext->PSSetShader(gfx11.prefilterPS.GetShader(), nullptr, 0);
			gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState.Get(), 0);
			gfx11.deviceContext->IASetInputLayout(gfx11.pbrVS.GetInputLayout());
			gfx11.deviceContext->VSSetShader(gfx11.pbrVS.GetShader(), nullptr, 0);
			this->gfx11.deviceContext->RSSetState(this->gfx11.rasterizerState.Get());
			gfx11.deviceContext->PSSetSamplers(0, 1, gfx11.samplerState_Wrap.GetAddressOf());
			gfx11.deviceContext->PSSetSamplers(1, 1, gfx11.samplerState_Clamp.GetAddressOf());
			gfx11.deviceContext->PSSetSamplers(2, 1, gfx11.samplerState_MipMap.GetAddressOf());

			DirectX::XMMATRIX viewMatrix = environmentProbe.viewMatrices[i];
			DirectX::XMMATRIX projectionMatrix = environmentProbe.projectionMatrices[i];

			debugCube.SetRenderTexture(gfx11.deviceContext.Get(), environmentProbe.environmentCubeMap);
			gfx11.deviceContext->PSSetShaderResources(0, 1, &environmentProbe.environmentCubeMap.shaderResourceView);
			debugCube.Draw(gfx11.deviceContext.Get(), viewMatrix, projectionMatrix, gfx11.cb_vs_vertexshader);

		}
	}
}

void Renderer::PbrRender(Camera& camera)
{
	BrdfRender(camera, brdfTexture);
	IrradianceConvolutionRender(camera);
	PrifilterRender(camera);
}

void Renderer::RenderToEnvProbe(EnvironmentProbe& probe, std::vector<Entity>& entities, std::vector<Light>& lights, std::vector<Light>& pointLights)
{
	//UpdateBuffers(lights,pointLights, camera);
	environmentProbe.UpdateCamera();
	//float rgb[4] = { 0.0f,0.0f,0.0f,1.0f };
	unsigned int _width = 256;
	unsigned int _height = 256;
	unsigned int maxMipLevels = 1;
	unsigned int mip = 0;
	environmentProbe.environmentCubeMap.CreateCubeMap(gfx11.device.Get(), gfx11.deviceContext.Get(), _width, _height, maxMipLevels);
	environmentProbe.environmentCubeMap.CreateCubeMapMipLevels(gfx11.device.Get(), gfx11.deviceContext.Get(), _width, _height, mip);
	gfx11.deviceContext->RSSetViewports(1, &environmentProbe.environmentCubeMap.m_viewport);

	gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState.Get(), 0);
	gfx11.deviceContext->PSSetSamplers(0, 1, gfx11.samplerState_Wrap.GetAddressOf());
	gfx11.deviceContext->PSSetSamplers(2, 1, gfx11.samplerState_MipMap.GetAddressOf());
	gfx11.deviceContext->RSSetState(gfx11.rasterizerState.Get());
	gfx11.deviceContext->OMSetBlendState(gfx11.blendState.Get(), NULL, 0xFFFFFFFF);

	///////////////////////////////////////////////////////
	//////////////////////////////////////////////////////
	gfx11.deviceContext->VSSetConstantBuffers(0, 1, gfx11.cb_vs_vertexshader.GetBuffer().GetAddressOf());
	gfx11.deviceContext->RSSetState(gfx11.rasterizerState.Get());

	gfx11.deviceContext->PSSetShader(gfx11.envProbePS.GetShader(), nullptr, 0);
	gfx11.deviceContext->OMSetBlendState(gfx11.blendState.Get(), NULL, 0xFFFFFFFF);

	for (int face = 0; face < 6; ++face)
	{
	
		environmentProbe.environmentCubeMap.RenderCubeMapFaces(gfx11.device.Get(), gfx11.deviceContext.Get(), face, gfx11.depthStencilView.Get(), rgb,true);
		for (int i = 0; i < entities.size(); ++i)
		{
			if (entities[i].physicsComponent.mass == 0.0f)
			{
				if (entities[i].isEmissive)
				{
					gfx11.deviceContext->PSSetShader(gfx11.lightPS.GetShader(), nullptr, 0);
					gfx11.cb_ps_lightBuffer.data.color = entities[i].emissiveColor;
					gfx11.cb_ps_lightBuffer.UpdateBuffer();
				}
				else
					gfx11.deviceContext->PSSetShader(gfx11.envProbePS.GetShader(), nullptr, 0);

				gfx11.deviceContext->IASetInputLayout(gfx11.pbrVS.GetInputLayout());
				gfx11.deviceContext->VSSetShader(gfx11.pbrVS.GetShader(), nullptr, 0);
				entities[i].Draw(probe.camera[face], probe.camera[face].GetViewMatrix(), probe.camera[face].GetProjectionMatrix());
			}

		}
	}
}

//********************************PBR************************************************
//***********************************************************************************



//********************************BLOOM************************************************
//***********************************************************************************
void Renderer::BloomRender(Camera& camera)
{
	rectBloom.pos = DirectX::XMFLOAT3(0, 0, 0);
	gfx11.deviceContext->RSSetViewports(1, &bloomRenderTexture.m_viewport);
	bloomRenderTexture.SetRenderTarget(gfx11.deviceContext.Get(), bloomRenderTexture.m_depthStencilView);
	bloomRenderTexture.ClearRenderTarget(gfx11.deviceContext.Get(), bloomRenderTexture.m_depthStencilView, 0, 0, 0, 1.0f);

	gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState2D.Get(), 0);
	gfx11.deviceContext->VSSetShader(gfx11.vs2D.GetShader(), NULL, 0);
	gfx11.deviceContext->IASetInputLayout(gfx11.vs2D.GetInputLayout());
	gfx11.deviceContext->PSSetShader(gfx11.bloomLightPS.GetShader(), NULL, 0);
	gfx11.deviceContext->PSSetShaderResources(0, 1, &gfx11.renderTexture.shaderResourceView);

	rect.Draw(gfx11.deviceContext.Get(), camera, gfx11.cb_vs_vertexshader);

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


void Renderer::RenderEntitiesPostProcess(std::vector<Entity>& entities,std::vector<Light>& lights, std::vector<Light>& pointLights, PixelShader& psShader, Camera& camera)
{
	gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState.Get(), 0);
	gfx11.deviceContext->PSSetSamplers(0, 1, gfx11.samplerState_Wrap.GetAddressOf());
	gfx11.deviceContext->PSSetSamplers(2, 1, gfx11.samplerState_MipMap.GetAddressOf());
	gfx11.deviceContext->RSSetState(gfx11.rasterizerState.Get());
	gfx11.deviceContext->OMSetBlendState(gfx11.blendState.Get(), NULL, 0xFFFFFFFF);


	///////////////////////////////////////////////////////
	//////////////////////////////////////////////////////
	gfx11.deviceContext->VSSetConstantBuffers(0, 1, gfx11.cb_vs_vertexshader.GetBuffer().GetAddressOf());
	gfx11.deviceContext->RSSetState(gfx11.rasterizerState.Get());

	gfx11.deviceContext->OMSetBlendState(gfx11.blendState.Get(), NULL, 0xFFFFFFFF);
	gfx11.deviceContext->PSSetShader(psShader.GetShader(), NULL, 0);
	for (int i = 0; i < entities.size(); ++i)
	{
		if (entities[i].isEmissive)
			gfx11.deviceContext->PSSetShader(gfx11.lightPS.GetShader(), NULL, 0);
		else
			gfx11.deviceContext->PSSetShader(psShader.GetShader(), NULL, 0);
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
	
	RenderEntitiesPostProcess(entities,lights,pointLights, gfx11.depthPS, camera);

	gfx11.deviceContext->IASetInputLayout(gfx11.depthVS.GetInputLayout());
	gfx11.deviceContext->VSSetShader(gfx11.depthVS.GetShader(), nullptr, 0);
	gfx11.deviceContext->PSSetShader(gfx11.lightPS.GetShader(), nullptr, 0);
	for (int i = 0; i < lights.size(); ++i)
	{
		lights[i].scale = DirectX::XMFLOAT3(0.05, 0.05, 0.05);
	
		lights[i].Draw(camera);
		lights[i].scale = DirectX::XMFLOAT3(0.01, 0.01, 0.01);
	}

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
	
	rect.Draw(gfx11.deviceContext.Get(), camera, gfx11.cb_vs_vertexshader);
}
