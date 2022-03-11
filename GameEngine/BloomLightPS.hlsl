cbuffer screenEffectBuffer : register(b3)
{
    float gamma;
    float bloomBrightness;
   // unsigned int shadowLightSize;
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
   // float3 inNormal : NORMAL;
   // float3 inWorldPos : WOLRD_POSITION;
};

Texture2D objTexture : TEXTURE : register(t0);

SamplerState objSamplerState : SAMPLER : register(s0);
SamplerState objSamplerStateClamp : SAMPLER : register(s1);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);

    float brightness = dot(sampleColor, float3(0.2126, 0.7152, 0.0722));

    if (brightness > bloomBrightness)
    {
        //return float4(sampleColor.xyz, 1.0f);
        return float4(sampleColor.x, sampleColor.y, sampleColor.z, 1.0f);
    }
    else
    {
        return float4(0, 0, 0, 1.0f);
    }
}