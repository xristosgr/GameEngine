//#pragma pack_matrix(row_major)
cbuffer constantBuffer : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

struct VS_INPUT
{
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
    //float3 instancePosition : TEXCOORD1;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
    float4x4 projection : TEXCOORD1;
};
VS_OUTPUT main(VS_INPUT input)
{
    //input.inPos.x += input.instancePosition.x;
    //input.inPos.y += input.instancePosition.y;
    //input.inPos.z += input.instancePosition.z;
    VS_OUTPUT output;
    output.outPosition = mul(float4(input.inPos.xyz, 1.0f), worldMatrix);
    //output.outPosition = mul(output.outPosition, viewMatrix);
    output.outPosition = mul(output.outPosition, projectionMatrix);

    output.projection = projectionMatrix;
    output.outTexCoord = input.inTexCoord;
    return output;
}