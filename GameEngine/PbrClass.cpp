#include "PbrClass.h"

PbrClass::PbrClass()
{
}

void PbrClass::Initialize(DX11& gfx11)
{
	brdfTexture.Initialize(gfx11.device.Get(), 512, 512, DXGI_FORMAT_R8G8B8A8_UNORM);
}

void PbrClass::BrdfRender(DX11& gfx11, RectShape& rect, Camera& camera, RenderTexture& texture)
{
	gfx11.deviceContext->RSSetViewports(1, &texture.m_viewport);
	texture.SetRenderTarget(gfx11.deviceContext.Get(), texture.m_depthStencilView);
	texture.ClearRenderTarget(gfx11.deviceContext.Get(), texture.m_depthStencilView, 0, 0, 0, 1);

	gfx11.deviceContext->PSSetShader(gfx11.brdfPS.GetShader(), nullptr, 0);
	gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState.Get(), 0);
	gfx11.deviceContext->IASetInputLayout(gfx11.vs2D.GetInputLayout());
	gfx11.deviceContext->VSSetShader(gfx11.vs2D.GetShader(), nullptr, 0);
	gfx11.deviceContext->RSSetState(gfx11.rasterizerState.Get());
	rect.Draw(gfx11.deviceContext.Get(), camera, gfx11.cb_vs_vertexshader);
}

void PbrClass::IrradianceConvolutionRender(DX11& gfx11, CubeShape debugCube, EnvironmentProbe& environmentProbe, Camera& camera, float* rgb)
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
	irradianceCubeMap.CreateCubeMap(gfx11.device.Get(), gfx11.deviceContext.Get(), _width, _height, DXGI_FORMAT_R16G16B16A16_FLOAT, maxMipLevels);
	irradianceCubeMap.CreateCubeMapMipLevels(gfx11.device.Get(), gfx11.deviceContext.Get(), _width, _height, mip);
	gfx11.deviceContext->RSSetViewports(1, &irradianceCubeMap.m_viewport);

	gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState.Get(), 0);
	gfx11.deviceContext->PSSetSamplers(0, 1, gfx11.samplerState_Wrap.GetAddressOf());
	gfx11.deviceContext->PSSetSamplers(2, 1, gfx11.samplerState_MipMap.GetAddressOf());
	gfx11.deviceContext->RSSetState(gfx11.rasterizerState.Get());
	gfx11.deviceContext->OMSetBlendState(gfx11.blendState.Get(), NULL, 0xFFFFFFFF);



	for (int i = 0; i < 6; ++i)
	{
		irradianceCubeMap.RenderCubeMapFaces(gfx11.device.Get(), gfx11.deviceContext.Get(), i, gfx11.depthStencilView.Get(), rgb, false);
		float roughness = (float)mip / (float)(maxMipLevels - 1);
		gfx11.cb_ps_pbrBuffer.data.roughness = roughness;
		gfx11.cb_ps_pbrBuffer.UpdateBuffer();

		gfx11.deviceContext->PSSetShader(gfx11.irradianceConvPS.GetShader(), nullptr, 0);
		gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState.Get(), 0);
		gfx11.deviceContext->IASetInputLayout(gfx11.deferredVS.GetInputLayout());
		gfx11.deviceContext->VSSetShader(gfx11.deferredVS.GetShader(), nullptr, 0);
		gfx11.deviceContext->RSSetState(gfx11.rasterizerState.Get());
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

void PbrClass::PrifilterRender(DX11& gfx11, CubeShape debugCube, EnvironmentProbe& environmentProbe, Camera& camera, float* rgb)
{
	debugCube.pos = DirectX::XMFLOAT3(0, 0, 0);

	//debugCube.pos = environmentProbe.pos;

	environmentProbe.pos = debugCube.pos;
	//environmentProbe.pos = debugCube.pos;
	environmentProbe.UpdateCamera();


	unsigned int maxMipLevels = 5;

	unsigned int _width = 128;
	unsigned int _height = 128;
	prefilterCubeMap.CreateCubeMap(gfx11.device.Get(), gfx11.deviceContext.Get(), _width, _height, DXGI_FORMAT_R16G16B16A16_FLOAT, maxMipLevels);

	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		unsigned int mipWidth = static_cast<unsigned int>(_width * std::pow(0.5, mip));
		unsigned int mipHeight = static_cast<unsigned int>(_height * std::pow(0.5, mip));

		prefilterCubeMap.CreateCubeMapMipLevels(gfx11.device.Get(), gfx11.deviceContext.Get(), mipWidth, mipHeight, mip);
		gfx11.deviceContext->RSSetViewports(1, &prefilterCubeMap.m_viewport);

		for (int i = 0; i < 6; ++i)
		{
			prefilterCubeMap.RenderCubeMapFaces(gfx11.device.Get(), gfx11.deviceContext.Get(), i, gfx11.depthStencilView.Get(), rgb, false, true);
			float roughness = (float)mip / (float)(maxMipLevels - 1);
			gfx11.cb_ps_pbrBuffer.data.roughness = roughness;
			gfx11.cb_ps_pbrBuffer.UpdateBuffer();

			gfx11.deviceContext->PSSetShader(gfx11.prefilterPS.GetShader(), nullptr, 0);
			gfx11.deviceContext->OMSetDepthStencilState(gfx11.depthStencilState.Get(), 0);
			gfx11.deviceContext->IASetInputLayout(gfx11.deferredVS.GetInputLayout());
			gfx11.deviceContext->VSSetShader(gfx11.deferredVS.GetShader(), nullptr, 0);
			gfx11.deviceContext->RSSetState(gfx11.rasterizerState.Get());
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

void PbrClass::PbrRender(DX11& gfx11, RectShape& rect, CubeShape debugCube, EnvironmentProbe& environmentProbe, Camera& camera, float* rgb)
{
	BrdfRender(gfx11, rect,camera, brdfTexture);
	IrradianceConvolutionRender(gfx11,debugCube, environmentProbe,camera,rgb);
	PrifilterRender(gfx11, debugCube, environmentProbe, camera, rgb);
}
