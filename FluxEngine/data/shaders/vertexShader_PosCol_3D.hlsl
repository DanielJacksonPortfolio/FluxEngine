#pragma pack_matrix( row_major )

cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
};

struct VS_INPUT
{
    float3 inPos : POSITION;
    float3 inColor : COLOR;
};

struct VS_OUTPUT
{
    float4 outPos : SV_POSITION;
    float3 outColor : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.outPos = mul(float4(input.inPos, 1.0f), wvpMatrix);
    output.outColor = input.inColor;
    return output;
}