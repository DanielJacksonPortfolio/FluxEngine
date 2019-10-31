#pragma once
#include "Buffer.h"
#include <memory>

template<class T>

class VertexBuffer : public Buffer
{
private:
	UINT stride = sizeof(T);
public:
	VertexBuffer() {}
	VertexBuffer(const VertexBuffer<T>& rhs)
	{
		this->buffer = rhs.buffer;
		this->count = rhs.count;
		this->stride = rhs.stride;
	}
	VertexBuffer<T>& operator=(const VertexBuffer<T>& a)
	{
		this->buffer = a.buffer;
		this->count = a.count;
		this->stride = a.stride;
		return *this;
	}

	const UINT  GetStride() { return this->stride; }
	const UINT* GetStridePtr() { return &this->stride; }

	HRESULT Init(ID3D11Device* device, T* data, UINT vertexCount)
	{
		if (this->buffer.Get() != nullptr)
			this->buffer.Reset();

		this->count = vertexCount;

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = this->stride * this->count;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = data;

		return device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->buffer.GetAddressOf());
	}
};

