#pragma once
#include "Texture.h"

#include "../../Buffers and Shaders/VertexBuffer.h"
#include "../../Buffers and Shaders/IndexBuffer.h"	
#include "../../Buffers and Shaders/ConstantBuffer.h"
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

using namespace DirectX;

class Mesh
{
public:

	Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, aiMesh* mesh, std::vector<Texture>& textures, const XMMATRIX& transformMatrix, std::string directory, std::vector<Vertex_PosTexNormTan>* modelVertices);
	Mesh(const Mesh& mesh);
	void Draw();
	const XMMATRIX& GetTransformMatrix();
	size_t GetNumFaces() { return faces.size(); }
	std::vector<aiFace> faces = {};
	XMMATRIX transformMatrix;
	std::vector<Vertex_PosTexNormTan> vertices;
private:
	VertexBuffer<Vertex_PosTexNormTan> vertexBuffer;
	std::vector<DWORD> indices;
	IndexBuffer indexBuffer;
	ID3D11DeviceContext* deviceContext;
	std::vector<Texture> textures;
	aiMesh* mesh = nullptr;
	std::string directory;

};

