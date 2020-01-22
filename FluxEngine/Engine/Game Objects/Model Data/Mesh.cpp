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

bool Mesh::RayMeshIntersect(XMMATRIX worldMatrix, XMVECTOR rayOrigin, XMVECTOR rayDir, float& intersectDistance)
{
	bool intersect = false;
	if (faces.size() > 0)
	{
		worldMatrix = this->transformMatrix * worldMatrix;
		for (int i = 0; i < faces.size(); ++i)
			if (RayTriangleIntersect(worldMatrix, rayOrigin, rayDir, faces[i], intersectDistance))
				intersect = true;
	}
	return intersect;
}

bool Mesh::RayTriangleIntersect(XMMATRIX worldMatrix, XMVECTOR rayOrigin, XMVECTOR rayDir, aiFace face, float& intersectDistance)
{
	if (face.mNumIndices == 3)
	{
		XMVECTOR vertex0 = XMVectorSet(vertices[face.mIndices[0]].pos.x, vertices[face.mIndices[0]].pos.y, vertices[face.mIndices[0]].pos.z, 1.0f);
		XMVECTOR vertex1 = XMVectorSet(vertices[face.mIndices[1]].pos.x, vertices[face.mIndices[1]].pos.y, vertices[face.mIndices[1]].pos.z, 1.0f);
		XMVECTOR vertex2 = XMVectorSet(vertices[face.mIndices[2]].pos.x, vertices[face.mIndices[2]].pos.y, vertices[face.mIndices[2]].pos.z, 1.0f);

		XMVECTOR normal = XMVector3Normalize(XMVector3Transform(XMVector3Cross(vertex1 - vertex0, vertex2 - vertex0), XMMatrixTranspose(XMMatrixInverse(nullptr, worldMatrix)))) * (XMVectorGetX(XMMatrixDeterminant(worldMatrix)) < 0 ? -1 : 1);
		
		vertex0 = XMVector3Transform(vertex0, worldMatrix);
		vertex1 = XMVector3Transform(vertex1, worldMatrix);
		vertex2 = XMVector3Transform(vertex2, worldMatrix);

		float normalDotRayDir = XMVectorGetX(XMVector3Dot(normal, rayDir));
		if (fabs(normalDotRayDir) < static_cast<float>(1e-8))
			return false;

		float t = -1 * (XMVectorGetX(XMVector3Dot(normal, rayOrigin)) - XMVectorGetX(XMVector3Dot(normal, vertex0))) / normalDotRayDir;
		if (t < 0) return false;

		XMVECTOR pointOfIntersect = rayOrigin + t * rayDir;

		if (XMVectorGetX(XMVector3Dot(normal, XMVector3Cross(vertex1 - vertex0, pointOfIntersect - vertex0))) < 0 ||
			XMVectorGetX(XMVector3Dot(normal, XMVector3Cross(vertex2 - vertex1, pointOfIntersect - vertex1))) < 0 ||
			XMVectorGetX(XMVector3Dot(normal, XMVector3Cross(vertex0 - vertex2, pointOfIntersect - vertex2))) < 0) return false;

		float distance = XMVectorGetX(XMVector3Length(pointOfIntersect - rayOrigin));
		if (intersectDistance > distance)
			intersectDistance = distance;
	
		return true;
	}
	return false;
}

const XMMATRIX& Mesh::GetTransformMatrix()
{
	return this->transformMatrix;
}