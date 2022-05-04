cbuffer lightBuffer : register(b5)
{
    float3 color;
    float bEmissive;
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inWorldPos : WOLRD_POSITION;
};

Texture2D depthTexture : TEXTURE : register(t0);
Texture2D bloomTexture : TEXTURE : register(t1);

SamplerState objSamplerState : SAMPLER : register(s0);
float4 main(PS_INPUT input) : SV_TARGET
{
    float depth = depthTexture.Load(input.inPosition.xyz).z;
    //float3 bloom = bloomTexture.Load(input.inPosition.xyz);
    //float3 depth = depthTexture.SampleCmp(objSamplerState, input.inTexCoord.xy, input.inPosition.z);
    
    if (bEmissive == 1.0f)
    {
        float dist = input.inPosition.z / input.inPosition.w;
        if (dist < depth)
            discard;
    }
    float3 _color = color / (color + float3(1.0, 1.0f, 1.0f));
    _color = pow(_color, float3(1.0f / 1.0f, 1.0f / 1.0f, 1.0f / 1.0f));
    return float4(_color, 1.0);
}