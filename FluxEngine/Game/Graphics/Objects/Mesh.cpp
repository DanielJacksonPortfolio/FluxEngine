#include "Mesh.h"

Mesh::Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float scale, aiMesh* mesh, std::vector<Texture>& textures, const XMMATRIX& transformMatrix, std::string directory)
{
	this->deviceContext = deviceContext;
	this->textures = textures;
	this->transformMatrix = transformMatrix;
	this->mesh = mesh;
	this->directory = directory;
	try
	{
		std::vector<Vertex_PosTexNormTanBinorm> vertices;

		for (UINT i = 0; i < mesh->mNumVertices; ++i)
		{
			Vertex_PosTexNormTanBinorm vertex;

			vertex.pos.x = mesh->mVertices[i].x * scale;
			vertex.pos.y = mesh->mVertices[i].y * scale;
			vertex.pos.z = mesh->mVertices[i].z * scale;

			if (mesh->mNormals != nullptr) //index 0 is main texture
			{
				vertex.normal.x = mesh->mNormals[i].x;
				vertex.normal.y = mesh->mNormals[i].y;
				vertex.normal.z = mesh->mNormals[i].z;
				//vertex.normal = *reinterpret_cast<XMFLOAT3*>(&mesh->mNormals[i]);
			}
			else
			{
				vertex.normal = { 0,0,0 };
			}

			if (mesh->mTextureCoords[0]) //index 0 is main texture
			{
				vertex.texCoord.x = static_cast<float>(mesh->mTextureCoords[0][i].x);
				vertex.texCoord.y = static_cast<float>(mesh->mTextureCoords[0][i].y);
			}

			//vertex.tangent.x = mesh->mTangents[i].x;
			//vertex.tangent.y = mesh->mTangents[i].y;
			//vertex.tangent.z = mesh->mTangents[i].z;
			if (mesh->mTangents != nullptr)
				vertex.tangent = *reinterpret_cast<XMFLOAT3*>(&mesh->mTangents[i]);
			else
				vertex.tangent = { 0.0f,0.0f,0.0f };
			if (mesh->mBitangents != nullptr)
				vertex.binormal = *reinterpret_cast<XMFLOAT3*>(&mesh->mBitangents[i]);
			else
				vertex.binormal = { 0.0f,0.0f,0.0f };
			vertices.push_back(vertex);
		}

		std::vector<DWORD> indices;
		for (UINT i = 0; i < mesh->mNumFaces; ++i)
		{
			aiFace face = mesh->mFaces[i];
			for (UINT j = 0; j < face.mNumIndices; ++j)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		HRESULT hr = this->vertexBuffer.Init(device, vertices.data(), vertices.size());
		EXCEPT_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh");

		hr = this->indexBuffer.Init(device, indices.data(), indices.size());
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
