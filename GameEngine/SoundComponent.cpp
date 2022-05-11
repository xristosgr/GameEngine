#include "SoundComponent.h"

SoundComponent::SoundComponent()
{
}

bool SoundComponent::Initialize(const char* filePath, bool is3D, ID3D11Device* device)
{
    cube.Initialize(device);
    cube.pos = DirectX::XMFLOAT3(0, 2, 0);
    b3D = is3D;
    system = NULL;
    FMOD_RESULT result;
    result = FMOD::Studio::System::create(&system);
    if (result != FMOD_OK)
    {
        return false;
    }
    result = system->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
    if (result != FMOD_OK)
    {
        return false;
    }
    result = FMOD::System_Create(&mpSystem);
    if (result != FMOD_OK)
    {
        return false;
    }
    result = mpSystem->init(512, FMOD_INIT_NORMAL, 0);
    if (result != FMOD_OK)
    {
        return false;
    }
    if (is3D)
        result = mpSystem->createSound(filePath, FMOD_3D, nullptr, &sound);
    else
        result = mpSystem->createSound(filePath, FMOD_DEFAULT, nullptr, &sound);

    if (result != FMOD_OK)
    {
        return false;
    }


    return true;
}

bool SoundComponent::Play()
{
    FMOD_RESULT result;
    result = mpSystem->playSound(sound, nullptr, false, &channel);
    if (result != FMOD_OK)
    {
        return false;
    }
    return true;
}

void SoundComponent::Update()
{
    system->update();
    mpSystem->update();
}

void SoundComponent::UpdatePos(const DirectX::XMFLOAT3& destPos, const DirectX::XMVECTOR& forwardVec, const DirectX::XMVECTOR& upVec)
{
    DirectX::XMFLOAT3 forwardFloat3;
    DirectX::XMStoreFloat3(&forwardFloat3, forwardVec);

    DirectX::XMFLOAT3 upFloat3;
    DirectX::XMStoreFloat3(&upFloat3, upVec);

    this->position = FMOD_VECTOR{ cube.pos.x,cube.pos.y,cube.pos.z };
    channel->set3DAttributes(&this->position, nullptr);
    channel->set3DMinMaxDistance(4.0f, 10000.0f);
    FMOD_VECTOR pos{ destPos.x,destPos.y,destPos.z };
    FMOD_VECTOR forward = { forwardFloat3.x,forwardFloat3.y,forwardFloat3.z };
    FMOD_VECTOR up = { upFloat3.x, upFloat3.y, upFloat3.z };

    mpSystem->set3DListenerAttributes(0, &pos, nullptr, &forward, &up);
}

void SoundComponent::Draw(ID3D11DeviceContext* deviceContext, Camera& camera, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexShader)
{
    cube.scale = DirectX::XMFLOAT3(0.2f, 0.2f, 0.2f);
    cube.Draw(deviceContext, camera, cb_vs_vertexShader);
}

