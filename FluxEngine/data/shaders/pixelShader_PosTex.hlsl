cbuffer buffer : register(b0)
{
    int grayscale;
}

struct PS_INPUT
{
    float4 inPos : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);

    float3 finalColor = sampleColor;
    float gray = 0.0f;

    if (grayscale == 1)
    {
        gray = (finalColor.r + finalColor.g + finalColor.b) / 3;
    }
    if (grayscale == 2)
    {
        gray = finalColor.r * 0.3f + finalColor.g * 0.59f + finalColor.b * 0.11f;
    }
    if (grayscale != 0)
    {
        finalColor = (gray, gray, gray);
    }

    return float4(finalColor, 1.0f);
}