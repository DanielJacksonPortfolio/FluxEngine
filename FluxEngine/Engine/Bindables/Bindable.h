#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include "../../Tools/ErrorLogger.h"
class Bindable
{
protected:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
public:
	void virtual Bind() {}
};

