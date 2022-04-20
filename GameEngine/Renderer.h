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
#include "PostProcessClass.h"
#include "PbrClass.h"
#include "GBufferClass.h"
#include "Sky.h"

class Renderer
{
public:
	Renderer();
	bool Initialize(HWND hwnd, Camera& camera, int width, int height,std::vector<Entity>& entities,std::vector<Light>& lights, std::vector<Light>& pointLights);
	void Render(Camera& camera, std::vector<Entity>& entity, PhysicsHandler& physicsHandler, std::vector<Light>& lights, std::vector<Light>& pointLights, std::vector<CollisionObject>& collisionObjects, GridClass& grid, std::vector<NavMeshClass>& navMeshes, std::vector<SoundComponent*>& sounds, Sky& sky);
	void InitScene(std::vector<Entity>& entities, std::vector<Light>& lights, std::vector<Light>& pointLights, Camera& camera,Sky& sky);

private:
	void RenderDeferred(std::vector<Entity>& entities, std::vector<Light>& lights, std::vector<Light>& pointLights, Camera& camera, Sky& sky);
	void UpdateBuffers(std::vector<Light>& lights, std::vector<Light>& pointLights, Camera& camera);
	void RenderToEnvProbe(EnvironmentProbe& probe, Camera& camera, std::vector<Entity>& entities, std::vector<Light>& lights, std::vector<Light>& pointLights, Sky& sky);
	void ForwardPass(std::vector<Entity>& entities, Camera& camera, Sky& sky);

private:
	float rgb[4];

	GBufferClass gBuffer;
	PbrClass pbr;
	PostProcessClass postProcess;
	InstancedShape object;
	RectShape rect,rectSmall;
	CubeShape debugCube;

	Shadows shadowsRenderer;

	GFXGui gfxGui;
	bool enablePostProccess = true;
	bool debugEnabled = false;


	PhysicsDebugDraw physicsDebugDraw;

	int vSync;
private:
	imgui_addons::ImGuiFileBrowser file_dialog;


	bool isFileOpen;

public:
	DX11 gfx11;

	bool bAddEntity;
	bool isAnimated;
	bool hasTexture;

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


	DirectX::XMFLOAT3 skyColor;


	bool bHasFinishedLoading = false;
private:
	int windowWidth, windowHeight;
	EnvironmentProbe environmentProbe;

	RenderTexture cubeTexture;

	//PBR
	//RenderTexture brdfTexture, prefilterCubeMap, irradianceCubeMap;
	DirectX::XMMATRIX ViewMatrix2D;

	Texture brdfText;

	float bloomBrightness = 0.65f;
	float bloomStrengh = 4.0f;
	float gamma;
	bool bRenderCollision;

	RenderTexture forwardRenderTexture;

	AppTimer timer;


	Texture defaultText[3];

};

