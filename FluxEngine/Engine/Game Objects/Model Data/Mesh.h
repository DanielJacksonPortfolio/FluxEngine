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
	Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float scale, aiMesh* mesh, std::vector<Texture>& textures, const XMMATRIX& transformMatrix, std::string directory);
	Mesh(const Mesh& mesh);
	void Draw();
	const XMMATRIX& GetTransformMatrix();
	bool RayMeshIntersect(XMFLOAT3 rayOrigin, XMFLOAT3 rayDir, float* nearestIntersect);
private:
	bool RayTriangleIntersect(XMFLOAT3 rayOrigin, XMFLOAT3 rayDir, float* triangleIntersect, int faceIndex);

	VertexBuffer<Vertex_PosTexNormTanBinorm> vertexBuffer;
	IndexBuffer indexBuffer;
	ID3D11DeviceContext* deviceContext;
	XMMATRIX transformMatrix;
	std::vector<Texture> textures;
	aiMesh* mesh;

	std::string directory;
};

