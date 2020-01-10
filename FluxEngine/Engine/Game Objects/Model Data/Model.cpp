#include "Model.h"

bool Model::Init(const std::string& filepath, float scale, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader>& cb_vertexShader, ConstantBuffer<CB_pixelShader>& cb_pixelShader)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->cb_vertexShader = &cb_vertexShader;
	this->cb_pixelShader = &cb_pixelShader;
	this->scale = scale;
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
	this->scale = model.scale;
	this->device = model.device;
	this->deviceContext = model.deviceContext;
	this->cb_vertexShader = model.cb_vertexShader;
	this->cb_pixelShader = model.cb_pixelShader;
	this->directory = model.directory;
}


void Model::Draw(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix)
{
	//Update & Set CBuffers

	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vertexShader->GetAddressOf());
	this->deviceContext->PSSetConstantBuffers(0, 1, this->cb_pixelShader->GetAddressOf());

	for (size_t i = 0; i < meshes.size(); ++i)
	{
		this->cb_vertexShader->data.wvpMatrix = worldMatrix * viewProjectionMatrix;
		this->cb_vertexShader->data.worldMatrix = worldMatrix;
		this->cb_vertexShader->ApplyChanges();
		this->cb_pixelShader->ApplyChanges();
		meshes[i]->Draw();
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

	return new Mesh(this->device, this->deviceContext, scale, mesh, textures, transformMatrix, this->directory);
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
