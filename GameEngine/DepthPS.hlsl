
SamplerState SampleType : register(s0);


struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float4 depthPosition : TEXTURE4;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
Texture2D objTexture : TEXTURE : register(t0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float depthValue;
    float4 color;

    float4 TextureColor = objTexture.Sample(SampleType, input.tex);
    
    if (TextureColor.a < 0.95)
    {
        discard;
    }

    //input.depthPosition.z = 1 / input.depthPosition.z;
    depthValue = input.depthPosition.z / input.depthPosition.w;
    
   
    color = float4(depthValue, depthValue, depthValue, 1.0f);
    
  
    return color;
}
