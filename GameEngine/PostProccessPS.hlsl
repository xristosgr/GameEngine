struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
};
cbuffer screenEffectBuffer : register(b3)
{
    float gamma;
    float bloomBrightness;
    float bloomStrength;
}

Texture2D objTexture : TEXTURE : register(t0);
Texture2D bloomTexture : TEXTURE : register(t1);
Texture2D shadowTexture : TEXTURE : register(t2);

SamplerState objSamplerState : SAMPLER : register(s0);
SamplerState objSamplerStateClamp : SAMPLER : register(s1);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    float3 bloom = bloomTexture.Sample(objSamplerState, input.inTexCoord);
    float3 shadows = shadowTexture.Sample(objSamplerState, input.inTexCoord);

    //sampleColor *= shadows;
  
    sampleColor += bloom * bloomStrength;
    //sampleColor += volumetricLight ;
    //sampleColor += lights;
    
    return float4(sampleColor.xyz, 1.0f);
}