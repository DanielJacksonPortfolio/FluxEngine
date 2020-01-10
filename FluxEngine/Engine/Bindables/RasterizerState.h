#pragma once
#include "Bindable.h"
class RasterizerState : public Bindable
{
private:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
public:
	RasterizerState(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, D3D11_CULL_MODE cullMode = D3D11_CULL_MODE::D3D11_CULL_BACK, D3D11_FILL_MODE fillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID)
	{
		this->device = device;
		this->deviceContext = deviceContext;

		CD3D11_RASTERIZER_DESC rasterizerDesc_CullFront_WireFrame(D3D11_DEFAULT);
		rasterizerDesc_CullFront_WireFrame.CullMode = cullMode;
		rasterizerDesc_CullFront_WireFrame.FillMode = fillMode;
		HRESULT hr = this->device->CreateRasterizerState(&rasterizerDesc_CullFront_WireFrame, this->rasterizerState.GetAddressOf());
		EXCEPT_IF_FAILED(hr, "CreateRasterizerState Failed");
	}
	void Bind() override
	{
		this->deviceContext->RSSetState(this->rasterizerState.Get());
	}
};

