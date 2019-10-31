#pragma pack_matrix( row_major )

cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
};

struct VS_INPUT
{
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 outPos : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.outPos = mul(float4(input.inPos, 1.0f), wvpMatrix);
    output.outTexCoord = input.inTexCoord;
    return output;
}