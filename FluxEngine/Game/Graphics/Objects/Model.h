#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>

#include "../Shaders+Buffers/VertexBuffer.h"
#include "../Shaders+Buffers/IndexBuffer.h"
#include "../Shaders+Buffers/ConstantBuffer.h"

using namespace DirectX;

class Model
{
public:
	bool Init(const std::string& filepath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_vertexShader_PosTex> & cb_vertexShader);
	void Draw(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix);

private:
	//std::vector<Mesh> meshes;
	//bool LoadModel(const std::string& filepath);
	//void ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix);
	//Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix);
	//std::vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);
	//TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMaterial, unsigned int index, aiTextureType textureType);
	//int GetTextureIndex(aiString* pStr);


	VertexBuffer<Vertex_PosTex> vertexBuffer;
	IndexBuffer indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Resource> texture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView = nullptr;

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	ConstantBuffer<CB_vertexShader_PosTex> * cb_vertexShader = nullptr;
	std::string directory = "";
};

