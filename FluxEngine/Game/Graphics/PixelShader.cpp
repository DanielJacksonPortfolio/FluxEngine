#include "PixelShader.h"

bool PixelShader::Init(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderPath)
{
	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), this->shaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to load shader @ ";
		errorMsg += shaderPath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = device->CreatePixelShader(this->shaderBuffer->GetBufferPointer(), this->shaderBuffer->GetBufferSize(), NULL, this->shader.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to create pixel shader @ ";
		errorMsg += shaderPath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	return true;
}

ID3D11PixelShader* PixelShader::GetShader()
{
	return this->shader.Get();
}