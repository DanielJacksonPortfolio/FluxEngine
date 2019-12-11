cbuffer lightBuffer : register(b0)
{
	float3 pointLightColor;
}

struct PS_INPUT
{
    float4 inPos : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WORLD_POSITION;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
	if (all(sampleColor == float3(0.8f, 0.8f, 0.8f)))
		return float4(pointLightColor, 1.0f);
	else
		return float4(sampleColor, 1.0f);
}