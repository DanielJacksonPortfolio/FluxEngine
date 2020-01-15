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

		this->boundingSphereRadius = 0.0f;
		for (int i = 0; i < vertices.size(); ++i)
		{
			float r = sqrt(vertices[i].pos.x * vertices[i].pos.x + vertices[i].pos.y * vertices[i].pos.y + vertices[i].pos.z * vertices[i].pos.z);
			if (r > boundingSphereRadius)
				boundingSphereRadius = r;
		}

		std::vector<DWORD> indices;
		for (UINT i = 0; i < mesh->mNumFaces; ++i)
		{
			Face* tempFace = new Face();
			aiFace face = mesh->mFaces[i];
			for (UINT j = 0; j < face.mNumIndices; ++j)
			{
				tempFace->vertices.push_back(*reinterpret_cast<XMFLOAT3*>(&mesh->mVertices[face.mIndices[j]]));
				indices.push_back(face.mIndices[j]);
			}
			faces.push_back(tempFace);
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
	this->worldMatrix = mesh.worldMatrix;
	this->mesh = mesh.mesh;
}

void Mesh::Draw(const XMMATRIX& worldMatrix)
{
	this->worldMatrix = worldMatrix;
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

bool Mesh::RayMeshIntersect(XMVECTOR rayOrigin, XMVECTOR rayDir, float* nearestIntersect)
{
	XMMATRIX invWorld = XMMatrixInverse(nullptr, worldMatrix);
	rayOrigin = XMVector3Transform(rayOrigin, invWorld);
	rayDir = XMVector3Transform(rayDir, invWorld);
	XMMATRIX invTransform = XMMatrixInverse(nullptr, transformMatrix);
	rayOrigin = XMVector3Transform(rayOrigin, invTransform);
	rayDir = XMVector3Transform(rayDir, invTransform);
	XMVectorSetW(rayOrigin, 1.0f);
	XMVectorSetW(rayDir, 0.0f);

	bool intersect = false;
	*nearestIntersect = INT_MAX;

	//if (RaySphereIntersect(rayOrigin, rayDir, nearestIntersect))
	//	return true;

	if (faces.size() > 0)
	{
		for (int i = 0; i < faces.size(); ++i)
		{
			float intersectDistance;
			if (RayTriangleIntersect(rayOrigin, rayDir, &intersectDistance, i))
			{
				intersect = true;
				if (*nearestIntersect > intersectDistance)
				{
					*nearestIntersect = intersectDistance;
				}
			}
		}
		if (intersect)
			return true;
	}
	return false;
}

bool Mesh::RaySphereIntersect(XMVECTOR rayOrigin, XMVECTOR rayDir, float* intersectDistance)
{
	//XMMATRIX invWorld = XMMatrixInverse(nullptr, worldMatrix);
	//XMStoreFloat3(&rayOrigin, XMVector3Transform(XMLoadFloat3(&rayOrigin), invWorld));

	float a = XMVectorGetX(XMVector3Dot(rayDir, rayDir));
	float b = 2*XMVectorGetX(XMVector3Dot(rayDir, rayOrigin));
	float c = XMVectorGetX(XMVector3Dot(rayOrigin, rayOrigin)) - (boundingSphereRadius * boundingSphereRadius);
	//XMFLOAT3 bf; XMStoreFloat3(&bf, XMVector3Dot(rayDir, rayOrigin));
	//float b = 2*bf.x;
	//XMFLOAT3 cf; XMStoreFloat3(&cf, XMVector3Dot(rayOrigin, rayOrigin));
	//float c = cf.x - (boundingSphereRadius*boundingSphereRadius);
	float discriminant = (b * b) - (4 * a * c);
	if (discriminant > 0)
		*intersectDistance = (-b - sqrt(discriminant)) / (2.0 * a);
	else
		*intersectDistance = -1.0f;
	return discriminant > 0;

}
bool Mesh::RayTriangleIntersect(XMVECTOR rayOrigin, XMVECTOR rayDir, float* intersectDistance, int faceIndex)
{
	//XMMATRIX invWorld = XMMatrixInverse(nullptr, worldMatrix);
	//XMStoreFloat3(&rayOrigin, XMVector3Transform(XMLoadFloat3(&rayOrigin), invWorld));
	//XMMATRIX invTransform = XMMatrixInverse(nullptr, transformMatrix);
	//XMStoreFloat3(&rayOrigin, XMVector3Transform(XMLoadFloat3(&rayOrigin), invTransform));
	Face* triangle = faces[faceIndex];
	if (triangle->vertices.size() == 3)
	{
		XMVECTOR vert1 = *reinterpret_cast<XMVECTOR*>(&triangle->vertices[0]);
		XMVECTOR vert2 = *reinterpret_cast<XMVECTOR*>(&triangle->vertices[1]);
		XMVECTOR vert3 = *reinterpret_cast<XMVECTOR*>(&triangle->vertices[2]);
		XMFLOAT3 vert1f; XMStoreFloat3(&vert1f, vert1);
		XMFLOAT3 vert2f; XMStoreFloat3(&vert2f, vert2);
		XMFLOAT3 vert3f; XMStoreFloat3(&vert3f, vert3);

		XMVECTOR U = vert2 - vert1;
		XMVECTOR V = vert3 - vert1;

		XMFLOAT3 faceNormal = XMFLOAT3();
		XMStoreFloat3(&faceNormal, XMVector3Normalize(XMVector3Cross(U, V)));

		float D = -1 * ((faceNormal.x * vert1f.x) + (faceNormal.y * vert1f.y) + (faceNormal.z * vert1f.z));
		float ep1 = faceNormal.x * XMVectorGetX(rayDir) + faceNormal.y * XMVectorGetY(rayDir) + faceNormal.z * XMVectorGetZ(rayDir);
		float ep2 = (faceNormal.x * (XMVectorGetX(rayOrigin) - XMVectorGetX(rayDir)) + faceNormal.y * (XMVectorGetY(rayOrigin) - XMVectorGetY(rayDir)) + faceNormal.z * (XMVectorGetZ(rayOrigin) - XMVectorGetZ(rayDir)));
		if (ep2 == 0) return false;
		float t = -(ep1 + D) / ep2;

		XMFLOAT3 pif = XMFLOAT3(XMVectorGetX(rayOrigin) + XMVectorGetX(rayDir) * t, XMVectorGetY(rayOrigin) + XMVectorGetY(rayDir) * t, XMVectorGetZ(rayOrigin) + XMVectorGetZ(rayDir) * t);
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
					XMStoreFloat3(&intersectLength, XMVector3Length(planeIntersect - rayOrigin));
					*intersectDistance = intersectLength.x;
					return true;
				}
			}
		}
	}
	return false;
}