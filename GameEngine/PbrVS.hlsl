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
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
    float4 ViewPosition : TEXCOORD1;
    float distToCamera : TEXCOORD2;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.outPosition = mul(float4(input.inPos.xyz, 1.0f), worldMatrix);
    output.outPosition = mul(output.outPosition, projectionMatrix);

    output.outTexCoord = input.inTexCoord;
    
    float4 position = mul(float4(input.inPos.xyz, 1.0f), worldMatrix);
    position = mul(position, viewMatrix);
    position = mul(position, projectionMatrix);
    output.distToCamera = position.w;
    
    output.ViewPosition = mul(float4(input.inPos, 1.0f), viewMatrix);
    return output;
}