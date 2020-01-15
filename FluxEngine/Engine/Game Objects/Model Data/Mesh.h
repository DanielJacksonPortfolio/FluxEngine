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
	struct Face
	{
		std::vector<XMFLOAT3> vertices;
	};

	Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float scale, aiMesh* mesh, std::vector<Texture>& textures, const XMMATRIX& transformMatrix, std::string directory);
	Mesh(const Mesh& mesh);
	void Draw(const XMMATRIX& worldMatrix);
	const XMMATRIX& GetTransformMatrix();
	bool RayMeshIntersect(XMVECTOR rayOrigin, XMVECTOR rayDir, float* nearestIntersect);
private:
	bool RayTriangleIntersect(XMVECTOR rayOrigin, XMVECTOR rayDir, float* intersectDistance, int faceIndex);
	bool RaySphereIntersect(XMVECTOR rayOrigin, XMVECTOR rayDir, float* intersectDistance);

	VertexBuffer<Vertex_PosTexNormTanBinorm> vertexBuffer;
	IndexBuffer indexBuffer;
	ID3D11DeviceContext* deviceContext;
	XMMATRIX transformMatrix;
	XMMATRIX worldMatrix;
	std::vector<Texture> textures;
	aiMesh* mesh = nullptr;
	std::vector<Face*> faces = {};
	float boundingSphereRadius = 0.0f;
	std::string directory;
};

