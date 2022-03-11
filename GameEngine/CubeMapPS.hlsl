struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WOLRD_POSITION;
};


TextureCube objTexture : TEXTURE : register(t0);

SamplerState objSamplerState : SAMPLER : register(s0);
SamplerState objSamplerStateClamp : SAMPLER : register(s1);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 sampleColor = objTexture.Sample(objSamplerState, input.inWorldPos).rgb;

    sampleColor = sampleColor / (sampleColor + float3(1.0, 1.0f, 1.0f));
    sampleColor = pow(sampleColor, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));
    return float4(sampleColor, 1.0f);
}