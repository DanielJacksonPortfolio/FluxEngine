#include "VertexShader.h"

bool VertexShader::Init(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderPath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements)
{
	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), this->shaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to load shader @ ";
		errorMsg += shaderPath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = device->CreateVertexShader(this->shaderBuffer->GetBufferPointer(), this->shaderBuffer->GetBufferSize(), NULL, this->shader.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to create vertex shader @ ";
		errorMsg += shaderPath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = device->CreateInputLayout(layoutDesc, numElements, this->shaderBuffer->GetBufferPointer(), this->shaderBuffer->GetBufferSize(), this->inputLayout.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "CreateInputLayout Failed.");
		return false;
	}

	return true;
}

ID3D11VertexShader* VertexShader::GetShader()
{
	return this->shader.Get();
}

ID3D11InputLayout* VertexShader::GetInputLayout()
{
	return this->inputLayout.Get();
}