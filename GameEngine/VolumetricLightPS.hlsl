#define NO_LIGHTS 4

cbuffer lightBuffer : register(b0)
{
    float4 dynamicLightPosition[NO_LIGHTS];
    float4 dynamicLightColor[NO_LIGHTS];
    float4 SpotlightDir[NO_LIGHTS];
    float4 cameraPos;
    float4 lightType[NO_LIGHTS];
}

cbuffer PCFbuffer : register(b1)
{
    int pcfLevel;
    double bias;
    bool enableShadows;
}

cbuffer lightMatrixBuffer : register(b5)
{
    float4x4 g_ShadowViewProjectionMatrix;
}

cbuffer timerBuffer : register(b5)
{
    float timer;
}
    struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WOLRD_POSITION;
    float3 inTangent : TANGENT;
    float3 inBinormal : BINORMAL;
    float4 ViewPosition : TEXCOORD1;
    float4 lightViewPosition[NO_LIGHTS] : LIGHTVIEWS;
};

static const float PI = 3.14159265359;

Texture2D cameraDepthTexture : TEXTURE : register(t0);
Texture2D depthMapTexture : TEXTURE : register(t1);
SamplerState SampleTypeWrap : register(s0);

float4 ComputeVolumetricLighting(float2 texCoord, float4 screenPosition);

float4 main(PS_INPUT input) : SV_TARGET
{


    float4 color = ComputeVolumetricLighting(input.inTexCoord, input.ViewPosition);
    
    return color;
}

float4 ComputeVolumetricLighting(float2 texCoord, float4 screenPosition)
{
    const int SAMPLES = 128;
        
    float intensity = 0.025;
    float decay = 0.98;

    float2 projectTexCoord;

    projectTexCoord = 0.5f * float2(screenPosition.x, -screenPosition.y) + 0.5f;
    //projectTexCoord.x = 640 / 2;
    //projectTexCoord.y = 360 / 2;
   // projectTexCoord.x = screenPosition.x / screenPosition.w;
   // projectTexCoord.y = -screenPosition.y / screenPosition.w;
   //
   // projectTexCoord.x = projectTexCoord.x * 0.5 + 0.5;
   // projectTexCoord.y = projectTexCoord.y * 0.5 + 0.5;


    float2 dir = (projectTexCoord / 1000) - texCoord;

    dir /= SAMPLES;

    float3 godRays = cameraDepthTexture.Sample(SampleTypeWrap, texCoord).xyz * intensity;
  
    for (int j = 0; j < SAMPLES; j++)
    {
        godRays += cameraDepthTexture.Sample(SampleTypeWrap, texCoord).xyz * intensity;
        intensity *= decay;
        texCoord += dir;
    }
    
    return float4(godRays.xyz, 1.0);

}