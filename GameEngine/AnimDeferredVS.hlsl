//#define NO_LIGHTS 24

cbuffer constantBuffer : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;

    float4x4 bones[100];
};


//cbuffer lightsBuffer : register(b1)
//{
//    float4x4 lightViewMatrix[NO_LIGHTS];
//    float4x4 lightProjectionMatrix[NO_LIGHTS];
//    uint lightsSize;
//};

struct VS_INPUT
{
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inTangent : TANGENT;
    float3 inBinormal : BINORMAL;
    
    
    float4 weights : WEIGHTS;
    uint4 boneIDs : BONE_IDs;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
    float3 outNormal : NORMAL;
    float3 outWorldPos : WORLD_POSITION;
    float3 outTangent : TANGENT;
    float3 outBinormal : BINORMAL;
    //float4 lightViewPosition[NO_LIGHTS] : LIGHTVIEWS;
};
VS_OUTPUT main(VS_INPUT input)
{
    //input.inPos.x += input.instancePosition.x;
    //input.inPos.y += input.instancePosition.y;
    //input.inPos.z += input.instancePosition.z;

    float Weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    Weights[0] = input.weights.x;
    Weights[1] = input.weights.y;
    Weights[2] = input.weights.z;
    Weights[3] = input.weights.w;

    float3 posL = float3(0.0f, 0.0f, 0.0f);
    float3 n = float3(0.0f, 0.0f, 0.0f);
    float3 t = float3(0.0f, 0.0f, 0.0f);
    float3 b = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 4; i++)
    {
        posL += Weights[i] * mul(float4(input.inPos, 1.0f), bones[input.boneIDs[i]]).xyz;
        n += Weights[i] * mul(float4(input.inNormal, 0.0f), bones[input.boneIDs[i]]).xyz;
        t += Weights[i] * mul(float4(input.inTangent, 0.0f), bones[input.boneIDs[i]]).xyz;
        b += Weights[i] * mul(float4(input.inBinormal, 0.0f), bones[input.boneIDs[i]]).xyz;
    
    }


    VS_OUTPUT output;
    output.outPosition = mul(float4(posL, 1.0f), worldMatrix);
    output.outPosition = mul(output.outPosition, viewMatrix);
    output.outPosition = mul(output.outPosition, projectionMatrix);

    output.outNormal = normalize(mul(float4(n, 0.0f), worldMatrix)); //set normalMatrix's w to 0 to omit translation with worldMatrix
    output.outTangent = normalize(mul(float4(t, 0.0f), worldMatrix));
    output.outBinormal = normalize(mul(float4(b, 0.0f), worldMatrix));

    output.outWorldPos = mul(float4(posL, 1.0f), worldMatrix);

    output.outTexCoord = input.inTexCoord;
    
   // for (int i = 0; i < NO_LIGHTS; ++i)
   // {
   //     if (i > lightsSize - 1)
   //         break;
   //     output.lightViewPosition[i] = mul(float4(posL, 1.0f), worldMatrix);
   //     output.lightViewPosition[i] = mul(output.lightViewPosition[i], transpose(lightViewMatrix[i]));
   //     output.lightViewPosition[i] = mul(output.lightViewPosition[i], transpose(lightProjectionMatrix[i]));
   // }
    //output.ViewPosition = output.outPosition;

    
    return output;
}