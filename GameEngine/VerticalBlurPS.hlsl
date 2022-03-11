struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float2 centerTexCoords : TEXCOORD1;
    float2 texCoords[21] : TEXCOORD2;

    
    
};

Texture2D shaderTexture : TEXTURE : register(t0);
SamplerState SampleType : register(s1);

float4 main(PS_INPUT input) : SV_TARGET
{

    float normalization = 0.0f;
    float4 color = float4(0, 0, 0, 1);
    

    float weight[] =
    {
        
        9.421273256292181e-7,
    0.000009677359081674635,
    0.0000777489201475167,
    0.0004886419989245074,
    0.0024027325605485827,
    0.009244616587506386,
                           
    0.027834685329492057,
                           
    0.06559073722230267,
                           
    0.12097746070390959,
                           
    0.17466647146354097,
                           
    0.19741257145444083,
                           
    0.17466647146354097,
                           
    0.12097746070390959,
                           
    0.06559073722230267,
                           
    0.027834685329492057,
                           
    0.009244616587506386,
                           
    0.0024027325605485827,
                           
    0.0004886419989245074,
                           
    0.0000777489201475167,
                           
    0.000009677359081674635,
                           
    9.421273256292181e-7

    };
 
    for (int i = 0; i < 21; ++i)
    {
        color += shaderTexture.Sample(SampleType, input.texCoords[i]) * weight[i];
    }

    return float4(color.xyz, 1.0f);
    
}