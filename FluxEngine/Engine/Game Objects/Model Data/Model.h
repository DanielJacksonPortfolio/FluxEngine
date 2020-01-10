#pragma once
#include "Mesh.h"

class Model
{
public:
	Model() {}
	bool Init(const std::string& filepath, float scale, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader> & cb_vertexShader, ConstantBuffer<CB_pixelShader>& cb_pixelShader);
	void Draw(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix);
	std::vector<Mesh*> GetMeshes() { return meshes; }
	Model(const Model& model);
private:
	bool LoadModel(const std::string& filepath);
	void ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix);
	Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);
	TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMaterial, unsigned int index, aiTextureType textureType);
	int GetTextureIndex(aiString* pStr);

	std::vector<Mesh*> meshes;
	float scale = 1.0f;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	ConstantBuffer<CB_vertexShader> * cb_vertexShader = nullptr;
	ConstantBuffer<CB_pixelShader> * cb_pixelShader = nullptr;
	std::string directory = "";
};

