#include "../../Tools/StrippedWindows.h"
#include <algorithm>
#include "../Physics/CollisionHandler.h"

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
	return XMVectorGetX(XMVector3Length(sphere1Pos - sphere2Pos)) < (sphere1Radius + sphere2Radius);
}

bool CollisionHandler::SpherePlaneCollision(XMVECTOR spherePos, float sphereRadius, XMVECTOR planeNormal, XMVECTOR pointOnPlane)
{
	float A = XMVectorGetX(planeNormal);
	float B = XMVectorGetY(planeNormal);
	float C = XMVectorGetZ(planeNormal);
	float D = -XMVectorGetX(pointOnPlane) - XMVectorGetY(pointOnPlane) - XMVectorGetZ(pointOnPlane);
	float numerator = static_cast<float>(fabs(A * XMVectorGetX(spherePos) + B * XMVectorGetY(spherePos) + C * XMVectorGetX(spherePos) + D));
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
	XMVECTOR normal = XMVector3Normalize(XMVector3Transform(XMVector3Cross(vertex1 - vertex0, vertex2 - vertex0), XMMatrixTranspose(XMMatrixInverse(nullptr, worldMatrix)))) * (XMVectorGetX(XMMatrixDeterminant(worldMatrix)) < 0.0f ? -1.0f : 1.0f);

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

void CollisionHandler::ResolveCollision(Physics* obj1, Physics* obj2)
{
	obj1->GetTransform()->SetPosition(obj1->GetTransform()->GetLastPosition());
	obj2->GetTransform()->SetPosition(obj2->GetTransform()->GetLastPosition());

}

bool CollisionHandler::RayPropIntersect(PropObject* object, XMVECTOR rayOrigin, XMVECTOR rayDir, float& nearestIntersect, XMVECTOR& intersectLocation)
{
	object->UpdateMatrix();
	object->GetAppearance()->GetModel()->SetBoundingShape(XMMatrixRotationQuaternion(object->GetTransform()->GetOrientation()));
	return Instance()->RayModelIntersect(object->GetAppearance()->GetModel(),object->GetTransform()->GetWorldMatrix(), object->GetTransform()->GetPosition(), object->GetTransform()->GetScale(), rayOrigin, rayDir, nearestIntersect, intersectLocation);
}

bool CollisionHandler::RayModelIntersect(Model* model, XMMATRIX worldMatrix, XMVECTOR position, float scale, XMVECTOR rayOrigin, XMVECTOR rayDir, float& nearestIntersect, XMVECTOR& intersectLocation)
{
	bool initialCheck = false;
	if (model->boundingShape == Model::BoundingShape::SPHERE)
	{
		model->objectBoundingSphereRadius = model->modelBoundingSphereRadius * scale;
		if (CollisionHandler::Instance()->RaySphereIntersect(position, model->objectBoundingSphereRadius, rayOrigin, rayDir))
			initialCheck = true;
	}
	else
	{
		if (CollisionHandler::Instance()->RayAABBIntersect(scale * model->minAABBCoord, scale * model->maxAABBCoord, position, rayOrigin, rayDir))
			initialCheck = true;
	}
	if (initialCheck)
	{
		bool intersect = false;
		for (int i = 0; i < model->meshes.size(); ++i)
		{
			float intersectDistance = INT_MAX;
			if (CollisionHandler::Instance()->RayMeshIntersect(model->meshes[i],XMMatrixTranslationFromVector(-model->originVector) * worldMatrix, rayOrigin, rayDir, intersectDistance, intersectLocation))
			{
				intersect = true;
				if (nearestIntersect > intersectDistance)
					nearestIntersect = intersectDistance;
			}
		}
		if (intersect)
			return true;
	}
	return false;
}

bool CollisionHandler::RayMeshIntersect(Mesh* mesh, XMMATRIX worldMatrix, XMVECTOR rayOrigin, XMVECTOR rayDir, float& intersectDistance, XMVECTOR& intersectLocation)
{
	bool intersect = false;
	if (mesh->faces.size() > 0)
	{
		worldMatrix = mesh->transformMatrix * worldMatrix;
		for (int i = 0; i < mesh->faces.size(); ++i)
		{
			XMVECTOR vertex0 = XMVectorSet(mesh->vertices[mesh->faces[i].mIndices[0]].pos.x, mesh->vertices[mesh->faces[i].mIndices[0]].pos.y, mesh->vertices[mesh->faces[i].mIndices[0]].pos.z, 1.0f);
			XMVECTOR vertex1 = XMVectorSet(mesh->vertices[mesh->faces[i].mIndices[1]].pos.x, mesh->vertices[mesh->faces[i].mIndices[1]].pos.y, mesh->vertices[mesh->faces[i].mIndices[1]].pos.z, 1.0f);
			XMVECTOR vertex2 = XMVectorSet(mesh->vertices[mesh->faces[i].mIndices[2]].pos.x, mesh->vertices[mesh->faces[i].mIndices[2]].pos.y, mesh->vertices[mesh->faces[i].mIndices[2]].pos.z, 1.0f);

			if (CollisionHandler::Instance()->RayTriangleIntersect(worldMatrix, rayOrigin, rayDir, vertex0, vertex1, vertex2, intersectDistance, intersectLocation))
			{
				intersect = true;
			}
		}
	}
	return intersect;
}
