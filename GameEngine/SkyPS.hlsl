

cbuffer lightBuffer : register(b5)
{
    float3 color;
    float bEmissive;
}
cbuffer skyBuffer : register(b8)
{
    float4 apexColor;
    float4 centerColor;

}
struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inWorldPos : WOLRD_POSITION;
};

Texture2D depthTexture : TEXTURE : register(t1);
SamplerState objSamplerState : SAMPLER : register(s0);
float4 main(PS_INPUT input) : SV_TARGET
{
    float depth = depthTexture.Load(input.inPosition.xyz).z;

    if (bEmissive == 1.0f)
    {
        float dist = input.inPosition.z / input.inPosition.w;
        if (dist < depth)
            discard;
    }
    float height = input.inWorldPos.y;
    float4 OutPutColor = lerp(centerColor, apexColor, height);
    
    return float4(OutPutColor.rgb, 1.0);
}