#include "PostProcessClass.h"

PostProcessClass::PostProcessClass()
{
}

void PostProcessClass::Initialize(DX11& gfx11, int width, int height)
{
	BloomHorizontalBlurTexture.Initialize(gfx11.device.Get(), 800, 600);
	BloomVerticalBlurTexture.Initialize(gfx11.device.Get(), 800, 600);

	bloomRenderTexture.Initialize(gfx11.device.Get(), width, height);
	rectBloom.Initialize(gfx11.device.Get(), gfx11.windowWidth, gfx11.windowHeight);
}

void PostProcessClass::BloomRender(DX11& gfx11, RectShape& rect, Camera& camera)
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
