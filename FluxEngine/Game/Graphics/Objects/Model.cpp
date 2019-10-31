#include "Model.h"

bool Model::Init(const std::string& filepath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader_PosTex>& cb_vertexShader)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->cb_vertexShader = &cb_vertexShader;
	try
	{
		//Temporary Loading
		Vertex_PosTex vertices[] =
		{
			Vertex_PosTex(-1.0f, -1.0f,  1.0f, 0.0f, 1.0f),	//0
			Vertex_PosTex(1.0f, -1.0f,  1.0f , 1.0f, 1.0f),	//1
			Vertex_PosTex(1.0f,  1.0f,  1.0f , 1.0f, 0.0f),	//2
			Vertex_PosTex(-1.0f,  1.0f,  1.0f, 0.0f, 0.0f),	//3						 	   
			Vertex_PosTex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),	//4
			Vertex_PosTex(1.0f, -1.0f, -1.0f,  1.0f, 1.0f),	//5
			Vertex_PosTex(1.0f,  1.0f, -1.0f,  1.0f, 0.0f),	//6
			Vertex_PosTex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f)	//7
		};

		HRESULT hr = this->vertexBuffer.Init(this->device, vertices, ARRAYSIZE(vertices));
		EXCEPT_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh");

		DWORD indices[] =
		{
			0, 1, 2,
			2, 3, 0,
			1, 5, 6,
			6, 2, 1,
			7, 6, 5,
			5, 4, 7,
			4, 0, 3,
			3, 7, 4,
			4, 5, 1,
			1, 0, 4,
			3, 2, 6,
			6, 7, 3
		};

		hr = this->indexBuffer.Init(this->device, indices, ARRAYSIZE(indices));
		EXCEPT_IF_FAILED(hr, "Failed to initialize index buffer for mesh");

		std::string filePath = "data/images/seamless_pavement.jpg";
		if (StringTools::GetFileExtension(filePath) == ".dds")
			hr = CreateDDSTextureFromFile(this->device, StringTools::StandardToWide(filePath).c_str(), this->texture.GetAddressOf(), this->textureView.GetAddressOf());
		else
			hr = CreateWICTextureFromFile(this->device, StringTools::StandardToWide(filePath).c_str(), this->texture.GetAddressOf(), this->textureView.GetAddressOf());
	}
	catch (CustomException & e)
	{
		ErrorLogger::Log(e);
		return false;
	}
	return true;
}

void Model::Draw(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix)
{
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vertexShader->GetAddressOf());

	//Set Shader Buffers
	UINT offset = 0;
	this->deviceContext->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), this->vertexBuffer.GetStridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(this->indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	//Set Textures
	this->deviceContext->PSSetShaderResources(0, 1, textureView.GetAddressOf());

	///Draw Stuff///
	this->deviceContext->DrawIndexed(this->indexBuffer.GetCount(), 0, 0);

}