#pragma once
#include "Entity.h"
#include "Camera.h"
#include "Light.h"
#include "DX11.h"
class Shadows
{
public:
	Shadows();
	void Initialize(DX11& gfx11,int screen_width, int screen_height);
	void RenderToTexture(DX11& gfx11, std::vector<Entity>& entities, Camera& camera, RenderTexture& shadowMap, Light& light, float& renderDistance);
	void RenderShadowEntities(DX11& gfx11, std::vector<Entity>& entity, Light& light, Camera& camera, float& renderDistance);
	void RenderEntities(DX11& gfx11, std::vector<Entity>& entity, std::vector<Light>& lights, Camera& camera);
	void RenderShadows(DX11& gfx11, std::vector<Entity>& entities, Light& light, Camera& camera, float& renderDistance, int& index);
	void SoftShadows(DX11& gfx11, RectShape& blurRect, std::vector<Entity>& entities, std::vector<Light>& lights, Camera& camera, float& renderDistance);
	void BlurPass(RenderTexture& texture, DX11& gfx11, RectShape& blurRect, VertexShader& vertexShader, PixelShader& pixelShader, Camera& camera);
	RenderTexture shadowTexture, shadowVerticalBlurTexture, shadowHorizontalBlurTexture;
};

