#pragma once
#include "Entity.h"
#include "Camera.h"
#include "Light.h"
#include "DX11.h"
class Shadows
{
public:
	Shadows();
	void RenderToTexture(DX11& gfx11, std::vector<Entity>& entities, Camera& camera, RenderTexture& shadowMap, Light* light, float& renderDistance);
	void RenderShadowEntities(DX11& gfx11, std::vector<Entity>& entity, Light* light, Camera& camera, float& renderDistance);
	void RenderShadows(DX11& gfx11, std::vector<Entity>& entities, Light* light, Camera& camera, float& renderDistance, int& index);
};

