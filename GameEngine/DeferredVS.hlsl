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
    float3 inNormal : NORMAL;
    float3 inTangent : TANGENT;
    float3 inBinormal : BINORMAL;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
    float3 outNormal : NORMAL;
    float3 outWorldPos : WORLD_POSITION;
    float3 outTangent : TANGENT;
    float3 outBinormal : BINORMAL;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.outPosition = mul(float4(input.inPos.xyz, 1.0f), worldMatrix);
    output.outPosition = mul(output.outPosition, viewMatrix);
    output.outPosition = mul(output.outPosition, projectionMatrix);

    output.outTexCoord = input.inTexCoord;

       //Calculate normals
    output.outNormal = normalize(mul(float4(input.inNormal, 0.0f), worldMatrix));
    

    output.outTangent = normalize(mul(float4(input.inTangent, 0.0f), worldMatrix));
    output.outBinormal = normalize(mul(float4(input.inBinormal, 0.0f), worldMatrix));
    
    output.outWorldPos = mul(float4(input.inPos, 1.0f), worldMatrix);
  
    return output;
}