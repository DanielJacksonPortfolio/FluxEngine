#pragma once
#include "Texture.h"

#include "../Shaders+Buffers/VertexBuffer.h"
#include "../Shaders+Buffers/IndexBuffer.h"	
#include "../Shaders+Buffers/ConstantBuffer.h"
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

using namespace DirectX;

class Mesh
{
public:
	Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float scale, aiMesh* mesh, std::vector<Texture>& textures, const XMMATRIX& transformMatrix, std::string directory);
	Mesh(const Mesh& mesh);
	void Draw();
	const XMMATRIX& GetTransformMatrix();
private:
	VertexBuffer<Vertex_PosTexNormTanBinorm> vertexBuffer;
	IndexBuffer indexBuffer;
	ID3D11DeviceContext* deviceContext;
	XMMATRIX transformMatrix;
	std::vector<Texture> textures;
	aiMesh* mesh;

	std::string directory;

	//Microsoft::WRL::ComPtr<ID3D11Resource> diffuseMap = nullptr;
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseView = nullptr;
};

