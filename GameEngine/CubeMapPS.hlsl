struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inWorldPos : WOLRD_POSITION;
};


TextureCube objTexture : TEXTURE : register(t0);

Texture2D depthTexture : TEXTURE : register(t1);

SamplerState objSamplerState : SAMPLER : register(s0);
SamplerState objSamplerStateClamp : SAMPLER : register(s1);
SamplerState objSamplerStateMip : SAMPLER : register(s2);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 sampleColor = objTexture.Sample(objSamplerStateMip, input.inWorldPos).rgb;
    float depth = depthTexture.Load(input.inPosition.xyz).z;
    
    float dist = input.inPosition.z / input.inPosition.w;
    //float3 depth = depthTexture.SampleCmp(objSamplerState, input.inTexCoord.xy, input.inPosition.z);
    if (dist < depth)
        discard;
    
  
    sampleColor = sampleColor / (sampleColor + float3(1.0, 1.0f, 1.0f));
    sampleColor = pow(sampleColor, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));
    return float4(sampleColor, 1.0f);
}