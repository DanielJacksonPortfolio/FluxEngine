#include "Model.h"

bool Model::Init(const std::string& filepath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader>& cb_vertexShader, ConstantBuffer<CB_pixelShader>& cb_pixelShader)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->cb_vertexShader = &cb_vertexShader;
	this->cb_pixelShader = &cb_pixelShader;
	try
	{
		if (!this->LoadModel(filepath))
			return false;
	}
	catch (CustomException & e)
	{
		ErrorLogger::Log(e);
		return false;
	}
	return true;
}

Model::Model(const Model& model)
{
	this->meshes = model.meshes;
	this->device = model.device;
	this->deviceContext = model.deviceContext;
	this->cb_vertexShader = model.cb_vertexShader;
	this->cb_pixelShader = model.cb_pixelShader;
	this->directory = model.directory;
}

void Model::SetOrigin()
{
	XMFLOAT3 origin = XMFLOAT3();
	for (int i = 0; i < vertices.size(); ++i)
	{
		origin.x += vertices[i].pos.x;
		origin.y += vertices[i].pos.y;
		origin.z += vertices[i].pos.z;
	}
	origin.x = origin.x / vertices.size();
	origin.y = origin.y / vertices.size();
	origin.z = origin.z / vertices.size();
	this->originVector = XMLoadFloat3(&origin);
	SetBoundingShape(XMMatrixIdentity());
}

void Model::SetBoundingShape(XMMATRIX rotation)
{
	this->boundingSphere.radius = 0.0f;
	float minDistance = INT_MAX;
	float maxDistance = 0.0f;
	for (int i = 0; i < vertices.size(); ++i)
	{
		float x = abs(vertices[i].pos.x - XMVectorGetX(this->originVector));
		float y = abs(vertices[i].pos.y - XMVectorGetY(this->originVector));
		float z = abs(vertices[i].pos.z - XMVectorGetZ(this->originVector));
		float distanceFromOrigin = sqrt(x * x + y * y + z * z);
		if (distanceFromOrigin < minDistance) minDistance = distanceFromOrigin;
		if (distanceFromOrigin > maxDistance) maxDistance = distanceFromOrigin;
	}
	this->boundingSphere.radius = maxDistance;
	SetScaledBoundingSphereRadius(1);

	
	this->axisAlignedBoundingBox.halfWidth = 0.0f;
	this->axisAlignedBoundingBox.halfHeight = 0.0f;
	this->axisAlignedBoundingBox.halfDepth = 0.0f;
	this->orientedBoundingBox.halfWidth = 0.0f;
	this->orientedBoundingBox.halfHeight = 0.0f;
	this->orientedBoundingBox.halfDepth = 0.0f;
	float minDistanceXAABB = 0.0f; float maxDistanceXAABB = 0.0f;
	float minDistanceYAABB = 0.0f; float maxDistanceYAABB = 0.0f;
	float minDistanceZAABB = 0.0f; float maxDistanceZAABB = 0.0f;
	float minDistanceX = 0.0f; float maxDistanceX = 0.0f;
	float minDistanceY = 0.0f; float maxDistanceY = 0.0f;
	float minDistanceZ = 0.0f; float maxDistanceZ = 0.0f;
	for (int i = 0; i < vertices.size(); ++i)
	{
		float x = vertices[i].pos.x - XMVectorGetX(this->originVector);
		float y = vertices[i].pos.y - XMVectorGetY(this->originVector);
		float z = vertices[i].pos.z - XMVectorGetZ(this->originVector);

		XMVECTOR xyz = XMVector3Transform(XMVectorSet(x, y, z, 0), rotation);
		float AABBx = XMVectorGetX(xyz);
		float AABBy = XMVectorGetY(xyz);
		float AABBz = XMVectorGetZ(xyz);

		if (AABBx > maxDistanceXAABB) maxDistanceXAABB = AABBx;
		if (AABBx < minDistanceXAABB) minDistanceXAABB = AABBx;
		if (AABBy > maxDistanceYAABB) maxDistanceYAABB = AABBy;
		if (AABBy < minDistanceYAABB) minDistanceYAABB = AABBy;
		if (AABBz > maxDistanceZAABB) maxDistanceZAABB = AABBz;
		if (AABBz < minDistanceZAABB) minDistanceZAABB = AABBz;

		if (x > maxDistanceX) maxDistanceX = x;
		if (x < minDistanceX) minDistanceX = x;
		if (y > maxDistanceY) maxDistanceY = y;
		if (y < minDistanceY) minDistanceY = y;
		if (z > maxDistanceZ) maxDistanceZ = z;
		if (z < minDistanceZ) minDistanceZ = z;
	}
	this->axisAlignedBoundingBox.halfWidth = (maxDistanceXAABB - minDistanceXAABB) * 0.5f;
	this->axisAlignedBoundingBox.halfHeight =(maxDistanceYAABB - minDistanceYAABB) * 0.5f;
	this->axisAlignedBoundingBox.halfDepth = (maxDistanceZAABB - minDistanceZAABB) * 0.5f;

	this->orientedBoundingBox.halfWidth = (maxDistanceX - minDistanceX) * 0.5f;
	this->orientedBoundingBox.halfHeight =(maxDistanceY - minDistanceY) * 0.5f;
	this->orientedBoundingBox.halfDepth = (maxDistanceZ - minDistanceZ) * 0.5f;

	this->boundingSphere.volume = (4.0f / 3.0f) * XM_PI * std::powf(this->boundingSphere.radius, 3.0f);
	this->axisAlignedBoundingBox.volume = axisAlignedBoundingBox.halfWidth * axisAlignedBoundingBox.halfHeight * axisAlignedBoundingBox.halfDepth * 8;
	this->orientedBoundingBox.volume = orientedBoundingBox.halfWidth * orientedBoundingBox.halfHeight * orientedBoundingBox.halfDepth * 8;
	//if (axisAlignedBoundingBox.volume < boundingSphere.volume)
	//	boundingShape = BoundingShape::AABB;
	//else
	//	boundingShape = BoundingShape::SPHERE;

	if (orientedBoundingBox.volume <= axisAlignedBoundingBox.volume && orientedBoundingBox.volume <= boundingSphere.volume)
	{
		boundingShape = BoundingShape::OBB;
	}
	else if (axisAlignedBoundingBox.volume <= boundingSphere.volume && axisAlignedBoundingBox.volume <= orientedBoundingBox.volume)
	{
		boundingShape = BoundingShape::AABB;
	}
	else
	{
		boundingShape = BoundingShape::SPHERE;
	}
}

Model::BoundingShape Model::GetBoundingShape()
{
	return this->boundingShape;
}

XMVECTOR Model::GetOriginVector()
{
	return this->originVector;
}

float Model::GetScaledBoundingSphereRadius()
{
	return this->boundingSphere.scaledRadius;
}

void Model::SetScaledBoundingSphereRadius(float scale)
{
	this->boundingSphere.scaledRadius = boundingSphere.radius * scale;
}

float Model::GetBoundingSphereRadius()
{
	return boundingSphere.radius;
}

float Model::GetVolume()
{
	if (this->boundingShape == BoundingShape::SPHERE)
	{
		return boundingSphere.volume;
	}
	else if (this->boundingShape == BoundingShape::AABB)
	{
		return axisAlignedBoundingBox.volume;
	}
	else
	{
		return orientedBoundingBox.volume;
	}
}

Model::AABB Model::GetAABB()
{
	return this->axisAlignedBoundingBox;
}
Model::OBB Model::GetOBB()
{
	return this->orientedBoundingBox;
}

float Model::GetWidth(XMMATRIX rotationMatrix)
{
	if (this->boundingShape == BoundingShape::SPHERE)
	{
		return boundingSphere.radius * 2;
	}
	else if (this->boundingShape == BoundingShape::AABB)
	{
		return axisAlignedBoundingBox.halfWidth * 2;
	}
	else
	{
		return orientedBoundingBox.halfWidth * 2;
		//XMVECTOR points[8] = {};
		//points[0] = XMVector3Transform(XMVectorSet(-orientedBoundingBox.halfWidth, -orientedBoundingBox.halfHeight, -orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//points[1] = XMVector3Transform(XMVectorSet(-orientedBoundingBox.halfWidth, -orientedBoundingBox.halfHeight, orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//points[2] = XMVector3Transform(XMVectorSet(orientedBoundingBox.halfWidth, -orientedBoundingBox.halfHeight, -orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//points[3] = XMVector3Transform(XMVectorSet(orientedBoundingBox.halfWidth, -orientedBoundingBox.halfHeight, orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//points[4] = XMVector3Transform(XMVectorSet(-orientedBoundingBox.halfWidth, orientedBoundingBox.halfHeight, -orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//points[5] = XMVector3Transform(XMVectorSet(-orientedBoundingBox.halfWidth, orientedBoundingBox.halfHeight, orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//points[6] = XMVector3Transform(XMVectorSet(orientedBoundingBox.halfWidth, orientedBoundingBox.halfHeight, -orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//points[7] = XMVector3Transform(XMVectorSet(orientedBoundingBox.halfWidth, orientedBoundingBox.halfHeight, orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//
		//XMVECTOR lowestVector = points[0];
		//XMVECTOR highestVector = points[0];
		//for (int i = 1; i < 8; ++i)
		//{
		//	if (XMVectorGetX(points[i]) < XMVectorGetX(lowestVector))
		//	{
		//		lowestVector = points[i];
		//	}
		//	else if (XMVectorGetX(points[i]) > XMVectorGetX(highestVector))
		//	{
		//		highestVector = points[i];
		//	}
		//}
		//return XMVectorGetX(highestVector - lowestVector);
	}
}

float Model::GetHeight(XMMATRIX rotationMatrix)
{
	if (this->boundingShape == BoundingShape::SPHERE)
	{
		return boundingSphere.radius * 2;
	}
	else if (this->boundingShape == BoundingShape::AABB)
	{
		return axisAlignedBoundingBox.halfHeight * 2;
	}
	else
	{
		return orientedBoundingBox.halfHeight * 2;
		//XMVECTOR points[8] = {};
		//points[0] = XMVector3Transform(XMVectorSet(-orientedBoundingBox.halfWidth, -orientedBoundingBox.halfHeight, -orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//points[1] = XMVector3Transform(XMVectorSet(-orientedBoundingBox.halfWidth, -orientedBoundingBox.halfHeight, orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//points[2] = XMVector3Transform(XMVectorSet(orientedBoundingBox.halfWidth, -orientedBoundingBox.halfHeight, -orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//points[3] = XMVector3Transform(XMVectorSet(orientedBoundingBox.halfWidth, -orientedBoundingBox.halfHeight, orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//points[4] = XMVector3Transform(XMVectorSet(-orientedBoundingBox.halfWidth, orientedBoundingBox.halfHeight, -orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//points[5] = XMVector3Transform(XMVectorSet(-orientedBoundingBox.halfWidth, orientedBoundingBox.halfHeight, orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//points[6] = XMVector3Transform(XMVectorSet(orientedBoundingBox.halfWidth, orientedBoundingBox.halfHeight, -orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//points[7] = XMVector3Transform(XMVectorSet(orientedBoundingBox.halfWidth, orientedBoundingBox.halfHeight, orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//
		//XMVECTOR lowestVector = points[0];
		//XMVECTOR highestVector = points[0];
		//for (int i = 1; i < 8; ++i)
		//{
		//	if (XMVectorGetY(points[i]) < XMVectorGetY(lowestVector))
		//	{
		//		lowestVector = points[i];
		//	}
		//	else if (XMVectorGetY(points[i]) > XMVectorGetY(highestVector))
		//	{
		//		highestVector = points[i];
		//	}
		//}
		//return XMVectorGetY(highestVector - lowestVector);
	}
}

float Model::GetDepth(XMMATRIX rotationMatrix)
{
	if (this->boundingShape == BoundingShape::SPHERE)
	{
		return boundingSphere.radius * 2;
	}
	else if (this->boundingShape == BoundingShape::AABB)
	{
		return axisAlignedBoundingBox.halfDepth * 2;
	}
	else
	{
		return orientedBoundingBox.halfDepth * 2;
		//XMVECTOR points[8] = {};
		//points[0] = XMVector3Transform(XMVectorSet(-orientedBoundingBox.halfWidth, -orientedBoundingBox.halfHeight, -orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//points[1] = XMVector3Transform(XMVectorSet(-orientedBoundingBox.halfWidth, -orientedBoundingBox.halfHeight, orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//points[2] = XMVector3Transform(XMVectorSet(orientedBoundingBox.halfWidth, -orientedBoundingBox.halfHeight, -orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//points[3] = XMVector3Transform(XMVectorSet(orientedBoundingBox.halfWidth, -orientedBoundingBox.halfHeight, orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//points[4] = XMVector3Transform(XMVectorSet(-orientedBoundingBox.halfWidth, orientedBoundingBox.halfHeight, -orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//points[5] = XMVector3Transform(XMVectorSet(-orientedBoundingBox.halfWidth, orientedBoundingBox.halfHeight, orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//points[6] = XMVector3Transform(XMVectorSet(orientedBoundingBox.halfWidth, orientedBoundingBox.halfHeight, -orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//points[7] = XMVector3Transform(XMVectorSet(orientedBoundingBox.halfWidth, orientedBoundingBox.halfHeight, orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		//
		//XMVECTOR lowestVector = points[0];
		//XMVECTOR highestVector = points[0];
		//for (int i = 1; i < 8; ++i)
		//{
		//	if (XMVectorGetZ(points[i]) < XMVectorGetZ(lowestVector))
		//	{
		//		lowestVector = points[i];
		//	}
		//	else if (XMVectorGetZ(points[i]) > XMVectorGetZ(highestVector))
		//	{
		//		highestVector = points[i];
		//	}
		//}
		//return XMVectorGetZ(highestVector - lowestVector);
	}
}

XMVECTOR Model::GetMinimumCoord(XMMATRIX rotationMatrix)
{
	if (boundingShape == BoundingShape::SPHERE)
	{
		return XMVectorSet(0.0f, -boundingSphere.radius, 0.0f, 0.0f);
	}
	else if (boundingShape == BoundingShape::AABB)
	{
		return XMVectorSet(0.0f, -axisAlignedBoundingBox.halfHeight, 0.0f, 0.0f);
	}
	else
	{
		XMVECTOR points[8] = {};
		points[0] = XMVector3Transform(XMVectorSet(-orientedBoundingBox.halfWidth, -orientedBoundingBox.halfHeight, -orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		points[1] = XMVector3Transform(XMVectorSet(-orientedBoundingBox.halfWidth, -orientedBoundingBox.halfHeight,  orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		points[2] = XMVector3Transform(XMVectorSet( orientedBoundingBox.halfWidth, -orientedBoundingBox.halfHeight, -orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		points[3] = XMVector3Transform(XMVectorSet( orientedBoundingBox.halfWidth, -orientedBoundingBox.halfHeight,  orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		points[4] = XMVector3Transform(XMVectorSet(-orientedBoundingBox.halfWidth,  orientedBoundingBox.halfHeight, -orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		points[5] = XMVector3Transform(XMVectorSet(-orientedBoundingBox.halfWidth,  orientedBoundingBox.halfHeight,  orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		points[6] = XMVector3Transform(XMVectorSet( orientedBoundingBox.halfWidth,  orientedBoundingBox.halfHeight, -orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		points[7] = XMVector3Transform(XMVectorSet( orientedBoundingBox.halfWidth,  orientedBoundingBox.halfHeight,  orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);

		XMVECTOR lowestVector = points[0];
		for (int i = 1; i < 8; ++i)
		{
			if (XMVectorGetY(points[i]) < XMVectorGetY(lowestVector))
			{
				lowestVector = points[i];
			}
		}
		return lowestVector;
	}
}

XMVECTOR Model::GetMaximumCoord(XMMATRIX rotationMatrix)
{
	if (boundingShape == BoundingShape::SPHERE)
	{
		return XMVectorSet(0.0f, boundingSphere.radius, 0.0f, 0.0f);
	}
	else if (boundingShape == BoundingShape::AABB)
	{
		return XMVectorSet(0.0f, axisAlignedBoundingBox.halfHeight, 0.0f, 0.0f);
	}
	else
	{
		XMVECTOR points[8] = {};
		points[0] = XMVector3Transform(XMVectorSet(-orientedBoundingBox.halfWidth, -orientedBoundingBox.halfHeight, -orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		points[1] = XMVector3Transform(XMVectorSet(-orientedBoundingBox.halfWidth, -orientedBoundingBox.halfHeight, orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		points[2] = XMVector3Transform(XMVectorSet(orientedBoundingBox.halfWidth, -orientedBoundingBox.halfHeight, -orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		points[3] = XMVector3Transform(XMVectorSet(orientedBoundingBox.halfWidth, -orientedBoundingBox.halfHeight, orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		points[4] = XMVector3Transform(XMVectorSet(-orientedBoundingBox.halfWidth, orientedBoundingBox.halfHeight, -orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		points[5] = XMVector3Transform(XMVectorSet(-orientedBoundingBox.halfWidth, orientedBoundingBox.halfHeight, orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		points[6] = XMVector3Transform(XMVectorSet(orientedBoundingBox.halfWidth, orientedBoundingBox.halfHeight, -orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);
		points[7] = XMVector3Transform(XMVectorSet(orientedBoundingBox.halfWidth, orientedBoundingBox.halfHeight, orientedBoundingBox.halfDepth, 0.0f), rotationMatrix);

		XMVECTOR highestVector = points[0];
		for (int i = 1; i < 8; ++i)
		{
			if (XMVectorGetY(points[i]) > XMVectorGetY(highestVector))
			{
				highestVector = points[i];
			}
		}
		return highestVector;
	}
}

std::vector<Mesh*> Model::GetMeshes()
{
	return this->meshes;
}

void Model::Draw(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix)
{
	//Update & Set CBuffers
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vertexShader->GetAddressOf());
	this->deviceContext->PSSetConstantBuffers(0, 1, this->cb_pixelShader->GetAddressOf());
	
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		this->cb_vertexShader->data.wvpMatrix = meshes[i]->GetTransformMatrix() * XMMatrixTranslationFromVector(-originVector) * worldMatrix * viewProjectionMatrix;
		this->cb_vertexShader->data.worldMatrix = meshes[i]->GetTransformMatrix() * XMMatrixTranslationFromVector(-originVector) * worldMatrix;
		this->cb_vertexShader->ApplyChanges();
		this->cb_pixelShader->ApplyChanges();
		meshes[i]->Draw();
	}
}

void Model::DrawDebug(XMVECTOR position, XMMATRIX rotationMatrix, float scale, const XMMATRIX& viewProjectionMatrix, Model* sphere, Model* box)
{
	if (boundingShape == BoundingShape::SPHERE)
	{
		SetScaledBoundingSphereRadius(scale);
		XMMATRIX newWorld = XMMatrixScaling(boundingSphere.scaledRadius, boundingSphere.scaledRadius, boundingSphere.scaledRadius) * XMMatrixTranslationFromVector(position);
		sphere->Draw(newWorld, viewProjectionMatrix);
	}
	else if (boundingShape == BoundingShape::AABB)
	{
		XMMATRIX newWorld = XMMatrixScaling(axisAlignedBoundingBox.halfWidth * 2 *scale, axisAlignedBoundingBox.halfHeight * 2 *scale, axisAlignedBoundingBox.halfDepth * 2 * scale) * XMMatrixTranslationFromVector(position);
		box->Draw(newWorld, viewProjectionMatrix);
	}
	else
	{
		XMMATRIX newWorld = XMMatrixScaling(orientedBoundingBox.halfWidth * 2 * scale, orientedBoundingBox.halfHeight * 2 * scale, orientedBoundingBox.halfDepth * 2 * scale) * rotationMatrix * XMMatrixTranslationFromVector(position);
		box->Draw(newWorld, viewProjectionMatrix);
	}
}


bool Model::LoadModel(const std::string& filepath)
{
	this->directory = StringTools::GetDirectoryFromPath(filepath);

	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_ConvertToLeftHanded | aiProcess_JoinIdenticalVertices);
	
	if (pScene == NULL)
		return false;

	this->ProcessNode(pScene->mRootNode, pScene, DirectX::XMMatrixIdentity());
	SetOrigin();
	return true;
}

void Model::ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix)
{
	XMMATRIX nodeTransformMatrix = XMMatrixTranspose(static_cast<XMMATRIX>(&node->mTransformation.a1))* parentTransformMatrix;

	for (UINT i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(this->ProcessMesh(mesh, scene, nodeTransformMatrix));
	}

	for (UINT i = 0; i < node->mNumChildren; ++i)
	{
		this->ProcessNode(node->mChildren[i], scene, nodeTransformMatrix);
	}
}

Mesh* Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix)
{
	std::vector<Texture> textures;
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	std::vector<Texture> diffuseTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, scene);
	textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());
	std::vector<Texture> specularTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_SPECULAR, scene);
	textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());
	std::vector<Texture> normalTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_HEIGHT, scene);
	textures.insert(textures.end(), normalTextures.begin(), normalTextures.end());

	return new Mesh(this->device, this->deviceContext, mesh, textures, transformMatrix, this->directory, &vertices);
}


std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene)
{
	std::vector<Texture> materialTextures;
	TextureStorageType storeType = TextureStorageType::INVALID;
	unsigned int textureCount = pMaterial->GetTextureCount(textureType);

	if (textureCount == 0)
	{
		storeType = TextureStorageType::NONE;
		aiColor3D aiColor(0.0f, 0.0f, 0.0f);
		switch (textureType)
		{
		case aiTextureType_DIFFUSE:
			pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
			if (aiColor.IsBlack())
			{
				materialTextures.push_back(Texture(this->device, Colors::UnloadedTextureColor, textureType));
				return materialTextures;
			}
			materialTextures.push_back(Texture(this->device, Color(static_cast<BYTE>(aiColor.r * 255), static_cast<BYTE>(aiColor.g * 255), static_cast<BYTE>(aiColor.b * 255)), textureType));
			return materialTextures;

		case aiTextureType_SPECULAR:
			materialTextures.push_back(Texture(this->device, Color(255, 255, 255), textureType));
			return materialTextures;

		case aiTextureType_HEIGHT:
			materialTextures.push_back(Texture(this->device, Color(50, 100, 200), textureType));
			return materialTextures;
		}
	}
	else
	{
		for (UINT i = 0; i < textureCount; ++i)
		{
			aiString path;
			pMaterial->GetTexture(textureType, i, &path);
			TextureStorageType storeType = DetermineTextureStorageType(pScene, pMaterial, i, textureType);
			switch (storeType)
			{
			case TextureStorageType::DISK:
			{
				std::string filename = this->directory + '\\' + path.C_Str();
				Texture diskTexture(this->device, filename, textureType);
				materialTextures.push_back(diskTexture);
				break;
			}
			case TextureStorageType::EMBEDDED_COMPRESSED:
			{
				const aiTexture* pTexture = pScene->GetEmbeddedTexture(path.C_Str());
				Texture embeddedTexture(this->device, reinterpret_cast<uint8_t*>(pTexture->pcData), pTexture->mWidth, textureType);
				materialTextures.push_back(embeddedTexture);
				break;
			}
			case TextureStorageType::EMBEDDED_INDEX_COMPRESSED:
			{
				int index = GetTextureIndex(&path);
				Texture embeddedIndexedTexture(this->device, reinterpret_cast<uint8_t*>(pScene->mTextures[index]->pcData), pScene->mTextures[index]->mWidth, textureType);
				materialTextures.push_back(embeddedIndexedTexture);
				break;
			}
			}
		}
	}
	if (materialTextures.size() == 0)
	{
		materialTextures.push_back(Texture(this->device, Colors::UnhandledTextureColor, aiTextureType::aiTextureType_DIFFUSE));
		materialTextures.push_back(Texture(this->device, Colors::NoSpecularColor, aiTextureType::aiTextureType_SPECULAR));
		materialTextures.push_back(Texture(this->device, Colors::NoSpecularColor, aiTextureType::aiTextureType_HEIGHT));
	}
	return materialTextures;
}

TextureStorageType Model::DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMaterial, unsigned int index, aiTextureType textureType)
{
	if (pMaterial->GetTextureCount(textureType) == 0)
		return TextureStorageType::NONE;

	aiString path;
	pMaterial->GetTexture(textureType, index, &path);
	std::string texturePath = path.C_Str();

	if (texturePath[0] == '*')
	{
		if (pScene->mTextures[0]->mHeight == 0)
			return TextureStorageType::EMBEDDED_INDEX_COMPRESSED;
		else
		{
			assert("SUPPORT DOES NOT YET EXIST FOR EMBEDDED INDEXED NON COMPRESSED TEXTURES!" && 0);
			return TextureStorageType::EMBEDDED_INDEX_NON_COMPRESSED;
		}
	}
	if (auto pTex = pScene->GetEmbeddedTexture(texturePath.c_str()))
	{
		if (pTex->mHeight == 0)
			return TextureStorageType::EMBEDDED_COMPRESSED;
		else
		{
			assert("SUPPORT DOES NOT YET EXIST FOR EMBEDDED NON COMPRESSED TEXTURES!" && 0);
			return TextureStorageType::EMBEDDED_NON_COMPRESSED;
		}
	}
	if (texturePath.find('.') != std::string::npos)
	{
		return TextureStorageType::DISK;
	}

	return TextureStorageType::NONE;
}

int Model::GetTextureIndex(aiString* pStr)
{
	assert(pStr->length >= 2);
	return atoi(&pStr->C_Str()[1]);
}