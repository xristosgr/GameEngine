struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
};


float4 main(PS_INPUT input) : SV_TARGET
{
    return float4(1,0,0, 1.0);
}