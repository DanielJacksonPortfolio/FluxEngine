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
	bool RayTriangleIntersect(XMMATRIX worldMatrix, XMVECTOR rayOrigin, XMVECTOR rayDir, aiFace traingle , float& intersectDistance, XMVECTOR& pointOfIntersect);
	bool RayMeshIntersect( XMMATRIX worldMatrix, XMVECTOR rayOrigin, XMVECTOR rayDir, float& intersectDistance, XMVECTOR& intersectLocation);
private:
	std::vector<Vertex_PosTexNormTan> vertices;
	VertexBuffer<Vertex_PosTexNormTan> vertexBuffer;
	std::vector<DWORD> indices;
	IndexBuffer indexBuffer;
	ID3D11DeviceContext* deviceContext;
	XMMATRIX transformMatrix;
	std::vector<Texture> textures;
	aiMesh* mesh = nullptr;
	std::vector<aiFace> faces = {};
	std::string directory;

};

