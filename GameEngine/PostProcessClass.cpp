#include "PostProcessClass.h"
#include <random>


PostProcessClass::PostProcessClass()
{
	radius = 0.01f;
	bias = 0.5f;
	sharpness = 16.f;
	powerExponent = 2.f;
	metersToViewSpaceUnits = 1.0f;
	largeScaleAO = 1.0f;
	smallScaleAO = 1.0f;
	decodeBias = 1.0f;
	decodeScale = 0.3f;
}

void PostProcessClass::Initialize(DX11& gfx11, int width, int height)
{
	BloomHorizontalBlurTexture.Initialize(gfx11.device.Get(), width/2, height/2, DXGI_FORMAT_R16G16B16A16_FLOAT);
	BloomVerticalBlurTexture.Initialize(gfx11.device.Get(), width/2, height/2, DXGI_FORMAT_R16G16B16A16_FLOAT);

	bloomRenderTexture.Initialize(gfx11.device.Get(), width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);
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

void PostProcessClass::HbaoPlusInit(DX11& gfx11, int width, int height)
{
	
	hbaoTexture.Initialize(gfx11.device.Get(), width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);
	
	GFSDK_SSAO_CustomHeap customHeap;
	customHeap.new_ = ::operator new;
	customHeap.delete_ = ::operator delete;


	status = GFSDK_SSAO_CreateContext_D3D11(gfx11.device.Get(), &pAOContext, &customHeap);
	assert(status == GFSDK_SSAO_OK);
}

void PostProcessClass::HbaoPlusRender(DX11& gfx11, RectShape& rect, Camera& camera, ID3D11ShaderResourceView* depthView, ID3D11ShaderResourceView* normalView)
{
	GFSDK_SSAO_Parameters Params;
	Params.LargeScaleAO = largeScaleAO;
	Params.SmallScaleAO = smallScaleAO;
	//Params.StepCount = GFSDK_SSAO_STEP_COUNT_4;
	//Params.DepthClampMode = GFSDK_SSAO_DepthClampMode::GFSDK_SSAO_CLAMP_TO_EDGE;
	Params.Radius = radius;
	Params.Bias = bias;
	Params.PowerExponent = powerExponent;
	Params.Blur.Enable = true;
	Params.Blur.Radius = GFSDK_SSAO_BlurRadius::GFSDK_SSAO_BLUR_RADIUS_4;
	Params.Blur.Sharpness = sharpness;
	Params.DepthStorage = GFSDK_SSAO_FP16_VIEW_DEPTHS;

	GFSDK_SSAO_InputData_D3D11 Input;
	Input.DepthData.DepthTextureType = GFSDK_SSAO_DepthTextureType::GFSDK_SSAO_HARDWARE_DEPTHS;
	Input.DepthData.pFullResDepthTextureSRV = depthView;
	Input.NormalData.Enable = true;

	Input.NormalData.DecodeBias = decodeBias;
	Input.NormalData.DecodeScale = decodeScale;
	Input.NormalData.pFullResNormalTextureSRV = normalView;
	DirectX::XMMATRIX worldMat =  DirectX::XMMatrixRotationRollPitchYawFromVector(camera.GetRotationVector());
	DirectX::XMMATRIX worldView = (camera.GetViewMatrix()) * (worldMat);
	Input.NormalData.WorldToViewMatrix.Layout = GFSDK_SSAO_ROW_MAJOR_ORDER;
	Input.NormalData.WorldToViewMatrix.Data = GFSDK_SSAO_Float4x4((const GFSDK_SSAO_FLOAT*)&worldView);
	DirectX::XMMATRIX proj = camera.GetProjectionMatrix();
	Input.DepthData.ProjectionMatrix.Data = GFSDK_SSAO_Float4x4((const GFSDK_SSAO_FLOAT*)&proj);
	Input.DepthData.ProjectionMatrix.Layout = GFSDK_SSAO_ROW_MAJOR_ORDER;
	Input.DepthData.MetersToViewSpaceUnits = metersToViewSpaceUnits;
	

	GFSDK_SSAO_RenderMask RenderMask = GFSDK_SSAO_RenderMask::GFSDK_SSAO_RENDER_AO;

	GFSDK_SSAO_Output_D3D11 Output;
	Output.pRenderTargetView = hbaoTexture.m_renderTargetView;
	Output.Blend.Mode = GFSDK_SSAO_OVERWRITE_RGB;
	status = pAOContext->RenderAO(gfx11.deviceContext.Get(), Input, Params, Output, RenderMask);
	assert(status == GFSDK_SSAO_OK);
}
