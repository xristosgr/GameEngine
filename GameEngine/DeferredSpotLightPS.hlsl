cbuffer lightBuffer : register(b1)
{
    float4 dynamicLightPosition;
    float4 dynamicLightColor;
    float4 SpotlightDir;
    float4 cameraPos;
    float4 lightType;
}

cbuffer PCFbuffer : register(b1)
{
    int pcfLevel;
    double bias;
    bool enableShadows;
}

cbuffer lightCull : register(b2)
{
    float4 RadiusAndcutOff;
}

cbuffer screenEffectBuffer : register(b3)
{
    float gamma;
}


cbuffer pointLightBuffer : register(b6)
{
    float4 pointdynamicLightPosition;
    float4 pointdynamicLightColor;
}

cbuffer pointLightCull : register(b7)
{
    float4 pointRadiusAndcutOff;
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
Texture2D distToCameraTexture : TEXTURE : register(t4);
TextureCube prefilterMap : TEXTURE : register(t5);
Texture2D brdfTexture : TEXTURE : register(t6);
TextureCube irradianceMap : TEXTURE : register(t7);
Texture2D shadowTexture : TEXTURE : register(t8);

//Texture2D depthMapTextures[NO_LIGHTS] : TEXTURE : register(t7);

SamplerState SampleTypeWrap : register(s0);
SamplerState objSamplerStateMip : SAMPLER : register(s2);

float3 fresnelSchlick(float cosTheta, float3 F0);
float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness);
float DistributionGGX(float3 N, float3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(float3 N, float3 V, float3 L, float roughness);
float3 pointLight(PS_INPUT input, float3 albedo, float3 pos, float3 color, float4 _cutOff, float3 bumpNormal, float roughness, float metallic, float3 V, float3 F0, float3 worldPos);
float3 spotLight(PS_INPUT input, float3 albedo, float3 bumpNormal, float roughness, float metallic, float3 V, float3 F0, float3 worldPos);

float4 main(PS_INPUT input) : SV_TARGET
{
    int3 sampleIndices = int3(input.inPosition.xy, 0);
    
    float3 albedo = float3(pow(objTexture.Load(sampleIndices).rgb, gamma));
    
    float3 bumpNormal = normalTexture.Load(sampleIndices).rgb;
    if (bumpNormal.r == -1 && bumpNormal.g == -1 && bumpNormal.b == -1)
    {
        return float4(albedo.r, albedo.g, albedo.b, 1.0f);
    }
    float metallic = roughnessMetalicTexture.Load(sampleIndices).b;
    float roughness = roughnessMetalicTexture.Load(sampleIndices).g;
    float3 worldPos = worldPositionTexture.Load(sampleIndices).xyz;
    //float3 worldPos = worldPositionTexture.Sample(SampleTypeWrap, input.inTexCoord).xyz;

    
    float3 V = normalize(cameraPos.xyz - worldPos);

    float3 ambient = float3(0.1, 0.1, 0.1);
    float3 F0 = float3(0.04f, 0.04f, 0.04f);

    F0 = lerp(F0, albedo.rgb, metallic);
    float3 Lo = float3(0.0f, 0.0f, 0.0f);
   
    Lo = spotLight(input, albedo.rgb, bumpNormal, roughness, metallic, V, F0, worldPos);
    //float3 shadows = shadowTexture.Load(sampleIndices).xyz;
    //Lo *= shadows;
 
    float3 F = fresnelSchlickRoughness(max(dot(bumpNormal, V), 0.0f), F0, roughness);
    float3 kS = F;
    float3 kD = 1.0f - kS;
    float3 irradiance = irradianceMap.Sample(objSamplerStateMip, bumpNormal.rgb).rgb;
    float3 diffuse = irradiance * albedo.rgb;
    float3 R = reflect(-V, bumpNormal);

    const float MAX_REF_LOD = 5.0f;
    float3 prefilteredColor = prefilterMap.SampleLevel(objSamplerStateMip, R, roughness * MAX_REF_LOD).rgb;
    float2 brdf = brdfTexture.Sample(SampleTypeWrap, float2(max(dot(bumpNormal, V), 0.0), roughness)).rg;

    float3 specular = prefilteredColor * (F * brdf.x + brdf.y);
    ambient = (kD * diffuse + specular);
    float3 color = ambient + Lo;
    
    color = color / (color + float3(1.0, 1.0f, 1.0f));
    color = pow(color, float3(1.0f / 1.0f, 1.0f / 1.0f, 1.0f / 1.0f));

    return float4(color, 1.0);
   
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

float3 pointLight(PS_INPUT input, float3 albedo, float3 pos, float3 color, float4 _cutOff, float3 bumpNormal, float roughness, float metallic, float3 V, float3 F0, float3 worldPos)
{
    float3 L = normalize(pos.xyz - worldPos.xyz).xyz;
    
    //float theta = dot(L, normalize(-lightDirectionAndSpecularPower[index].xyz));
    float outerCutOff = _cutOff.x / 3.0f;
    float epsilon = _cutOff.x - outerCutOff;
    float intensity = clamp((L - outerCutOff) / epsilon, 0.0f, 1.0f);
    
    float3 H = normalize(V + L);
        
    float distance = length(pos.xyz - worldPos.xyz);
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

float3 spotLight(PS_INPUT input, float3 albedo, float3 bumpNormal, float roughness, float metallic, float3 V, float3 F0, float3 worldPos)
{
    float3 L = normalize(dynamicLightPosition.xyz - worldPos.xyz).xyz;
    float theta = dot(L, normalize(-SpotlightDir.xyz));
    float outerCutOff = RadiusAndcutOff.y / 3.0f;
    float epsilon = RadiusAndcutOff.y - outerCutOff;
    float intensity = clamp((theta - outerCutOff) / epsilon, 0.0f, 1.0f);
    
    float3 H = normalize(V + L);
        
    float distance = length(dynamicLightPosition.xyz - worldPos.xyz);
    float attenuation = 1.0f / (distance * distance) * intensity;
    float3 radiance = dynamicLightColor.xyz * attenuation;

        
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