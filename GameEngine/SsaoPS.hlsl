cbuffer SSAObuffer : register(b1)
{
    float4x4 projection;
    float4x4 view;
    float4 ssaoKernel[48];
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float4x4 projection : TEXCOORD1;
};

Texture2D normalTexture : TEXTURE : register(t0);
Texture2D positionTexture : TEXTURE : register(t1);
Texture2D noiseTexture : TEXTURE : register(t2);

SamplerState objSamplerState : register(s0);
SamplerState objSamplerStateClamp : register(s1);
SamplerState objSamplerStatePoint : register(s3);
const float2 scale = float2(1600.0 / 8.0, 900.0 / 8.0);
float4 main(PS_INPUT input) : SV_TARGET
{
    
    float radius = 0.5;
    float bias = 0.025;
    
    float3 normal = normalTexture.Sample(objSamplerState, input.inTexCoord).xyz;
    
    //if (normal.r == -1 && normal.g == -1 && normal.b == -1)
    //    discard;
    
    float3 position = positionTexture.Sample(objSamplerState, input.inTexCoord).xyz;
    
    float3 noise = normalize(noiseTexture.Sample(objSamplerStatePoint, input.inTexCoord).xyz);
    float3 tangent = normalize(noise - normal * dot(noise, normal));
    float3 bitangent = cross(normal, tangent);
    float3x3 TBN = float3x3(tangent, bitangent, normal);
    
    
    float occlusion = 0.0;
    for (int i = 0; i < 48; ++i)
    {
        float3 samplePos = mul(ssaoKernel[i].xyz, TBN); // from tangent to view-space
        samplePos = position + samplePos * radius;
        
        float4 offset = float4(samplePos, 1.0);
        offset = mul(offset, projection); // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0  
        
        float sampleDepth = positionTexture.Sample(objSamplerState, offset.xy).z;
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(position.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / 64);
    return float4(occlusion, occlusion, occlusion, 1.0);
}