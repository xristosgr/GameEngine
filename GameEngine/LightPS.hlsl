cbuffer lightBuffer : register(b5)
{
    float3 color;
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
};


float4 main(PS_INPUT input) : SV_TARGET
{
    return float4(color, 1.0);
}