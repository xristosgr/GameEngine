#define NO_LIGHTS 24

cbuffer lightBuffer : register(b0)
{
    float4 dynamicLightPosition[NO_LIGHTS];
    float4 dynamicLightColor[NO_LIGHTS];
    float4 SpotlightDir[NO_LIGHTS];
    float4 cameraPos;
    float4 lightType[NO_LIGHTS];
    //float4 acceptedDistShadowAndLight;
    //float acceptedDist;
    uint lightsSize;
}



cbuffer lightCull : register(b2)
{
    float4 RadiusAndcutOff[NO_LIGHTS];
}

cbuffer screenEffectBuffer : register(b3)
{
    float gamma;
}


struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float4 ViewPosition : TEXCOORD1;
    float distToCamera : TEXCOORD2;
};

static const float PI = 3.14159265359;

Texture2D objTexture : TEXTURE : register(t0);
Texture2D normalTexture : TEXTURE : register(t1);
Texture2D roughnessMetalicTexture : TEXTURE : register(t2);
Texture2D worldPositionTexture : TEXTURE : register(t3);
TextureCube prefilterMap : TEXTURE : register(t5);
Texture2D brdfTexture : TEXTURE : register(t6);
TextureCube irradianceMap : TEXTURE : register(t7);
Texture2D depthTexture : TEXTURE : register(t8);
//Texture2D depthMapTextures[NO_LIGHTS] : TEXTURE : register(t7);

SamplerState SampleTypeWrap : register(s0);
SamplerState objSamplerStateMip : SAMPLER : register(s2);

float4 main(PS_INPUT input) : SV_TARGET
{
    
    float depth = depthTexture.Load(input.inPosition.xyz).z;
   
    float dist = input.inPosition.z / input.inPosition.w;
    if (dist < depth)
        discard;
    
    
    float4 albedo = float4(pow(objTexture.Sample(SampleTypeWrap, input.inTexCoord), gamma));
    

    return float4(albedo.rgb, 0.4);
   
}