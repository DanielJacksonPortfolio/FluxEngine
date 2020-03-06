#include "../../Tools/StrippedWindows.h"
#include <algorithm>
#include "CollisionHandler.h"

CollisionHandler* CollisionHandler::instance = nullptr;

CollisionHandler* CollisionHandler::Instance()
{
	if (instance == nullptr)
	{
		instance = new CollisionHandler();
	}
	return instance;
}

bool CollisionHandler::SphereSphereCollision(XMVECTOR sphere1Pos, float sphere1Radius, XMVECTOR sphere2Pos, float sphere2Radius)
{
	return fabs(XMVectorGetX(XMVector3Length(sphere1Pos - sphere2Pos))) < (sphere1Radius + sphere2Radius);
}

bool CollisionHandler::SpherePlaneCollision(XMVECTOR spherePos, float sphereRadius, XMVECTOR planeNormal, XMVECTOR pointOnPlane)
{
	float A = XMVectorGetX(planeNormal);
	float B = XMVectorGetY(planeNormal);
	float C = XMVectorGetZ(planeNormal);
	float D = -XMVectorGetX(pointOnPlane) - XMVectorGetY(pointOnPlane) - XMVectorGetZ(pointOnPlane);
	float numerator = fabs(A * XMVectorGetX(spherePos) + B * XMVectorGetY(spherePos) + C * XMVectorGetX(spherePos) + D);
	float denominator = sqrtf(A * A + B * B + C * C);

	return (numerator / denominator) < sphereRadius; //Distance < radius
}

bool CollisionHandler::SphereAABBCollision(XMVECTOR spherePos, float sphereRadius)
{
	return false;
}

bool CollisionHandler::SphereOBBCollision(XMVECTOR spherePos, float sphereRadius)
{
	return false;
}

bool CollisionHandler::AABBAABBCollision()
{
	return false;
}

bool CollisionHandler::AABBOBBCollision()
{
	return false;
}

bool CollisionHandler::OBBOBBCollision()
{
	return false;
}

bool CollisionHandler::RaySphereIntersect(XMVECTOR spherePos, float sphereRadius, XMVECTOR rayOrigin, XMVECTOR rayDir)
{
	XMVECTOR oc = rayOrigin - spherePos;
	float a = XMVectorGetX(XMVector3Dot(rayDir, rayDir));
	float b = 2 * XMVectorGetX(XMVector3Dot(oc, rayDir));
	float c = XMVectorGetX(XMVector3Dot(oc, oc)) - (sphereRadius * sphereRadius);
	return 0 < ((b * b) - (4 * a * c));

}

bool CollisionHandler::RayAABBIntersect(XMVECTOR aabbMin, XMVECTOR aabbMax, XMVECTOR aabbPos, XMVECTOR rayOrigin, XMVECTOR rayDir)
{
	XMVECTOR dirfrac = XMVectorSet(1 / XMVectorGetX(rayDir), 1 / XMVectorGetY(rayDir), 1 / XMVectorGetZ(rayDir), 0);

	float t1 = (XMVectorGetX(aabbMin + aabbPos) - XMVectorGetX(rayOrigin)) * XMVectorGetX(dirfrac);
	float t2 = (XMVectorGetX(aabbMax + aabbPos) - XMVectorGetX(rayOrigin)) * XMVectorGetX(dirfrac);
	float t3 = (XMVectorGetY(aabbMin + aabbPos) - XMVectorGetY(rayOrigin)) * XMVectorGetY(dirfrac);
	float t4 = (XMVectorGetY(aabbMax + aabbPos) - XMVectorGetY(rayOrigin)) * XMVectorGetY(dirfrac);
	float t5 = (XMVectorGetZ(aabbMin + aabbPos) - XMVectorGetZ(rayOrigin)) * XMVectorGetZ(dirfrac);
	float t6 = (XMVectorGetZ(aabbMax + aabbPos) - XMVectorGetZ(rayOrigin)) * XMVectorGetZ(dirfrac);

	float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	if (tmax < 0 || tmin > tmax)
		return false;
	return true;
}

bool CollisionHandler::RayOBBIntersect(XMVECTOR rayOrigin, XMVECTOR rayDir)
{
	return false;
}

bool CollisionHandler::RayTriangleIntersect(XMMATRIX worldMatrix, XMVECTOR rayOrigin, XMVECTOR rayDir, XMVECTOR vertex0, XMVECTOR vertex1, XMVECTOR vertex2, float& intersectDistance, XMVECTOR& pointOfIntersect)
{
	XMVECTOR normal = XMVector3Normalize(XMVector3Transform(XMVector3Cross(vertex1 - vertex0, vertex2 - vertex0), XMMatrixTranspose(XMMatrixInverse(nullptr, worldMatrix)))) * (XMVectorGetX(XMMatrixDeterminant(worldMatrix)) < 0 ? -1 : 1);

	vertex0 = XMVector3Transform(vertex0, worldMatrix);
	vertex1 = XMVector3Transform(vertex1, worldMatrix);
	vertex2 = XMVector3Transform(vertex2, worldMatrix);

	float normalDotRayDir = XMVectorGetX(XMVector3Dot(normal, rayDir));
	if (fabs(normalDotRayDir) < static_cast<float>(1e-8))
		return false;

	float t = -1 * (XMVectorGetX(XMVector3Dot(normal, rayOrigin)) - XMVectorGetX(XMVector3Dot(normal, vertex0))) / normalDotRayDir;
	if (t < 0) return false;

	pointOfIntersect = rayOrigin + t * rayDir;

	if (XMVectorGetX(XMVector3Dot(normal, XMVector3Cross(vertex1 - vertex0, pointOfIntersect - vertex0))) < 0 ||
		XMVectorGetX(XMVector3Dot(normal, XMVector3Cross(vertex2 - vertex1, pointOfIntersect - vertex1))) < 0 ||
		XMVectorGetX(XMVector3Dot(normal, XMVector3Cross(vertex0 - vertex2, pointOfIntersect - vertex2))) < 0) return false;

	float distance = XMVectorGetX(XMVector3Length(pointOfIntersect - rayOrigin));
	if (intersectDistance > distance)
		intersectDistance = distance;

	return true;
}

XMVECTOR CollisionHandler::VectorReflection(XMVECTOR vector, XMVECTOR planeNormal)
{
	return vector + 2 * XMVector3Dot(-vector, planeNormal) * planeNormal;
}
