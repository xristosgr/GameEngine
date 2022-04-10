#include "GBufferClass.h"

GBufferClass::GBufferClass()
{
}

void GBufferClass::Initialize(DX11& gfx11)
{

	albedoTexture.Initialize(gfx11.device.Get(), gfx11.windowWidth, gfx11.windowHeight);
	normalTexture.Initialize(gfx11.device.Get(), gfx11.windowWidth, gfx11.windowHeight);
	metallicRoughnessTexture.Initialize(gfx11.device.Get(), gfx11.windowWidth, gfx11.windowHeight);
	worldPositionTexture.Initialize(gfx11.device.Get(), gfx11.windowWidth, gfx11.windowHeight);
	emmisiveTexture.Initialize(gfx11.device.Get(), gfx11.windowWidth, gfx11.windowHeight);

	m_renderTargetTextureArray[0] = albedoTexture.m_renderTargetTexture;
	m_renderTargetTextureArray[1] = normalTexture.m_renderTargetTexture;
	m_renderTargetTextureArray[2] = metallicRoughnessTexture.m_renderTargetTexture;
	m_renderTargetTextureArray[3] = worldPositionTexture.m_renderTargetTexture;
	m_renderTargetTextureArray[4] = emmisiveTexture.m_renderTargetTexture;

	m_renderTargetViewArray[0] = albedoTexture.m_renderTargetView;
	m_renderTargetViewArray[1] = normalTexture.m_renderTargetView;
	m_renderTargetViewArray[2] = metallicRoughnessTexture.m_renderTargetView;
	m_renderTargetViewArray[3] = worldPositionTexture.m_renderTargetView;
	m_renderTargetViewArray[4] = emmisiveTexture.m_renderTargetView;

	m_shaderResourceViewArray[0] = albedoTexture.shaderResourceView;
	m_shaderResourceViewArray[1] = normalTexture.shaderResourceView;
	m_shaderResourceViewArray[2] = metallicRoughnessTexture.shaderResourceView;
	m_shaderResourceViewArray[3] = worldPositionTexture.shaderResourceView;
	m_shaderResourceViewArray[4] = emmisiveTexture.shaderResourceView;
}

void GBufferClass::GeometryPass(DX11& gfx11, Camera& camera, ID3D11DepthStencilView* depthView, float* rgb)
{
	//gfx11.deviceContext->RSSetViewports(1, &albedoTexture.m_viewport);
	SetRenderTargets(gfx11, depthView);
	ClearRenderTargets(gfx11, depthView, rgb);
}

void GBufferClass::LightPass(DX11& gfx11, RectShape& rect, Camera& camera)
{
	gfx11.renderTexture.SetRenderTarget(gfx11.deviceContext.Get(), gfx11.renderTexture.m_depthStencilView);
	gfx11.renderTexture.ClearRenderTarget(gfx11.deviceContext.Get(), gfx11.renderTexture.m_depthStencilView, 0, 0, 0, 1);

	gfx11.deviceContext->PSSetShader(gfx11.pbrPS.GetShader(), nullptr, 0);
	gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState.Get(), 0);
	gfx11.deviceContext->IASetInputLayout(gfx11.pbrVS.GetInputLayout());
	gfx11.deviceContext->VSSetShader(gfx11.pbrVS.GetShader(), nullptr, 0);

	gfx11.deviceContext->VSSetShaderResources(0, 1, &m_shaderResourceViewArray[4]);
	gfx11.deviceContext->VSSetSamplers(0, 1, gfx11.samplerState_Wrap.GetAddressOf());
	gfx11.deviceContext->RSSetState(gfx11.rasterizerState.Get());

	for (int i = 0; i < BUFFER_COUNT; ++i)
	{
		gfx11.deviceContext->PSSetShaderResources(i, 1, &m_shaderResourceViewArray[i]);
	}
	
	rect.Draw(gfx11.deviceContext.Get(), camera, gfx11.cb_vs_vertexshader);

}

void GBufferClass::SetRenderTargets(DX11& gfx11, ID3D11DepthStencilView* depthView)
{
	gfx11.deviceContext->OMSetRenderTargets(BUFFER_COUNT, &m_renderTargetViewArray[0], depthView);
}

void GBufferClass::ClearRenderTargets(DX11& gfx11, ID3D11DepthStencilView* depthView, float* rgb)
{
	for (int i = 0; i < BUFFER_COUNT; ++i)
	{
		gfx11.deviceContext->ClearRenderTargetView(m_renderTargetViewArray[i], rgb);
	}
	gfx11.deviceContext->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
