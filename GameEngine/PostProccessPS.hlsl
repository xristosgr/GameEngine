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
Texture2D forwardRenderTexture : TEXTURE : register(t2);
Texture2D hbaoPlusTexture : TEXTURE : register(t3);

SamplerState objSamplerState : SAMPLER : register(s0);
SamplerState objSamplerStateClamp : SAMPLER : register(s1);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    float3 bloom = bloomTexture.Sample(objSamplerState, input.inTexCoord);
    float3 forwardColor = forwardRenderTexture.Sample(objSamplerState, input.inTexCoord);
    float3 hbaoPlus = hbaoPlusTexture.Sample(objSamplerState, input.inTexCoord) * 2;
    sampleColor *= hbaoPlus;
    sampleColor += bloom * bloomStrength;
    sampleColor += forwardColor;

    
    return float4(sampleColor, 1.0f);
}