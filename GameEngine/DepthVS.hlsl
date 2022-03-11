cbuffer perObjectBuffer : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

struct VS_INPUT
{
    float4 position : POSITION;
    float2 tex : TEXCOORD;
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
    
    input.position.w = 1.0f;

    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.tex = input.tex;

    output.depthPosition = output.position;
    
    return output;
}