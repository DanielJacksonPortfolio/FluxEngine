#pragma once
#include "Mesh.h"

class Model
{
public:
	enum class BoundingShape
	{
		SPHERE,
		AABB
	};

	Model() {}
	bool Init(const std::string& filepath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader> & cb_vertexShader, ConstantBuffer<CB_pixelShader>& cb_pixelShader);
	void Draw(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix);
	void DrawDebug(XMVECTOR position, float scale, const XMMATRIX& viewProjectionMatrix, Model* sphere, Model* box);
	Model(const Model& model);
	bool RayModelIntersect(XMMATRIX worldMatrix, XMVECTOR position, float scale, XMVECTOR rayOrigin, XMVECTOR rayDir, float& nearestIntersect, XMVECTOR& intersectLocation);
	void SetBoundingShape(XMMATRIX rotation);

	XMVECTOR originVector = XMVECTOR();
	float objectBoundingSphereRadius = 1.0f;
	XMVECTOR minAABBCoord = XMVECTOR();
	XMVECTOR maxAABBCoord = XMVECTOR();
	float aabbw = 1.0f;
	float aabbh = 1.0f;
	float aabbd = 1.0f;
private:
	void SetOrigin();
	bool RaySphereIntersect(XMVECTOR position, XMVECTOR rayOrigin, XMVECTOR rayDir);
	bool RayAABBIntersect(float scale, XMVECTOR position, XMVECTOR rayOrigin, XMVECTOR rayDir);
	bool LoadModel(const std::string& filepath);
	void ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix);
	Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);
	TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMaterial, unsigned int index, aiTextureType textureType);
	int GetTextureIndex(aiString* pStr);

	std::vector<Mesh*> meshes;
	std::vector<Vertex_PosTexNormTan> vertices;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	ConstantBuffer<CB_vertexShader> * cb_vertexShader = nullptr;
	ConstantBuffer<CB_pixelShader> * cb_pixelShader = nullptr;
	std::string directory = "";
	float modelBoundingSphereRadius = 1.0f;
	BoundingShape boundingShape = BoundingShape::SPHERE;
};

