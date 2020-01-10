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

			if (mesh->mNormals != nullptr)
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

			if (mesh->mTextureCoords[0])
			{
				vertex.texCoord.x = static_cast<float>(mesh->mTextureCoords[0][i].x);
				vertex.texCoord.y = static_cast<float>(mesh->mTextureCoords[0][i].y);
			}

			if (mesh->mTangents != nullptr)
				vertex.tangent = *reinterpret_cast<XMFLOAT3*>(&mesh->mTangents[i]);
			else
				vertex.tangent = { 0.0f,0.0f,0.0f };
			/*if (mesh->mBitangents != nullptr)
				vertex.binormal = *reinterpret_cast<XMFLOAT3*>(&mesh->mBitangents[i]);
			else
				vertex.binormal = { 0.0f,0.0f,0.0f };*/
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

bool Mesh::RayMeshIntersect(XMFLOAT3 rayOrigin, XMFLOAT3 rayDir, float* nearestIntersect)
{
	if (mesh->mNumFaces > 0)
	{
		bool intersect = false;
		for (int i = 0; i < mesh->mNumFaces; ++i)
		{
			float triangleIntersect = INT_MAX;
			if (RayTriangleIntersect(rayOrigin, rayDir, &triangleIntersect, i))
			{
				intersect = true;
				if (*nearestIntersect > triangleIntersect)
				{
					*nearestIntersect = triangleIntersect;
				}
			}
		}
		if (intersect)
			return true;
	}
	return false;
}

bool Mesh::RayTriangleIntersect(XMFLOAT3 rayOrigin, XMFLOAT3 rayDir, float* triangleIntersect, int faceIndex)
{
	aiFace triangle = mesh->mFaces[faceIndex];
	if (triangle.mNumIndices == 3)
	{
		XMVECTOR vert1 = *reinterpret_cast<XMVECTOR*>(&mesh->mVertices[triangle.mIndices[0]]);
		XMVECTOR vert2 = *reinterpret_cast<XMVECTOR*>(&mesh->mVertices[triangle.mIndices[1]]);
		XMVECTOR vert3 = *reinterpret_cast<XMVECTOR*>(&mesh->mVertices[triangle.mIndices[2]]);
		XMFLOAT3 vert1f = *reinterpret_cast<XMFLOAT3*>(&mesh->mVertices[triangle.mIndices[0]]);
		XMFLOAT3 vert2f = *reinterpret_cast<XMFLOAT3*>(&mesh->mVertices[triangle.mIndices[1]]);
		XMFLOAT3 vert3f = *reinterpret_cast<XMFLOAT3*>(&mesh->mVertices[triangle.mIndices[2]]);

		XMVECTOR U = vert2 - vert1;
		XMVECTOR V = vert3 - vert1;

		XMFLOAT3 faceNormal = XMFLOAT3();
		XMStoreFloat3(&faceNormal, XMVector3Normalize(XMVector3Cross(U, V)));

		float D = -1 * ((faceNormal.x * vert1f.x) + (faceNormal.y * vert1f.y) + (faceNormal.z * vert1f.z));
		float ep1 = faceNormal.x * rayDir.x + faceNormal.y * rayDir.y + faceNormal.z * rayDir.z;
		float ep2 = (faceNormal.x * (rayOrigin.x - rayDir.x) + faceNormal.y * (rayOrigin.y - rayDir.y) + faceNormal.z * (rayOrigin.z - rayDir.z));
		if (ep2 == 0) return false;
		float t = -(ep1 + D) / ep2;

		XMFLOAT3 pif = XMFLOAT3(rayOrigin.x + rayDir.x * t, rayOrigin.y + rayDir.y * t, rayOrigin.z + rayDir.z * t);
		XMVECTOR planeIntersect = XMLoadFloat3(&pif);
		//// Research Barycentric Coordinates
		XMVECTOR cp1 = XMVector3Cross((vert3 - vert2), (planeIntersect - vert2));
		XMVECTOR cp2 = XMVector3Cross((vert3 - vert2), (vert1 - vert2));
		if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
		{
			cp1 = XMVector3Cross((vert3 - vert1), (planeIntersect - vert1));
			cp2 = XMVector3Cross((vert3 - vert1), (vert2 - vert1));
			if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
			{
				cp1 = XMVector3Cross((vert3 - vert1), (planeIntersect - vert1));
				cp2 = XMVector3Cross((vert3 - vert1), (vert3 - vert1));
				if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
				{
					XMFLOAT3 intersectLength;
					XMStoreFloat3(&intersectLength, XMVector3Length(planeIntersect - XMLoadFloat3(&rayOrigin)));
					*triangleIntersect = intersectLength.x;
					return true;
				}
			}
		}
	}
	return false;
}