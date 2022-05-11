#pragma once
#include <fmod/fmod_studio.hpp>
#include <fmod/fmod.hpp>
#include <DirectXMath.h>
#include "CubeShape.h"

class SoundComponent
{
public:
	SoundComponent();
	bool Initialize(const char* filePath, bool is3D, ID3D11Device* device);
	bool Play();
	void Update();
	void UpdatePos(const DirectX::XMFLOAT3& destPos, const DirectX::XMVECTOR& forwardVec, const DirectX::XMVECTOR& upVec);
	void Draw(ID3D11DeviceContext* deviceContext, Camera& camera, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexShader);

	FMOD_VECTOR position;
	FMOD::Studio::System* system;
	FMOD::System* mpSystem;
	int mnNextChannelId;
	FMOD::Sound* sound = nullptr;
	FMOD::Channel* channel = nullptr;

	CubeShape cube;
private:
	bool b3D = false;
};

