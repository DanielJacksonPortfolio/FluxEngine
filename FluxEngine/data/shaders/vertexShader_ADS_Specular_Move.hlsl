#pragma pack_matrix( row_major )

cbuffer perObjectBuffer : register(b0)
{
	float4x4 wvpMatrix;
	float4x4 worldMatrix;
	float elapsedTime;
};

struct VS_INPUT
{
	float3 inPos : POSITION;
	float2 inTexCoord : TEXCOORD;
	float3 inNormal : NORMAL;
	float3 inTangent : TANGENT;
	float3 inBinormal : BINORMAL;
};

struct VS_OUTPUT
{
	float4 outPos : SV_POSITION;
	float2 outTexCoord : TEXCOORD;
	float3 outNormal : NORMAL;
	float3 outWorldPos : WORLD_POSITION;
	float3x3 outTBN : TBN;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	float3 newPos = input.inPos;
	newPos.y += (sin(newPos.x * 10 + elapsedTime * 2.0) + sin(newPos.z * 0.1 + elapsedTime * 2.2)) * 0.5f;
	output.outPos = mul(float4(newPos, 1.0f), wvpMatrix);
	output.outWorldPos = mul(float4(newPos, 1.0f), worldMatrix);
	output.outTexCoord = input.inTexCoord;
	output.outNormal = normalize(mul(float4(input.inNormal, 0.0f), worldMatrix));
	float3 tangent = normalize(mul(float4(input.inTangent, 0.0f), worldMatrix)); //output.outTangent
	tangent = normalize(tangent - dot(tangent, output.outNormal) * output.outNormal);
	float3 binormal = normalize(mul(float4(input.inBinormal, 0.0f), worldMatrix)); //output.outTangent
	binormal = normalize(binormal - dot(binormal, output.outNormal) * output.outNormal);
	output.outTBN = float3x3(tangent, binormal, output.outNormal);
	return output;
}