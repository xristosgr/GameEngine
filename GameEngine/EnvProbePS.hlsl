
cbuffer screenEffectBuffer : register(b3)
{
    float gamma;
}


struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
};

Texture2D objTexture : TEXTURE : register(t0);

SamplerState SampleTypeWrap : register(s0);
SamplerState SampleTypeMip : register(s2);


float4 main(PS_INPUT input) : SV_TARGET
{
    float4 albedo = float4(pow(objTexture.Sample(SampleTypeMip, input.inTexCoord).xyz, 1.0f), gamma);
    float3 color = albedo.rgb;
    color.rgb = color.rgb / (color.rgb + float3(1.0, 1.0f, 1.0f));
    color.rgb = pow(color.rgb, float3(1.0f / 1.0f, 1.0f / 1.0f, 1.0f / 1.0f));
    return float4(color, 1.0);
}
