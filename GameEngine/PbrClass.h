#pragma once
#include"DX11.h"
#include "Entity.h"
#include "Light.h"
#include "EnvironmentProbe.h"

class PbrClass
{
public:
	PbrClass();

	void Initialize(DX11& gfx11);
	void BrdfRender(DX11& gfx11, RectShape& rect, Camera& camera, RenderTexture& texture);
	void IrradianceConvolutionRender(DX11& gfx11, CubeShape debugCube, EnvironmentProbe& environmentProbe, Camera& camera, float* rgb);
	void PrifilterRender(DX11& gfx11, CubeShape debugCube, EnvironmentProbe& environmentProbe, Camera& camera, float* rgb);
	void PbrRender(DX11& gfx11, RectShape& rect, CubeShape debugCube, EnvironmentProbe& environmentProbe, Camera& camera, float* rgb);

	RenderTexture brdfTexture, prefilterCubeMap, irradianceCubeMap;
};

