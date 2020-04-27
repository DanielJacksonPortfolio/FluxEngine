#pragma once
#include "Mesh.h"

class Model
{
public:
	enum class BoundingShape
	{
		SPHERE,
		AABB,
		OBB
	};

	struct AABB
	{
		float volume = 0.0f;
		float halfWidth = 0.0f;
		float halfHeight = 0.0f;
		float halfDepth = 0.0f;
	};		 
	struct OBB
	{
		float volume = 0.0f;
		float halfWidth = 0.0f;
		float halfHeight = 0.0f;
		float halfDepth = 0.0f;
	};
	struct BoundingSphere
	{
		float volume = 0.0f;
		float radius = 0.0f;
		float scaledRadius = 0.0f;
	};

	Model() {}
	bool Init(const std::string& filepath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader> & cb_vertexShader, ConstantBuffer<CB_pixelShader>& cb_pixelShader);
	void Draw(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix);
	void DrawDebug(XMVECTOR position, XMMATRIX rotationMatrix, float scale, const XMMATRIX& viewProjectionMatrix, Model* sphere, Model* box);
	Model(const Model& model);
	void SetBoundingShape(XMMATRIX rotation);
	BoundingShape GetBoundingShape();
	XMVECTOR GetOriginVector();
	float GetScaledBoundingSphereRadius();
	void SetScaledBoundingSphereRadius(float scale);
	float GetBoundingSphereRadius();
	float GetVolume();
	Model::AABB GetAABB();
	Model::OBB GetOBB();
	float GetWidth( XMMATRIX rotationMatrix = XMMATRIX());
	float GetHeight(XMMATRIX rotationMatrix = XMMATRIX());
	float GetDepth( XMMATRIX rotationMatrix = XMMATRIX());
	XMVECTOR GetMinimumCoord(XMMATRIX rotationMatrix = XMMATRIX());
	XMVECTOR GetMaximumCoord(XMMATRIX rotationMatrix = XMMATRIX());
	std::vector<Mesh*> GetMeshes();

private:
	void SetOrigin();
	bool LoadModel(const std::string& filepath);
	void ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix);
	Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);
	TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMaterial, unsigned int index, aiTextureType textureType);
	int GetTextureIndex(aiString* pStr);

	std::vector<Vertex_PosTexNormTan> vertices;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	ConstantBuffer<CB_vertexShader> * cb_vertexShader = nullptr;
	ConstantBuffer<CB_pixelShader> * cb_pixelShader = nullptr;
	std::string directory = "";
	BoundingShape boundingShape = BoundingShape::SPHERE;
	XMVECTOR originVector = XMVECTOR();
	//float scaledBoundingSphereRadius = 1.0f;
	//float boundingSphereRadius = 1.0f;
	AABB axisAlignedBoundingBox;
	OBB orientedBoundingBox;
	BoundingSphere boundingSphere;
	//XMVECTOR minAABBCoord = XMVECTOR();
	//XMVECTOR maxAABBCoord = XMVECTOR();
	//float aabbw = 1.0f;
	//float aabbh = 1.0f;
	//float aabbd = 1.0f;
	//float aabbVolume = 0.0f;
	//float sphereVolume = 0.0f;
	std::vector<Mesh*> meshes;
};

