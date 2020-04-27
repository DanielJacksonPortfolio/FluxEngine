#include "../../../Tools/StrippedWindows.h"
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

unsigned CollisionHandler::SphereSphereCollision(SpherePrimitive* sphere1, SpherePrimitive* sphere2, CollisionHandler::CollisionData* data)
{
	if (data->unusedContacts <= 0) return 0;

	XMVECTOR sphere1Pos = sphere1->rigidBody->GetPosition();
	XMVECTOR sphere2Pos = sphere2->rigidBody->GetPosition();
	XMVECTOR diff = sphere1Pos - sphere2Pos;
	float size = XMVectorGetX(XMVector3Length(diff));

	sphere1->radius = sphere1->rigidBody->GetAppearance()->GetScaledBoundingSphereRadius();
	sphere2->radius = sphere2->rigidBody->GetAppearance()->GetScaledBoundingSphereRadius();
	float dist = sphere1->radius + sphere2->radius;

	if (size <= 0 || size >= dist) return 0;
	XMVECTOR normal = diff / size;

	Contact* contact = data->contacts;
	contact->contactNormal = normal;
	contact->contactLocation = sphere1Pos + diff * 0.5f;
	contact->penetrationDepth = dist - size;
	contact->SetBodyData(sphere1->rigidBody, sphere2->rigidBody, data->friction, data->restitution);
	data->AddContacts();
	return 1;
}

unsigned CollisionHandler::SphereHalfSpaceCollision(SpherePrimitive* sphere, PlanePrimitive* plane, CollisionHandler::CollisionData* data)
{
	if (data->unusedContacts <= 0) return 0;
	XMVECTOR spherePos = sphere->rigidBody->GetPosition();
	sphere->radius = sphere->rigidBody->GetAppearance()->GetScaledBoundingSphereRadius();

	float distance = XMVectorGetX(XMVector3Dot(plane->normal, spherePos)) - sphere->radius - plane->offset;
	if (distance >= 0) return 0;

	Contact* contact = data->contacts;
	contact->contactNormal = plane->normal;
	contact->penetrationDepth = -distance;
	contact->contactLocation = spherePos - plane->normal * (distance + sphere->radius);
	contact->SetBodyData(sphere->rigidBody, nullptr, data->friction, data->restitution);
	data->AddContacts();
	return 1;
}
unsigned CollisionHandler::SphereTruePlaneCollision(SpherePrimitive* sphere, PlanePrimitive* plane, CollisionHandler::CollisionData* data)
{
	if (data->unusedContacts <= 0) return 0;
	XMVECTOR spherePos = sphere->rigidBody->GetPosition();
	sphere->radius = sphere->rigidBody->GetAppearance()->GetScaledBoundingSphereRadius();

	float centreDistance = XMVectorGetX(XMVector3Dot(plane->normal, spherePos)) - plane->offset;
	if (centreDistance* centreDistance > sphere->radius * sphere->radius) return 0;

	Contact* contact = data->contacts;

	float penetration = -centreDistance;

	contact->contactNormal = plane->normal;
	if (centreDistance < 0)
	{
		contact->contactNormal *= -1;
		penetration *= -1;
	}
	penetration += sphere->radius;

	contact->penetrationDepth = penetration;
	contact->contactLocation = spherePos - plane->normal * centreDistance;
	contact->SetBodyData(sphere->rigidBody, nullptr, data->friction, data->restitution);
	data->AddContacts();
	return 1;
}

//unsigned CollisionHandler::SpherePlaneCollision(XMVECTOR spherePos, float sphereRadius, XMVECTOR planeNormal, XMVECTOR pointOnPlane)
//{
//	float A = XMVectorGetX(planeNormal);
//	float B = XMVectorGetY(planeNormal);
//	float C = XMVectorGetZ(planeNormal);
//	float D = -XMVectorGetX(pointOnPlane) - XMVectorGetY(pointOnPlane) - XMVectorGetZ(pointOnPlane);
//	float numerator = static_cast<float>(fabs(A * XMVectorGetX(spherePos) + B * XMVectorGetY(spherePos) + C * XMVectorGetX(spherePos) + D));
//	float denominator = sqrtf(A * A + B * B + C * C);
//
//	return (numerator / denominator) < sphereRadius; //Distance < radius
//}

unsigned CollisionHandler::SphereAABBCollision(XMVECTOR spherePos, float sphereRadius, PropObject* object)
{
	return 0;
}

unsigned CollisionHandler::SphereOBBCollision(XMVECTOR spherePos, float sphereRadius)
{
	return 0;
}

unsigned CollisionHandler::AABBAABBCollision(PropObject* a, PropObject* b)
{
	return 0;
}

unsigned CollisionHandler::AABBOBBCollision(PropObject* a, PropObject* b)
{
	return 0;
}

unsigned CollisionHandler::OBBOBBCollision(PropObject* a, PropObject* b)
{
	return 0;
}

bool CollisionHandler::RaySphereIntersect(XMVECTOR spherePos, float sphereRadius, XMVECTOR rayOrigin, XMVECTOR rayDir)
{
	XMVECTOR oc = rayOrigin - spherePos;
	float a = XMVectorGetX(XMVector3Dot(rayDir, rayDir));
	float b = 2 * XMVectorGetX(XMVector3Dot(oc, rayDir));
	float c = XMVectorGetX(XMVector3Dot(oc, oc)) - (sphereRadius * sphereRadius);
	return 0 < ((b * b) - (4 * a * c));
}

bool CollisionHandler::RayBoxIntersect(XMVECTOR min, XMVECTOR max, XMVECTOR pos, XMVECTOR rayOrigin, XMVECTOR dirfrac)
{
	float t1 = (XMVectorGetX(min + pos) - XMVectorGetX(rayOrigin)) * XMVectorGetX(dirfrac);
	float t2 = (XMVectorGetX(max + pos) - XMVectorGetX(rayOrigin)) * XMVectorGetX(dirfrac);
	float t3 = (XMVectorGetY(min + pos) - XMVectorGetY(rayOrigin)) * XMVectorGetY(dirfrac);
	float t4 = (XMVectorGetY(max + pos) - XMVectorGetY(rayOrigin)) * XMVectorGetY(dirfrac);
	float t5 = (XMVectorGetZ(min + pos) - XMVectorGetZ(rayOrigin)) * XMVectorGetZ(dirfrac);
	float t6 = (XMVectorGetZ(max + pos) - XMVectorGetZ(rayOrigin)) * XMVectorGetZ(dirfrac);

	float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	if (tmax < 0 || tmin > tmax)
		return false;
	return true;
}

bool CollisionHandler::RayAABBIntersect(float scale, Model::AABB aabb, XMVECTOR aabbPos, XMVECTOR rayOrigin, XMVECTOR rayDir)
{
	XMVECTOR dirfrac = XMVectorSet(1 / XMVectorGetX(rayDir), 1 / XMVectorGetY(rayDir), 1 / XMVectorGetZ(rayDir), 0);

	XMVECTOR aabbMin = scale * XMVectorSet(-aabb.halfWidth, -aabb.halfHeight, -aabb.halfDepth,  0.0f);
	XMVECTOR aabbMax = scale * XMVectorSet( aabb.halfWidth,  aabb.halfHeight,  aabb.halfDepth,  0.0f);

	return RayBoxIntersect( aabbMin, aabbMax, aabbPos, rayOrigin, dirfrac);
}

bool CollisionHandler::RayOBBIntersect(float scale, Model::OBB obb, XMVECTOR obbPos, XMVECTOR rayOrigin, XMVECTOR rayDir, XMMATRIX rotationMatrix)
{
	rayOrigin = XMVectorSetW(XMVector3Transform(rayOrigin, XMMatrixInverse(nullptr, rotationMatrix)),1.0f);
	rayDir = XMVectorSetW(XMVector3Normalize(XMVector3Transform(rayDir, XMMatrixInverse(nullptr, rotationMatrix))),0.0f);
	XMVECTOR dirfrac = XMVectorSet(1 / XMVectorGetX(rayDir), 1 / XMVectorGetY(rayDir), 1 / XMVectorGetZ(rayDir), 0);
	//scale * 
	XMVECTOR obbMin = XMVectorSet(XMVectorGetX(obbPos) - obb.halfWidth, XMVectorGetY(obbPos) - obb.halfHeight, XMVectorGetZ(obbPos) - obb.halfDepth, 0.0f);
	XMVECTOR obbMax = XMVectorSet(XMVectorGetX(obbPos) + obb.halfWidth, XMVectorGetY(obbPos) + obb.halfHeight, XMVectorGetZ(obbPos) + obb.halfDepth, 0.0f);

	return RayBoxIntersect(obbMin, obbMax, obbPos, rayOrigin, dirfrac);
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

void CollisionHandler::ResolveCollision(RigidBody* obj1, RigidBody* obj2)
{
	obj1->GetTransform()->SetPosition(obj1->GetTransform()->GetLastPosition());
	obj2->GetTransform()->SetPosition(obj2->GetTransform()->GetLastPosition());
}

bool CollisionHandler::RayPropIntersect(PropObject* object, XMVECTOR rayOrigin, XMVECTOR rayDir, float& nearestIntersect, XMVECTOR& intersectLocation)
{
	object->UpdateMatrix();
	object->GetAppearance()->GetModel()->SetBoundingShape(XMMatrixRotationQuaternion(object->GetTransform()->GetOrientation()));
	return Instance()->RayModelIntersect(object->GetAppearance()->GetModel(),object->GetTransform()->GetRotationMatrix(), object->GetTransform()->GetPosition(), object->GetTransform()->GetScale(), rayOrigin, rayDir, nearestIntersect, intersectLocation);
}

bool CollisionHandler::RayModelIntersect(Model* model, XMMATRIX rotationMatrix, XMVECTOR position, float scale, XMVECTOR rayOrigin, XMVECTOR rayDir, float& nearestIntersect, XMVECTOR& intersectLocation)
{
	bool initialCheck = false;
	if (model->GetBoundingShape() == Model::BoundingShape::SPHERE)
	{
		model->SetScaledBoundingSphereRadius(scale);
		if (CollisionHandler::Instance()->RaySphereIntersect(position, model->GetScaledBoundingSphereRadius(), rayOrigin, rayDir))
			initialCheck = true;
	}
	else if (model->GetBoundingShape() == Model::BoundingShape::AABB)
	{
		if (CollisionHandler::Instance()->RayAABBIntersect(scale, model->GetAABB(), position, rayOrigin, rayDir))
			initialCheck = true;
	}
	else
	{
		if (CollisionHandler::Instance()->RayOBBIntersect(scale, model->GetOBB(), position, rayOrigin, rayDir, XMMatrixScaling(scale, scale, scale) * rotationMatrix * XMMatrixTranslationFromVector(position)))
			initialCheck = true;
	}
	if (initialCheck)
	{
		bool intersect = false;
		for (int i = 0; i < model->GetMeshes().size(); ++i)
		{
			float intersectDistance = INT_MAX;
			if (CollisionHandler::Instance()->RayMeshIntersect(model->GetMeshes()[i],XMMatrixTranslationFromVector(-model->GetOriginVector()) * XMMatrixScaling(scale,scale,scale) * rotationMatrix * XMMatrixTranslationFromVector(position), rayOrigin, rayDir, intersectDistance, intersectLocation))
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
