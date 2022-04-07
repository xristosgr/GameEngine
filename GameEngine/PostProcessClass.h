#pragma once
#include"DX11.h"
#include "Entity.h"
#include "Light.h"

class PostProcessClass
{
public:
	PostProcessClass();
	void Initialize(DX11& gfx11, int width, int height);
	void BloomRender(DX11& gfx11, RectShape& rect, Camera& camera);


	RenderTexture bloomRenderTexture;
	RenderTexture BloomVerticalBlurTexture, BloomHorizontalBlurTexture, downSampleTexture;
	RectShape rectBloom;
};

