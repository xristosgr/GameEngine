#define NO_LIGHTS 4

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

//cbuffer PCFbuffer : register(b1)
//{
//    int pcfLevel;
//    double bias;
//    bool enableShadows;
//}

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
    float4 albedo = objTexture.Sample(SampleTypeWrap, input.inTexCoord);
    float3 color = float3(0, 0, 0);
    for (int i = 0; i < NO_LIGHTS; ++i)
    {
        if (i > lightsSize - 1)
            break;
        
        float distance = length(dynamicLightPosition[i].xyz - input.inWorldPos);
        //if (distance < RadiusAndcutOff[i].x)
        //{
            float3 shadows = Shadows(input.lightViewPosition[i], depthMapTextures[i], input.distToCamera, input,i);
            float3 light = float3(albedo.r * 0.5f, albedo.g * 0.5f, albedo.b * 0.5f);
 
            color += shadows;
        //}
        //else
        //{
        //    color += float3(1, 1, 1);
        //
        //}
        
    }    
    return float4(color, 1.0);

}

float3 Shadows(float4 lightViewPosition, Texture2D depthMapTexture, float dist, PS_INPUT input, int index)
{
    //float zbias = bias;
    float2 projectTexCoord;
    float lightDepthValue;
    float lightIntensity;
    float shadow = 0.0f;
    float3 color = float3(0, 0, 0);
    int width;
    int height;
    depthMapTexture.GetDimensions(width, height);
    float2 texelSize;
    texelSize.x = 0.5 / width;
    texelSize.y = 0.5 / height;
    

    projectTexCoord.x = lightViewPosition.x / lightViewPosition.w/2.0f + 0.5f;
    projectTexCoord.y = -lightViewPosition.y / lightViewPosition.w / 2.0f + 0.5f;
   
    
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        lightDepthValue = lightViewPosition.z / lightViewPosition.w;
        
        //if (dist < acceptedDistShadowAndLight.x)
        //{
            //lightDepthValue = lightDepthValue - 0.00008f;
            lightDepthValue = lightDepthValue - 0.0005f;
        
            int PCF_RANGE = 1;
        
            [unroll(PCF_RANGE*2+1)]
            for (int x = -PCF_RANGE; x <= PCF_RANGE; x++)
            {
               [unroll(PCF_RANGE*2+1)]
                for (int y = -PCF_RANGE; y <= PCF_RANGE; y++)
                {
                    float pcfDepth = depthMapTexture.Sample(SampleTypeWrap, projectTexCoord + float2(x, y) * texelSize).r;
                
        
                    shadow += lightDepthValue > pcfDepth ? 0.0f : 1.0f;
                }
            }
            shadow /= ((PCF_RANGE * 2 + 1) * (PCF_RANGE * 2 + 1));
        //}
        //else
        //{
        //    lightDepthValue = lightDepthValue - bias;
        //
        //    float pcfDepth = depthMapTexture.Sample(SampleTypeWrap, projectTexCoord).r;
        //    shadow += lightDepthValue > pcfDepth ? 0.0f : 1.0f;
        //}
    }
    else
    {
        //shadow = 1.0f;
        //color = float3(1, 1, 1);
    }
    return float3(shadow, shadow, shadow);
}
