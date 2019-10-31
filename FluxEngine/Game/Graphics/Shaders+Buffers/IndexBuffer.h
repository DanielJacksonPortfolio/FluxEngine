#pragma once
#include "Buffer.h"

class IndexBuffer : public Buffer
{
private:
	IndexBuffer(const IndexBuffer& rhs);
public:
	IndexBuffer() {}

	HRESULT Init(ID3D11Device* device, DWORD* data, UINT indexCount)
	{
		if (this->buffer.Get() != nullptr)
			this->buffer.Reset();

		this->count = indexCount;
		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * this->count;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexBufferData;
		ZeroMemory(&indexBufferData, sizeof(indexBufferData));
		indexBufferData.pSysMem = data;

		return device->CreateBuffer(&indexBufferDesc, &indexBufferData, this->buffer.GetAddressOf());
	}
};

