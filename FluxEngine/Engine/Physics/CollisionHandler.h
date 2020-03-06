#pragma once
#include <DirectXMath.h>
using namespace DirectX;

#include "../Game Objects/PropObject.h"

class CollisionHandler
{
public:
	static CollisionHandler* Instance();
	bool SphereSphereCollision(XMVECTOR sphere1Pos, float sphere1Radius, XMVECTOR sphere2Pos, float sphere2Radius);
	bool SpherePlaneCollision(XMVECTOR spherePos, float sphereRadius, XMVECTOR planeNormal, XMVECTOR pointOnPlane);
	bool SphereAABBCollision(XMVECTOR spherePos, float sphereRadius);
	bool SphereOBBCollision(XMVECTOR spherePos, float sphereRadius);
	bool AABBAABBCollision();
	bool AABBOBBCollision();
	bool OBBOBBCollision();

	bool RaySphereIntersect(XMVECTOR spherePos, float sphereRadius, XMVECTOR rayOrigin, XMVECTOR rayDir);
	bool RayAABBIntersect(XMVECTOR aabbMin, XMVECTOR aabbMax, XMVECTOR aabbPos, XMVECTOR rayOrigin, XMVECTOR rayDir);
	bool RayOBBIntersect(XMVECTOR rayOrigin, XMVECTOR rayDir);
	bool RayTriangleIntersect(XMMATRIX worldMatrix, XMVECTOR rayOrigin, XMVECTOR rayDir, XMVECTOR vertex0, XMVECTOR vertex1, XMVECTOR vertex2, float& intersectDistance, XMVECTOR& pointOfIntersect);

	XMVECTOR VectorReflection(XMVECTOR vector, XMVECTOR planeNormal);

	bool RayPropIntersect(PropObject* object, XMVECTOR rayOrigin, XMVECTOR rayDir, float& nearestIntersect, XMVECTOR& intersectLocation);

	bool RayModelIntersect(Model* model, XMMATRIX worldMatrix, XMVECTOR position, float scale, XMVECTOR rayOrigin, XMVECTOR rayDir, float& nearestIntersect, XMVECTOR& intersectLocation);

	bool RayMeshIntersect(Mesh* mesh, XMMATRIX worldMatrix, XMVECTOR rayOrigin, XMVECTOR rayDir, float& intersectDistance, XMVECTOR& intersectLocation);

private:
	static CollisionHandler* instance;
	CollisionHandler() {}
};

