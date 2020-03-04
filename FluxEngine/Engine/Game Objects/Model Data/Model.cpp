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
	this->modelBoundingSphereRadius = 0.0f;
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
	modelBoundingSphereRadius = maxDistance;

	float sphereVolume = (4.0f / 3.0f) * XM_PI * std::powf(modelBoundingSphereRadius, 3.0f);
	
	this->aabbw = 0.0f;
	this->aabbh = 0.0f;
	this->aabbd = 0.0f;
	float minDistanceX = 0.0f;
	float maxDistanceX = 0.0f;
	float minDistanceY = 0.0f;
	float maxDistanceY = 0.0f;
	float minDistanceZ = 0.0f;
	float maxDistanceZ = 0.0f;
	for (int i = 0; i < vertices.size(); ++i)
	{
		
		float x = vertices[i].pos.x - XMVectorGetX(this->originVector);
		float y = vertices[i].pos.y - XMVectorGetY(this->originVector);
		float z = vertices[i].pos.z - XMVectorGetZ(this->originVector);

		XMVECTOR xyz = XMVector3Transform(XMVectorSet(x, y, z, 0), rotation);
		x = XMVectorGetX(xyz);
		y = XMVectorGetY(xyz);
		z = XMVectorGetZ(xyz);

		if (x > maxDistanceX) maxDistanceX = x;
		if (x < minDistanceX) minDistanceX = x;
		if (y > maxDistanceY) maxDistanceY = y;
		if (y < minDistanceY) minDistanceY = y;
		if (z > maxDistanceZ) maxDistanceZ = z;
		if (z < minDistanceZ) minDistanceZ = z;
	}
	aabbw = maxDistanceX - minDistanceX;
	aabbh = maxDistanceY - minDistanceY;
	aabbd = maxDistanceZ - minDistanceZ;

	this->minAABBCoord = XMVectorSet(minDistanceX, minDistanceY, minDistanceZ, 0);
	this->maxAABBCoord = XMVectorSet(maxDistanceX, maxDistanceY, maxDistanceZ, 0);

	float aabbVolume = aabbw * aabbh * aabbd;

	if (aabbVolume < sphereVolume)
		boundingShape = BoundingShape::AABB;
	else
		boundingShape = BoundingShape::SPHERE;
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

void Model::DrawDebug(XMVECTOR position, float scale, const XMMATRIX& viewProjectionMatrix, Model* sphere, Model* box)
{
	if (boundingShape == BoundingShape::SPHERE)
	{
		objectBoundingSphereRadius = modelBoundingSphereRadius * scale;
		XMMATRIX newWorld = XMMatrixScaling(objectBoundingSphereRadius, objectBoundingSphereRadius, objectBoundingSphereRadius) * XMMatrixTranslationFromVector(position);
		sphere->Draw(newWorld, viewProjectionMatrix);
	}
	else
	{
		XMMATRIX newWorld = XMMatrixScaling(aabbw*scale, aabbh*scale, aabbd * scale) * XMMatrixTranslationFromVector(position);
		box->Draw(newWorld, viewProjectionMatrix);
	}
}

bool Model::RayModelIntersect(XMMATRIX worldMatrix, XMVECTOR position, float scale, XMVECTOR rayOrigin, XMVECTOR rayDir, float& nearestIntersect, XMVECTOR& intersectLocation)
{
	bool initialCheck = false;
	if (boundingShape == BoundingShape::SPHERE)
	{
		objectBoundingSphereRadius = modelBoundingSphereRadius * scale;
		if (RaySphereIntersect(position, rayOrigin, rayDir))
			initialCheck = true;
	}
	else
	{
		if (RayAABBIntersect(scale,position, rayOrigin, rayDir))
			initialCheck = true;
	}
	if(initialCheck)
	{
		bool intersect = false;
		for (int i = 0; i < meshes.size(); ++i)
		{
			float intersectDistance = INT_MAX;
			if (meshes[i]->RayMeshIntersect(XMMatrixTranslationFromVector(-originVector) * worldMatrix, rayOrigin, rayDir, intersectDistance, intersectLocation))
			{
				intersect = true;
				if (nearestIntersect > intersectDistance)
					nearestIntersect = intersectDistance;
			}
		}
		if (intersect)
			return true;
	}
	return false;
}

bool Model::RaySphereIntersect(XMVECTOR position, XMVECTOR rayOrigin, XMVECTOR rayDir)
{
	XMVECTOR oc = rayOrigin - position;
	float a = XMVectorGetX(XMVector3Dot(rayDir, rayDir));
	float b = 2 * XMVectorGetX(XMVector3Dot(oc, rayDir));
	float c = XMVectorGetX(XMVector3Dot(oc, oc)) - (objectBoundingSphereRadius * objectBoundingSphereRadius);
	return 0 < ((b * b) - (4 * a * c));

}

bool Model::RayAABBIntersect(float scale, XMVECTOR position, XMVECTOR rayOrigin, XMVECTOR rayDir)
{
	XMVECTOR dirfrac = XMVectorSet(1 / XMVectorGetX(rayDir), 1 / XMVectorGetY(rayDir), 1 / XMVectorGetZ(rayDir), 0);

	float t1 = (XMVectorGetX(scale*minAABBCoord+position) - XMVectorGetX(rayOrigin)) * XMVectorGetX(dirfrac);
	float t2 = (XMVectorGetX(scale*maxAABBCoord+position) - XMVectorGetX(rayOrigin)) * XMVectorGetX(dirfrac);
	float t3 = (XMVectorGetY(scale*minAABBCoord+position) - XMVectorGetY(rayOrigin)) * XMVectorGetY(dirfrac);
	float t4 = (XMVectorGetY(scale*maxAABBCoord+position) - XMVectorGetY(rayOrigin)) * XMVectorGetY(dirfrac);
	float t5 = (XMVectorGetZ(scale*minAABBCoord+position) - XMVectorGetZ(rayOrigin)) * XMVectorGetZ(dirfrac);
	float t6 = (XMVectorGetZ(scale*maxAABBCoord+position) - XMVectorGetZ(rayOrigin)) * XMVectorGetZ(dirfrac);

	float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	if (tmax < 0 || tmin > tmax)
		return false;
	return true;
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