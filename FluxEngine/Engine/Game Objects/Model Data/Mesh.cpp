#include "Mesh.h"

Mesh::Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, aiMesh* mesh, std::vector<Texture>& textures, const XMMATRIX& transformMatrix, std::string directory, std::vector<Vertex_PosTexNormTan>* modelVertices)
{
	this->deviceContext = deviceContext;
	this->textures = textures;
	this->transformMatrix = transformMatrix;
	this->mesh = mesh;

	this->directory = directory;
	try
	{
		for (UINT i = 0; i < mesh->mNumVertices; ++i)
		{
			Vertex_PosTexNormTan vertex;

			vertex.pos.x = mesh->mVertices[i].x;
			vertex.pos.y = mesh->mVertices[i].y;
			vertex.pos.z = mesh->mVertices[i].z;

			if (mesh->mNormals != nullptr)
				vertex.normal = *reinterpret_cast<XMFLOAT3*>(&mesh->mNormals[i]);
			else
				vertex.normal = { 0,0,0 };

			if (mesh->mTextureCoords[0])
			{
				vertex.texCoord.x = static_cast<float>(mesh->mTextureCoords[0][i].x);
				vertex.texCoord.y = static_cast<float>(mesh->mTextureCoords[0][i].y);
			}

			if (mesh->mTangents != nullptr)
				vertex.tangent = *reinterpret_cast<XMFLOAT3*>(&mesh->mTangents[i]);
			else
				vertex.tangent = { 0.0f,0.0f,0.0f };
			vertices.push_back(vertex);
			XMStoreFloat3(&vertex.pos,XMVector3Transform(XMLoadFloat3(&vertex.pos),transformMatrix));
			modelVertices->push_back(vertex);
		}

		for (UINT i = 0; i < mesh->mNumFaces; ++i)
		{
			aiFace face = mesh->mFaces[i];
			for (UINT j = 0; j < face.mNumIndices; ++j)
			{
				indices.push_back(face.mIndices[j]);
			}
			faces.push_back(face);
		}

		HRESULT hr = this->vertexBuffer.Init(device, vertices.data(), static_cast<UINT>(vertices.size()));
		EXCEPT_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh");

		hr = this->indexBuffer.Init(device, indices.data(), static_cast<UINT>(indices.size()));
		EXCEPT_IF_FAILED(hr, "Failed to initialize index buffer for mesh");

	}
	catch (CustomException & exception)
	{
		ErrorLogger::Log(exception);
		return;
	}
}

Mesh::Mesh(const Mesh& mesh)
{
	this->deviceContext = mesh.deviceContext;
	this->indexBuffer = mesh.indexBuffer;
	this->vertexBuffer = mesh.vertexBuffer;
	this->textures = mesh.textures;
	this->transformMatrix = mesh.transformMatrix;
	this->mesh = mesh.mesh;
}

void Mesh::Draw()
{
	//Set Textures
	for (size_t i = 0; i < textures.size(); ++i)
	{
		if (textures[i].GetType() == aiTextureType::aiTextureType_DIFFUSE)
		{
			this->deviceContext->PSSetShaderResources(0, 1, textures[i].GetTextureResourceViewAddress());
		}
		if (textures[i].GetType() == aiTextureType::aiTextureType_SPECULAR)
		{
			this->deviceContext->PSSetShaderResources(1, 1, textures[i].GetTextureResourceViewAddress());		}
		if (textures[i].GetType() == aiTextureType::aiTextureType_HEIGHT)
		{
			this->deviceContext->PSSetShaderResources(2, 1, textures[i].GetTextureResourceViewAddress());
		}
	}

	///Draw Stuff///
	UINT offset = 0;
	this->deviceContext->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), this->vertexBuffer.GetStridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(this->indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	this->deviceContext->DrawIndexed(this->indexBuffer.GetCount(), 0, 0);
}

const XMMATRIX& Mesh::GetTransformMatrix()
{
	return this->transformMatrix;
}