#define NO_LIGHTS 4
cbuffer constantBuffer : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

cbuffer lightsBuffer : register(b1)
{
    float4x4 lightViewMatrix[NO_LIGHTS];
    float4x4 lightProjectionMatrix[NO_LIGHTS];
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
    float3 outWorldPos : WOLRD_POSITION;
    float3 outTangent : TANGENT;
    float3 outBinormal : BINORMAL;
    float4 ViewPosition : TEXCOORD1;
    float4 lightViewPosition[NO_LIGHTS] : LIGHTVIEWS;
};
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.outPosition = mul(float4(input.inPos.xyz, 1.0f), worldMatrix);
    //output.outPosition = mul(output.outPosition, viewMatrix);
    output.outPosition = mul(output.outPosition, projectionMatrix);

    output.outTexCoord = input.inTexCoord;

       //Calculate normals
    output.outNormal = normalize(mul(float4(input.inNormal, 0.0f), worldMatrix));
    output.outTangent = normalize(mul(float4(input.inTangent, 0.0f), worldMatrix));
    output.outBinormal = normalize(mul(float4(input.inBinormal, 0.0f), worldMatrix));
    
    output.outWorldPos = mul(float4(input.inPos, 1.0f), worldMatrix);


    for (int i = 0; i < NO_LIGHTS; ++i)
    {
        output.lightViewPosition[i] = mul(float4(input.inPos, 1.0f), worldMatrix);
        output.lightViewPosition[i] = mul(output.lightViewPosition[i], transpose(lightViewMatrix[i]));
        output.lightViewPosition[i] = mul(output.lightViewPosition[i], transpose(lightProjectionMatrix[i]));
    }
        

    //output.ViewPosition = output.outPosition;
    output.ViewPosition = mul(float4(input.inPos, 1.0f), viewMatrix);

    return output;
}