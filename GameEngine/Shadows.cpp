#include "Shadows.h"

Shadows::Shadows()
{
}

void Shadows::RenderToTexture(DX11& gfx11, std::vector<Entity>& entities, Camera& camera, RenderTexture& shadowMap, Light& light)
{
	light.UpdateCamera();
	shadowMap.SetRenderTarget(gfx11.deviceContext.Get(), shadowMap.m_depthStencilView);
	shadowMap.ClearRenderTarget(gfx11.deviceContext.Get(), shadowMap.m_depthStencilView, 0.0f, 0.0f, 0.0f, 1.0f);

	RenderShadowEntities(gfx11, entities, light, camera);
}

void Shadows::RenderShadowEntities(DX11& gfx11, std::vector<Entity>& entities, Light& light, Camera& camera)
{
	XMMATRIX viewMatrix = (light.lightViewMatrix);
	XMMATRIX projectionMatrix = (light.lightProjectionMatrix);
	gfx11.deviceContext->PSSetShader(gfx11.depthPS.GetShader(), NULL, 0);

	for (int i = 0; i < entities.size(); ++i)
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

		entities[i].Draw(camera, viewMatrix, projectionMatrix);
	}
}

void Shadows::RenderShadows(DX11& gfx11, std::vector<Entity>& entities, Light& light, Camera& camera, int& index)
{
	light.UpdateCamera();
	gfx11.cb_vs_lightsShader.data.lightViewMatrix[index] = DirectX::XMMatrixTranspose(light.lightViewMatrix);
	gfx11.cb_vs_lightsShader.data.lightProjectionMatrix[index] = DirectX::XMMatrixTranspose(light.lightProjectionMatrix);


	gfx11.cb_vs_lightsShader.UpdateBuffer();

	gfx11.deviceContext->RSSetViewports(1, &light.m_shadowMap.m_viewport);
	RenderToTexture(gfx11, entities, camera, light.m_shadowMap, light);
}
