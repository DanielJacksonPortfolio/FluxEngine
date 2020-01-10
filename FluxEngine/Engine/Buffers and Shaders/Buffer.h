#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "../../../Tools/ErrorLogger.h"

class Buffer
{
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	UINT count = 0;
public:
	ID3D11Buffer* Get() { return this->buffer.Get(); }
	ID3D11Buffer* const* GetAddressOf()const { return this->buffer.GetAddressOf(); }
	const UINT GetCount() { return this->count; }

};

