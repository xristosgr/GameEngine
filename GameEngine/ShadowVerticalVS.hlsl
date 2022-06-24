cbuffer constantBuffer : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

cbuffer windowParams : register(b2)
{
    float window_width;
    float window_height;
}
struct VS_INPUT
{
    float3 position : POSITION;
    float2 tex : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float2 centerTexCoords : TEXCOORD1;
    float2 texCoords[21] : TEXCOORD2;
    
};


VS_OUTPUT main(VS_INPUT input)
{

    VS_OUTPUT output;
    float texelSize;
    output.position = mul(float4(input.position.xyz, 1.0f), worldMatrix);
    //output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.tex = input.tex;
    
    output.centerTexCoords = output.position * 0.5 + 0.5;
    texelSize = (1.0f / window_height);
   
    
    for (int i = -10; i <= 10; ++i)
    {
        output.texCoords[i + 10] = output.centerTexCoords + float2(0, texelSize * i);

    }
    
    return output;
}