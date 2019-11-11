cbuffer lightBuffer : register(b0)
{
    float3 ambientLightColor;
    float ambientLightStrength;

    float3 directionalLightColor;
    float directionalLightStrength;

    float3 directionalLightDirection;
    float pointLightStrength;

    float3 pointLightColor;
    float pointLightConstantAttenuationFactor;

    float3 pointLightPosition;
    float pointLightLinearAttenuationFactor;

    float3 cameraPosition;
    float shininess;

    int grayscale;
    float pointLightExponentAttenuationFactor;

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

    float3 ambientLight = ambientLightColor * ambientLightStrength;
    float3 appliedLight = ambientLight;
    
    float distanceToPointLight = distance(pointLightPosition, input.inWorldPos);
    float attenuationFactor = 1 / (pointLightConstantAttenuationFactor + pointLightLinearAttenuationFactor * distanceToPointLight + pointLightExponentAttenuationFactor * pow(distanceToPointLight, 2));
    float3 vectorToPointLight = normalize(pointLightPosition - input.inWorldPos);
    float3 pointLightIntensity = max(dot(vectorToPointLight, input.inNormal), 0) * attenuationFactor;

    float directionalEffect = max(dot(directionalLightDirection, input.inNormal), 0.0f);
    float3 diffuseLight = (pointLightIntensity * pointLightStrength * pointLightColor) + (directionalEffect * directionalLightColor * directionalLightStrength);

    appliedLight += diffuseLight;

    float3 vectorToEye = normalize(cameraPosition - input.inWorldPos.xyz);
    float3 directionalR = reflect(-directionalLightDirection, input.inNormal);

    float specularDEffect = pow(max(dot(directionalR, vectorToEye), 0.0f), shininess);
    float3 specularD = specularDEffect * directionalEffect * directionalLightColor * directionalLightStrength;

    float3 pointR = reflect(-vectorToPointLight, input.inNormal);

    float specularPEffect = pow(max(dot(pointR, vectorToEye), 0.0f), shininess);
    float3 specularP = specularPEffect * pointLightIntensity * pointLightColor * pointLightStrength;

    appliedLight += specularD + specularP;

    float3 finalColor = sampleColor * appliedLight;

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