#pragma once
#include"DX11.h"
#include "Light.h"
#include "Entity.h"
#include "Shadows.h"
#include "PhysicsDebugDraw.h"
#include "PhysicsHandler.h"
#include "ImGui/ImGuiFileBrowser.h"
#include "CollisionObject.h"
#include "GridClass.h"
#include "NavMeshClass.h"
#include "EnvironmentProbe.h"
#include <thread>
#include "AppTimer.h"
#include "SoundComponent.h"

class Renderer
{
public:
	Renderer();
	bool Initialize(HWND hwnd, Camera& camera, int width, int height,std::vector<Entity>& entities,std::vector<Light>& lights, std::vector<Light>& pointLights);
	void Render(Camera& camera, std::vector<Entity>& entity, PhysicsHandler& physicsHandler, std::vector<Light>& lights, std::vector<Light>& pointLights, std::vector<CollisionObject>& collisionObjects, GridClass& grid, std::vector<NavMeshClass>& navMeshes, std::vector<SoundComponent*>& sounds );
	void InitScene(std::vector<Entity>& entities, std::vector<Light>& lights, std::vector<Light>& pointLights, Camera& camera);

private:

	void RenderEntitiesAndLights(std::vector<Entity>& entity, std::vector<Light>& lights, std::vector<Light>& pointLights, Camera& camera);
	void RenderSceneToTexture(RenderTexture& texture, Camera& camera, std::vector<Entity>& entities, std::vector<Light>& lights, std::vector<Light>& pointLights, std::vector<CollisionObject>& collisionObjects);
	void UpdateBuffers(std::vector<Light>& lights, std::vector<Light>& pointLights, Camera& camera);
	void BrdfRender(Camera& camera, RenderTexture& texture);
	void IrradianceConvolutionRender(Camera& camera);
	//void PrifilterRender(Camera& camera, RenderTexture& texture);
	void PbrRender(Camera& camera);
	void RenderToEnvProbe(RenderTexture& texture, Camera& camera, std::vector<Entity>& entities, std::vector<Light>& lights, std::vector<Light>& pointLights);
	void BloomRender(Camera& camera);

	void RenderEntitiesPostProcess(std::vector<Entity>& entities, std::vector<Light>& lights, std::vector<Light>& pointLights, PixelShader& psShader, Camera& camera);
	void VolumeLightRender(std::vector<Entity>& entities, std::vector<Light>& lights, std::vector<Light>& pointLights, Camera& camera);
private:
	float rgb[4];

	InstancedShape object;
	RectShape rect,rectSmall,rectBloom;
	CubeShape debugCube;

	Shadows shadowsRenderer;

	GFXGui gfxGui;
	bool enablePostProccess = true;
	bool debugEnabled = false;


	PhysicsDebugDraw physicsDebugDraw;
	//TEST
	//CubeShape cube;
	int vSync;
private:
	imgui_addons::ImGuiFileBrowser file_dialog;

	bool hasTexture;
	bool isFileOpen;

public:
	bool bAddEntity;
	bool isAnimated;
	DX11 gfx11;
	std::string inName;
	bool runPhysics;
	bool bAddLight;
	bool bAddPointLight;
	bool bAddCollisionObject;
	bool bClear;
	bool bCreateGrid;
	bool bRenderNavMesh;
	bool bRenderNavMeshBounds;
	bool bConvertCordinates;
	bool bEnableShadows;
	bool bModelsLoaded;
	bool save;
	bool bGuiEnabled;
	int switchCameraMode;
	bool copyLight;
	bool copyPointLight;
	int selectedLight = -1;
	int selectedPointLight = -1;

	float renderDistance;
	float renderShadowDistance;
	float shadowDist;
	float acceptedDist;

	int listbox_item_current = -1;

	bool bEntityDeleted;
private:
	int windowWidth, windowHeight;
	EnvironmentProbe environmentProbe;

	RenderTexture cubeTexture;


	//float rgb[4];
	//PBR
	RenderTexture brdfTexture, prefilterCubeMap, irradianceCubeMap, IrradianceConvCubeTextures[6];
	DirectX::XMMATRIX ViewMatrix2D;

	Texture  envTextures[6], irradianceTextures[6], brdfText;

	//Bloom
	RenderTexture bloomRenderTexture;
	RenderTexture BloomVerticalBlurTexture, BloomHorizontalBlurTexture, downSampleTexture;

	float bloomBrightness = 0.65f;
	float bloomStrengh = 4.0f;
	float gamma = 1.0f;
	bool bRenderCollision;

	//Volumetric light
	RenderTexture volumetricLightTexture, cameraDepthTexture;

	AppTimer timer;
};

