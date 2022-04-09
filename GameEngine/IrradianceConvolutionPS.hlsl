struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WOLRD_POSITION;
};

TextureCube environmentMap : TEXTURE : register(t0);


SamplerState objSamplerState : SAMPLER : register(s0);
SamplerState objSamplerStateClamp : SAMPLER : register(s1);
SamplerState objSamplerStateMip : SAMPLER : register(s2);


const float PI = 3.14159265359;

float4 main(PS_INPUT input) : SV_TARGET
{
    

    float3 normal = normalize(input.inWorldPos.xyz);
    float3 irradiance = float3(0.0f, 0.0f, 0.0f);

    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));
    
    //float sampleDelta = 3.0f;
    float sampleDelta = 0.025;
    float nrSamples = 0.0;
    
    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
         // spherical to cartesian (in tangent space)
            float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
        // tangent space to world
            float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

            irradiance += environmentMap.Sample(objSamplerState, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
  
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));
    
    return float4(irradiance, 1.0f);
}