#define MAX_LIGHTS 256
cbuffer lightBuffer : register(b0)
{
	float4 pointLightColor[MAX_LIGHTS];
	float4 pointLightPosition[MAX_LIGHTS];
	float4 pointLightFactors[MAX_LIGHTS];
	
	float4 directionalLightColor[MAX_LIGHTS];
	float4 directionalLightDirection[MAX_LIGHTS];
	float4 directionalLightStrength[MAX_LIGHTS];

	float3 ambientLightColor;
	float ambientLightStrength;
	
    float3 cameraPosition;
    float shininess;
	
    int grayscale;
    bool normalMap;
    bool specularMap;
	int numPLights;
	
	int numDLights;
}

struct PS_INPUT
{
    float4 inPos : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WORLD_POSITION;
    float3x3 inTBN : TBN;
};

Texture2D objTexture : TEXTURE : register(t0);
Texture2D specTexture : TEXTURE : register(t1);
Texture2D normTexture : TEXTURE : register(t2);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{

    float3 textureColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    float3 specularSample = specTexture.Sample(objSamplerState, input.inTexCoord);
    float3 normalSample = normTexture.Sample(objSamplerState, input.inTexCoord);

	if(normalMap)
    {
		input.inNormal = normalize((normalSample * 2.0) - 1.0);
		input.inNormal = normalize(mul(input.inNormal, input.inTBN));
	}

    float3 ambientLight = ambientLightColor * ambientLightStrength;
    float3 appliedLight = ambientLight;
    
	float3 vectorToEye = normalize(cameraPosition - input.inWorldPos);
    
	for (int i = 0; i < numPLights; ++i)
	{
		float3 vectorToPointLight = normalize(pointLightPosition[i].rgb - input.inWorldPos);
		float3 pointLightIntensity = saturate(dot(vectorToPointLight, input.inNormal));
		float3 diffuse = pointLightIntensity * pointLightColor[i].rgb;
		
		float3 reflection = reflect(-vectorToPointLight, input.inNormal);
		
		float specularIntensity = pow(saturate(dot(vectorToEye, reflection)), shininess);
		float3 specular;
		if (specularMap)
			specular = specularIntensity * pointLightColor[i].rgb * specularSample;
		else
			specular = specularIntensity * pointLightColor[i].rgb;
		
		float distanceToPointLight = distance(pointLightPosition[i].rgb, input.inWorldPos);
		float attenuationFactor = 1 / (pointLightFactors[i].g + pointLightFactors[i].b * distanceToPointLight + pointLightFactors[i].a * pow(distanceToPointLight, 2));

		appliedLight += (diffuse + specular) * attenuationFactor * pointLightFactors[i].r;

	}
	for (int j = 0; j < numDLights; ++j)
	{
		float directionalEffect = saturate(dot(directionalLightDirection[j].rgb, input.inNormal));
		appliedLight += (directionalEffect * directionalLightColor[j].rgb * directionalLightStrength[j].r);
        
		float3 reflection = reflect(-directionalLightDirection[j].rgb, input.inNormal);
		float specularEffect = pow(saturate(dot(reflection, vectorToEye)), shininess);
		float3 specular = specularEffect * directionalLightColor[j].rgb * directionalLightStrength[j].r;        
		if (specularMap)
			appliedLight += (specular * specularSample);
		else
			appliedLight += specular;
	}
	float3 finalColor =  textureColor * appliedLight;
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