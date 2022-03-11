cbuffer perObjectBuffer : register(b0)
{
 
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;

  

    float4x4 bones[100];
};

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

    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float4 depthPosition : TEXTURE4;


};

VS_OUTPUT main(VS_INPUT input)
{

  
    VS_OUTPUT output;
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

    
    output.position = mul(float4(posL, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    

    output.depthPosition = output.position;
   
    
    output.tex = input.inTexCoord;
    
    
 
     
    return output;
}