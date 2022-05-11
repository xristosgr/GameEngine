#include "Shadows.h"

Shadows::Shadows()
{
}

void Shadows::Initialize(DX11& gfx11, int& screen_width, int& screen_height)
{
	shadowTexture.Initialize(gfx11.device.Get(), screen_width, screen_height);
	shadowHorizontalBlurTexture.Initialize(gfx11.device.Get(), screen_width/2, screen_height/2);
	shadowVerticalBlurTexture.Initialize(gfx11.device.Get(), screen_width/2, screen_height/2);
}

void Shadows::RenderToTexture(DX11& gfx11, std::vector<Entity>& entities, Camera& camera, RenderTexture& shadowMap, Light& light, float& renderDistance)
{
	light.UpdateCamera();
	shadowMap.SetRenderTarget(gfx11.deviceContext.Get(), shadowMap.m_depthStencilView);
	shadowMap.ClearRenderTarget(gfx11.deviceContext.Get(), shadowMap.m_depthStencilView, 0.0f, 0.0f, 0.0f, 1.0f);

	RenderShadowEntities(gfx11, entities, light, camera, renderDistance);
}

void Shadows::RenderShadowEntities(DX11& gfx11, std::vector<Entity>& entities, Light& light, Camera& camera,float& renderDistance)
{
	DirectX::XMMATRIX viewMatrix = (light.lightViewMatrix);
	DirectX::XMMATRIX projectionMatrix = (light.lightProjectionMatrix);
	gfx11.deviceContext->PSSetShader(gfx11.depthPS.GetShader(), NULL, 0);

	for (int i = 0; i < entities.size(); ++i)
	{
		DirectX::XMFLOAT3 diff = DirectX::XMFLOAT3(camera.GetPositionFloat3().x - entities[i].pos.x, camera.GetPositionFloat3().y - entities[i].pos.y, camera.GetPositionFloat3().z - entities[i].pos.z);
		physx::PxVec3 diffVec = physx::PxVec3(diff.x, diff.y, diff.z);
		float dist = diffVec.dot(diffVec);

		if (dist < renderDistance)
		{
			if (entities[i].model.isAnimated)
			{
				gfx11.deviceContext->IASetInputLayout(gfx11.depthAnimVS.GetInputLayout());
				gfx11.deviceContext->VSSetShader(gfx11.depthAnimVS.GetShader(), nullptr, 0);
			}
			else
			{
				gfx11.deviceContext->IASetInputLayout(gfx11.depthVS.GetInputLayout());
				gfx11.deviceContext->VSSetShader(gfx11.depthVS.GetShader(), nullptr, 0);
			}

			if(light.lightType == 2.0f)
				entities[i].Draw(camera, viewMatrix, projectionMatrix,nullptr, false);
			else
				entities[i].Draw(camera, viewMatrix, projectionMatrix, nullptr, true);
		}
	}
}

void Shadows::RenderEntities(DX11& gfx11, std::vector<Entity>& entities, std::vector<Light>& lights, Camera& camera)
{
	DirectX::XMMATRIX viewMatrix = camera.GetViewMatrix();
	DirectX::XMMATRIX projectionMatrix = camera.GetProjectionMatrix();
	gfx11.deviceContext->PSSetShader(gfx11.shadowPS.GetShader(), NULL, 0);

	std::vector< ID3D11ShaderResourceView*> ShadowTextures;
	ShadowTextures.resize(lights.size());
	int index = 0;
	for (int j = 0; j < ShadowTextures.size(); ++j)
	{

		ShadowTextures[index] = lights[j].m_shadowMap.shaderResourceView;
		index++;



	}
	gfx11.deviceContext->PSSetShaderResources(4, ShadowTextures.size(), ShadowTextures.data());
	for (int i = 0; i < entities.size(); ++i)
	{
		if (entities[i].model.isAnimated)
		{
			gfx11.deviceContext->IASetInputLayout(gfx11.animVS.GetInputLayout());
			gfx11.deviceContext->VSSetShader(gfx11.animVS.GetShader(), nullptr, 0);
		}
		else
		{
			gfx11.deviceContext->IASetInputLayout(gfx11.shadowVS.GetInputLayout());
			gfx11.deviceContext->VSSetShader(gfx11.shadowVS.GetShader(), nullptr, 0);
		}

		entities[i].Draw(camera, viewMatrix, projectionMatrix);
		
	}
}

void Shadows::RenderShadows(DX11& gfx11, std::vector<Entity>& entities, Light& light, Camera& camera, float& renderDistance, int& index)
{
	light.UpdateCamera();
	gfx11.cb_vs_lightsShader.data.lightViewMatrix[index] = DirectX::XMMatrixTranspose(light.lightViewMatrix);
	gfx11.cb_vs_lightsShader.data.lightProjectionMatrix[index] = DirectX::XMMatrixTranspose(light.lightProjectionMatrix);


	gfx11.cb_vs_lightsShader.UpdateBuffer();

	gfx11.deviceContext->RSSetViewports(1, &light.m_shadowMap.m_viewport);
	RenderToTexture(gfx11, entities, camera, light.m_shadowMap, light, renderDistance);
}

void Shadows::SoftShadows(DX11& gfx11, RectShape& blurRect, std::vector<Entity>& entities, std::vector<Light>& lights, Camera& camera, float& renderDistance)
{
	//light.UpdateCamera();
	gfx11.deviceContext->RSSetViewports(1, &shadowTexture.m_viewport);
	shadowTexture.SetRenderTarget(gfx11.deviceContext.Get(), shadowTexture.m_depthStencilView);
	shadowTexture.ClearRenderTarget(gfx11.deviceContext.Get(), shadowTexture.m_depthStencilView, 0.0f, 0.0f, 0.0f, 1.0f);

	gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState.Get(), 0);
	gfx11.deviceContext->RSSetState(gfx11.rasterizerState.Get());

	gfx11.deviceContext->PSSetSamplers(0, 1, gfx11.samplerState_Wrap.GetAddressOf());
	gfx11.deviceContext->PSSetSamplers(1, 1, gfx11.samplerState_Clamp.GetAddressOf());
	RenderEntities(gfx11, entities, lights, camera);

	float rgb[4] = { 0,0,0,1 };
	gfx11.deviceContext->RSSetViewports(1, &gfx11.viewport);
	gfx11.deviceContext->OMSetRenderTargets(1, gfx11.renderTargetView.GetAddressOf(), gfx11.depthStencilView.Get());
	gfx11.deviceContext->ClearRenderTargetView(gfx11.renderTargetView.Get(), rgb);
	gfx11.deviceContext->ClearDepthStencilView(gfx11.depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	gfx11.deviceContext->PSSetShaderResources(0, 1, &shadowTexture.shaderResourceView);
	BlurPass(shadowHorizontalBlurTexture, gfx11, blurRect, gfx11.shadowHorizontalBlurVS, gfx11.shadowHorizontalBlurPS, camera);

	gfx11.deviceContext->RSSetViewports(1, &gfx11.viewport);
	gfx11.deviceContext->OMSetRenderTargets(1, gfx11.renderTargetView.GetAddressOf(), gfx11.depthStencilView.Get());
	gfx11.deviceContext->ClearRenderTargetView(gfx11.renderTargetView.Get(), rgb);
	gfx11.deviceContext->ClearDepthStencilView(gfx11.depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	gfx11.deviceContext->PSSetShaderResources(0, 1, &shadowHorizontalBlurTexture.shaderResourceView);
	BlurPass(shadowVerticalBlurTexture, gfx11, blurRect, gfx11.shadowVerticalBlurVS, gfx11.shadowVerticalBlurPS, camera);
}

void Shadows::BlurPass(RenderTexture& texture, DX11& gfx11, RectShape& blurRect, VertexShader& vertexShader, PixelShader& pixelShader, Camera& camera)
{
	gfx11.deviceContext->RSSetViewports(1, &texture.m_viewport);
	texture.SetRenderTarget(gfx11.deviceContext.Get(), texture.m_depthStencilView);
	texture.ClearRenderTarget(gfx11.deviceContext.Get(), texture.m_depthStencilView, 0.0f, 0.0f, 0.0f, 1.0f);


	DirectX::XMMATRIX viewMatrix = camera.GetViewMatrix();
	DirectX::XMMATRIX projectionMatrix = camera.GetProjectionMatrix();
	gfx11.deviceContext->PSSetShader(pixelShader.GetShader(), NULL, 0);

	gfx11.deviceContext->IASetInputLayout(vertexShader.GetInputLayout());
	gfx11.deviceContext->VSSetShader(vertexShader.GetShader(), nullptr, 0);

	blurRect.Draw(gfx11.deviceContext.Get(), camera, gfx11.cb_vs_vertexshader);
}
