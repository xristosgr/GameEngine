

cbuffer screenEffectBuffer : register(b3)
{
    float gamma;
}

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
    
    if(height < 0.0)
        height = 0.0f;
    float4 OutPutColor = lerp(centerColor, apexColor, height);
   
    //OutPutColor.rgb = OutPutColor.rgb / (OutPutColor.rgb + float3(1.0, 1.0f, 1.0f));
    //OutPutColor.rgb = pow(OutPutColor.rgb, float3(1.0f / gamma, 1.0f / gamma, 1.0f / gamma));
    return OutPutColor;
}