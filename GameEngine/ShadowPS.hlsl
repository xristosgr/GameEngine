#define NO_LIGHTS 24

cbuffer lightBuffer : register(b0)
{
    float4 dynamicLightPosition[NO_LIGHTS];
    float4 dynamicLightColor[NO_LIGHTS];
    float4 SpotlightDir[NO_LIGHTS];
    float4 cameraPos;
    float4 lightTypeEnableShadows[NO_LIGHTS];
    //float4 acceptedDistShadowAndLight;
    //float acceptedDist;
    uint lightsSize;
    
}

cbuffer shadowsbuffer : register(b9)
{
    float4 dynamicLightShadowStrength[NO_LIGHTS];
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
    float3 inNormal : NORMAL;
    float3 inWorldPos : WOLRD_POSITION;
    float3 inTangent : TANGENT;
    float3 inBinormal : BINORMAL;
    float4 ViewPosition : TEXCOORD1;
    float distToCamera : TEXCOORD2;
    float4 lightViewPosition[NO_LIGHTS] : LIGHTVIEWS;
};

Texture2D objTexture : TEXTURE : register(t0);
Texture2D depthMapTextures[NO_LIGHTS] : TEXTURE : register(t4);

SamplerState SampleTypeWrap : register(s0);
SamplerState SampleTypeClamp : register(s1);
SamplerState objSamplerStateMip : SAMPLER : register(s2);

float3 Shadows(float4 lightViewPosition, Texture2D depthMapTexture, float dist, PS_INPUT input,int index);

float4 main(PS_INPUT input) : SV_TARGET
{
    float alpha = objTexture.Sample(SampleTypeWrap, input.inTexCoord).a;
    if (alpha < 0.95)
        discard;
    
    float3 color = float3(0, 0,0);
    
    for (int i = 0; i < NO_LIGHTS; ++i)
    {
        if (i > lightsSize - 1)
            break;
       
         float3 shadows = Shadows(input.lightViewPosition[i], depthMapTextures[i], input.distToCamera, input,i);
        
        if (lightTypeEnableShadows[i].y)
            color += shadows;
    }
    return (float4(color, 1.0));

}

float3 Shadows(float4 lightViewPosition, Texture2D depthMapTexture, float dist, PS_INPUT input, int index)
{
    float lightIntensity = 1.0f / (lightsSize);
    float shadowIntensity = lightsSize;
    
    float shadow = 0.0f;
    float3 color = float3(0, 0, 0);
    int width;
    int height;
    depthMapTexture.GetDimensions(width, height);
    float2 texelSize;
    texelSize.x = 0.5 / width;
    texelSize.y = 0.5 / height;
    
    float3 projCoords;
   
    projCoords.x = lightViewPosition.x / lightViewPosition.w / 2.0f + 0.5f;
    projCoords.y = -lightViewPosition.y / lightViewPosition.w / 2.0f + 0.5f;
    projCoords.z = lightViewPosition.z / lightViewPosition.w;
    
    if (lightTypeEnableShadows[index].x == 2.0f)
        projCoords.z = projCoords.z - 0.00002f;
    else
        projCoords.z = projCoords.z - 0.00004f;
    
    if ((saturate(projCoords.x) == projCoords.x) && (saturate(projCoords.y) == projCoords.y))
    {
        int PCF_RANGE = 2;
        [unroll(PCF_RANGE*2+1)]
        for (int x = -PCF_RANGE; x <= PCF_RANGE; x++)
        {
        [unroll(PCF_RANGE*2+1)]
            for (int y = -PCF_RANGE; y <= PCF_RANGE; y++)
            {
                float pcfDepth = depthMapTexture.Sample(SampleTypeWrap, projCoords.xy + float2(x, y) * texelSize).r;
             
                shadow += projCoords.z > pcfDepth ? 0.0f : dynamicLightColor[index].w;
            }
        }
        shadow /= ((PCF_RANGE * 2 + 1) * (PCF_RANGE * 2 + 1));
    }
    else
    {
        shadow = dynamicLightColor[index].w;
    }
    return (float3(shadow, shadow, shadow));
}
