#define NO_LIGHTS 25
#define NO_POINT_LIGHTS 100

cbuffer lightBuffer : register(b0)
{
    float4 dynamicLightPosition[NO_LIGHTS];
    float4 dynamicLightColor[NO_LIGHTS];
    float4 SpotlightDir[NO_LIGHTS];
    float4 cameraPos;
    float4 lightType[NO_LIGHTS];
    float acceptedDistShadow;
    float acceptedDist;
    uint lightsSize;
}

cbuffer PCFbuffer : register(b1)
{
    int pcfLevel;
    double bias;
    bool enableShadows;
}

cbuffer lightCull : register(b2)
{
    float4 Radius[NO_LIGHTS];
    float4 cutOff[NO_LIGHTS];
}

cbuffer screenEffectBuffer : register(b3)
{
    float gamma;
}



cbuffer pointLightBuffer : register(b6)
{
    float4 pointdynamicLightPosition[NO_POINT_LIGHTS];
    float4 pointdynamicLightColor[NO_POINT_LIGHTS];
    uint pointLightsSize;
}
cbuffer pointLightCull : register(b7)
{
    float4 pointRadius[NO_POINT_LIGHTS];
    float4 pointcutOff[NO_POINT_LIGHTS];
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

static const float PI = 3.14159265359;

Texture2D objTexture : TEXTURE : register(t0);
Texture2D normalTexture : TEXTURE : register(t1);
Texture2D roughnessMetalicTexture : TEXTURE : register(t2);
TextureCube prefilterMap : TEXTURE : register(t3);
Texture2D brdfTexture : TEXTURE : register(t4);
TextureCube irradianceMap : TEXTURE : register(t5);
Texture2D depthMapTextures[NO_LIGHTS] : TEXTURE : register(t6);

SamplerState SampleTypeWrap : register(s0);


float Shadows(float4 lightViewPosition, Texture2D depthMapTexture,float dist);

float3 fresnelSchlick(float cosTheta, float3 F0);
float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness);
float DistributionGGX(float3 N, float3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(float3 N, float3 V, float3 L, float roughness);
float Shadows(float4 viewPos);
float3 pointLight(PS_INPUT input, float3 albedo, float3 pos,float3 color, float4 _cutOff, float3 bumpNormal, float roughness, float metallic, float3 V, float3 F0);
float3 spotLight(PS_INPUT input, float3 albedo, float3 bumpNormal, float roughness, float metallic, float3 V, float3 F0, int index);

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 albedo = float4(pow(objTexture.Sample(SampleTypeWrap, input.inTexCoord), gamma));

    if(albedo.a < 0.95)
        discard;
    float3 normalColor = normalTexture.Sample(SampleTypeWrap, input.inTexCoord).xyz;
    float metallic = roughnessMetalicTexture.Sample(SampleTypeWrap, input.inTexCoord).b;
    float roughness = roughnessMetalicTexture.Sample(SampleTypeWrap, input.inTexCoord).g;

    normalColor = (normalColor * 2.0f) - 1.0f;
    float3 bumpNormal = (normalColor.x * input.inTangent) + (normalColor.y * input.inBinormal) + (normalColor.z * input.inNormal);
    bumpNormal = normalize(bumpNormal);
    //bumpNormal = input.inNormal;
    float3 V = normalize(cameraPos.xyz - input.inWorldPos);

    float3 ambient = float3(0.1, 0.1, 0.1);
    float3 F0 = float3(0.04f, 0.04f, 0.04f);

    F0 = lerp(F0, albedo.rgb, metallic);
    float3 Lo = float3(0.0f, 0.0f, 0.0f);

    [unroll(NO_LIGHTS)]
    for (int i = 0; i < NO_LIGHTS;++i)
    {
        if (i > lightsSize-1)
            break;

        if (input.distToCamera < acceptedDist)
        {
            float distance = length(dynamicLightPosition[i].xyz - input.inWorldPos);
            if (distance < Radius[i].x)
            {
                if (lightType[i].x == 0.0)
                    Lo += pointLight(input, albedo.rgb, dynamicLightPosition[i].xyz, dynamicLightColor[i].rgb, cutOff[i], bumpNormal, roughness, metallic, V, F0); //* Shadows(input.lightViewPosition[i], depthMapTextures[i]);
                else if (lightType[i].x == 1.0)
                {
                    if (input.distToCamera < acceptedDistShadow * 2.0f)
                        Lo += spotLight(input, albedo.rgb, bumpNormal, roughness, metallic, V, F0, i) * Shadows(input.lightViewPosition[i], depthMapTextures[i], input.distToCamera);
                    else
                        Lo += spotLight(input, albedo.rgb, bumpNormal, roughness, metallic, V, F0, i);
                }

            }
        }
       
    }

    [unroll(NO_POINT_LIGHTS)]
    for (int j = 0; j < NO_POINT_LIGHTS; ++j)
    {
        if (j > pointLightsSize - 1)
            break;

        if (input.distToCamera < acceptedDist)
        {
            float distance = length(pointdynamicLightPosition[j].xyz - input.inWorldPos);
            if (distance < pointRadius[j].x)
            {
                Lo += pointLight(input, albedo.rgb, pointdynamicLightPosition[j].xyz, pointdynamicLightColor[j].rgb, pointcutOff[j], bumpNormal, roughness, metallic, V, F0);
            }
        }
        
    }
   

    float3 F = fresnelSchlickRoughness(max(dot(bumpNormal, V), 0.0f), F0, roughness);
    float3 kS = F;
    float3 kD = 1.0f - kS;
    float3 irradiance = irradianceMap.Sample(SampleTypeWrap, bumpNormal.rgb).rgb;
    float3 diffuse = irradiance * albedo.rgb;
    float3 R = reflect(-V, bumpNormal);
    float3 prefilteredColor = prefilterMap.Sample(SampleTypeWrap, R);
  
    float2 brdf = brdfTexture.Sample(SampleTypeWrap, float2(max(dot(bumpNormal, V), 0.0), roughness)).rg;

    float3 specular = prefilteredColor * (F * brdf.x + brdf.y);
    ambient = (kD * diffuse + specular);
    float3 color = ambient + Lo;

    float2 projectTexCoord;
    projectTexCoord.x = input.ViewPosition.x / input.ViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -input.ViewPosition.y / input.ViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.x = projectTexCoord.x * 0.5 + 0.5;
    projectTexCoord.y = projectTexCoord.y * 0.5 + 0.5;

    color = color / (color + float3(1.0, 1.0f, 1.0f));
    color = pow(color, float3(1.0f / 1.0f, 1.0f / 1.0f, 1.0f / 1.0f));

    return float4(color, 1.0);
}

float Shadows(float4 lightViewPosition, Texture2D depthMapTexture, float dist)
{
    //float zbias = bias;
    float2 projectTexCoord;
    float lightDepthValue;
    float shadow = 0.0f;
    
    int width;
    int height;
    depthMapTexture.GetDimensions(width, height);
    float2 texelSize;
    texelSize.x = 0.5 / width;
    texelSize.y = 0.5 / height;
    
    projectTexCoord.x = lightViewPosition.x / lightViewPosition.w;
    projectTexCoord.y = -lightViewPosition.y / lightViewPosition.w;
   
    projectTexCoord.x = projectTexCoord.x * 0.5 + 0.5;
    projectTexCoord.y = projectTexCoord.y * 0.5 + 0.5;
    


    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        lightDepthValue = lightViewPosition.z / lightViewPosition.w;
        
       if (dist < acceptedDistShadow)
       {
            lightDepthValue = lightDepthValue - bias;
        
        
            int PCF_RANGE = 4;

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
        }
        else
        {
            lightDepthValue = lightDepthValue - bias;
        
            float pcfDepth = depthMapTexture.Sample(SampleTypeWrap, projectTexCoord).r;
            shadow += lightDepthValue > pcfDepth ? 0.0f : 1.0f;
        }
     
    
      
       //lightDepthValue = lightDepthValue - bias;
       // 
       // 
       //int PCF_RANGE = _range;
       //
       //[unroll(PCF_RANGE*2+1)]
       // for (int x = -PCF_RANGE; x <= PCF_RANGE; x++)
       // {
       //     [unroll(PCF_RANGE*2+1)]
       //     for (int y = -PCF_RANGE; y <= PCF_RANGE; y++)
       //     {
       //         float pcfDepth = depthMapTexture.Sample(SampleTypeWrap, projectTexCoord + float2(x, y) * texelSize).r;
       //         
       //         shadow += lightDepthValue > pcfDepth ? 0.0f : 1.0f;
       //     }
       // }
       // shadow /= ((PCF_RANGE * 2 + 1) * (PCF_RANGE * 2 + 1));
     
    }
   // else
   // {
   //     return shadow = 1.0f;
   // }
  
    return shadow;
}


float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0f - F0) * pow(clamp(1.0f - cosTheta, 0.0, 1.0), 5.0f);
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;
    
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    
    denom = PI * denom * denom;
    return nom / denom;
    //return nom / max(denom, 0.001f);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;
    
    float nom = NdotV;
    float denom = NdotV * (1.0f - k) + k;
    
    return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

float3 pointLight(PS_INPUT input, float3 albedo, float3 pos, float3 color, float4 _cutOff, float3 bumpNormal, float roughness, float metallic, float3 V, float3 F0)
{
    float3 L = normalize(pos.xyz - input.inWorldPos.xyz).xyz;
    
    //float theta = dot(L, normalize(-lightDirectionAndSpecularPower[index].xyz));
    float outerCutOff = _cutOff.x / 3.0f;
    float epsilon = _cutOff.x - outerCutOff;
    float intensity = clamp((L - outerCutOff) / epsilon, 0.0f, 1.0f);
    
    float3 H = normalize(V + L);
        
    float distance = length(pos.xyz - input.inWorldPos.xyz);
    //float attenuation = 1.0f / (distance * distance);
    float attenuation = 1.0f / (distance * distance) * epsilon;
    float3 radiance = color.xyz * attenuation;

        
    float NDF = DistributionGGX(bumpNormal, H, roughness);
    float G = GeometrySmith(bumpNormal, V, L, roughness);
    float3 F = fresnelSchlick(max(dot(H, V), 0.0f), F0);
    //float3 F = fresnelSchlickRoughness(max(dot(bumpNormal, V), 0.0f), F0, roughness);

        
    float3 nominator = NDF * G * F;
    float denominator = 4 * max(dot(bumpNormal, V), 0.0f) * max(dot(bumpNormal, L), 0.0f) + 0.001;
    float3 specular = (nominator / denominator);
        
    float3 kS = F;
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
    kD *= 1.0f - metallic;
        
    float NdotL = max(dot(bumpNormal, L), 0.0f);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

float3 spotLight(PS_INPUT input, float3 albedo, float3 bumpNormal, float roughness, float metallic, float3 V, float3 F0, int index)
{
    float3 L = normalize(dynamicLightPosition[index].xyz - input.inWorldPos.xyz).xyz;
    float theta = dot(L, normalize(-SpotlightDir[index].xyz));
    float outerCutOff = cutOff[index].x / 3.0f;
    float epsilon = cutOff[index].x - outerCutOff;
    float intensity = clamp((theta - outerCutOff) / epsilon, 0.0f, 1.0f);
    
    float3 H = normalize(V + L);
        
    float distance = length(dynamicLightPosition[index].xyz - input.inWorldPos.xyz);
    float attenuation = 1.0f / (distance * distance) * intensity;
    float3 radiance = dynamicLightColor[index].xyz * attenuation;

        
    float NDF = DistributionGGX(bumpNormal, H, roughness);
    float G = GeometrySmith(bumpNormal, V, L, roughness);
    float3 F = fresnelSchlick(max(dot(H, V), 0.0f), F0);
    //float3 F = fresnelSchlickRoughness(max(dot(bumpNormal, V), 0.0f), F0, roughness);

        
    float3 nominator = NDF * G * F;
    float denominator = 4 * max(dot(bumpNormal, V), 0.0f) * max(dot(bumpNormal, L), 0.0f) + 0.001;
    float3 specular = (nominator / denominator);
        
    float3 kS = F;
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
    kD *= 1.0f - metallic;
        
    float NdotL = max(dot(bumpNormal, L), 0.0f);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(float3(1.0f - roughness, 1.0f - roughness, 1.0f - roughness), F0) - F0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
    //return F0 + (max(float3(1.0f - roughness, 1.0f - roughness, 1.0f - roughness), F0) - F0) * pow(1.0f - cosTheta, 5.0f);
}