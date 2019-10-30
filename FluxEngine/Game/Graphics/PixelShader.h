#pragma once
#include "Shader.h"
class PixelShader : public Shader
{
public:
	bool Init(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderPath);
	ID3D11PixelShader* GetShader();
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
};

