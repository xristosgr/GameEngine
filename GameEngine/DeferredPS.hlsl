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







cbuffer materialBuffer : register(b5)
{
    float3 color;
    float bEmissive;
}



struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WOLRD_POSITION;
    float3 inTangent : TANGENT;
    float3 inBinormal : BINORMAL;
    float4 lightViewPosition[NO_LIGHTS] : LIGHTVIEWS;
};

struct PS_OUTPUT
{
    float4 albedo : SV_Target0;
    float4 normal : SV_Target1;
    float4 roughnessMetalic : SV_Target2;
    float4 worldPosition : SV_Target3;
    float4 depth : SV_TARGET4;
    float4 shadows : SV_TARGET5;
};

Texture2D albedoTexture : TEXTURE : register(t0);
Texture2D normalTexture : TEXTURE : register(t1);
Texture2D roughnessMetalicTexture : TEXTURE : register(t2);
Texture2D depthMapTextures[NO_LIGHTS] : TEXTURE : register(t3);

SamplerState SampleTypeWrap : register(s0);

float3 Shadows(float4 lightViewPosition, Texture2D depthMapTexture, PS_INPUT input, int index);

PS_OUTPUT main(PS_INPUT input) : SV_TARGET
{
    PS_OUTPUT output;
    
   
  
    
    if (bEmissive == 0.0f)
    {
        output.albedo = albedoTexture.Sample(SampleTypeWrap, input.inTexCoord);
        if (output.albedo.a < 0.95)
        {
            discard;
        }
       
        
       output.normal = normalTexture.Sample(SampleTypeWrap, input.inTexCoord);

       output.normal = (output.normal * 2.0f) - 1.0f;
    
       float3 bumpNormal = (output.normal.x * input.inTangent) + (output.normal.y * input.inBinormal) + (output.normal.z * input.inNormal);
       bumpNormal = normalize(bumpNormal);
       output.normal = float4(bumpNormal, 1.0f);
    
       output.roughnessMetalic = roughnessMetalicTexture.Sample(SampleTypeWrap, input.inTexCoord);
    }
    else if(bEmissive == 1.0f)
    {
        output.albedo = float4(color.r, color.g, color.b, 1.0f);
        output.normal = float4(-1, -1, -1, 1.0f);
        output.roughnessMetalic = float4(-1, -1, -1, 1.0f);
    }
    output.worldPosition = float4(input.inWorldPos, 1.0f);
    
    float depthValue = input.inPosition.z / input.inPosition.w;
    output.depth = float4(depthValue, depthValue, depthValue,1.0f);
    
    output.shadows = float4(0, 0, 0, 1.0f);
    for (int i = 0; i < NO_LIGHTS; ++i)
    {
        if (i > lightsSize - 1)
            break;
       
        float3 shadows = Shadows(input.lightViewPosition[i], depthMapTextures[i], input, i);
        
        if (lightTypeEnableShadows[i].y)
            output.shadows.rgb += shadows;
    }
    return output;
}


float3 Shadows(float4 lightViewPosition, Texture2D depthMapTexture, PS_INPUT input, int index)
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
