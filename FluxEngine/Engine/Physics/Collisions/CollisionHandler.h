#pragma once
#include <DirectXMath.h>
using namespace DirectX;
#include "Primitves/SpherePrimitive.h"
#include "Primitves/PlanePrimitive.h"
#include "Primitves/AABBPrimitive.h"
#include "Contacts/Contact.h"

#include "../../Game Objects/PropObject.h"

#define SpherePlaneCollision SphereHalfSpaceCollision

class CollisionHandler
{
public:

	struct CollisionData
	{
		Contact* contactArray;
		Contact* contacts;
		int unusedContacts;
		int usedContacts;
		float friction = 0.0f;
		float restitution = 0.5f;
		float tolerance = 0.0f;

		bool FreeContacts() { return unusedContacts > 0; }
		void Reset(unsigned maxContacts) 
		{ 
			 unusedContacts = maxContacts;
			 usedContacts = 0;
			 contacts = contactArray;
		}
		void AddContacts(unsigned numContacts = 1) 
		{ 
			unusedContacts -= numContacts;
			usedContacts += numContacts;

			contacts += numContacts;
		}
	};

	static CollisionHandler* Instance();
	unsigned SphereSphereCollision(SpherePrimitive* sphere1, SpherePrimitive* sphere2, CollisionData* data);
	unsigned SphereHalfSpaceCollision(SpherePrimitive* sphere, PlanePrimitive* plane, CollisionData* data);
	unsigned SphereTruePlaneCollision(SpherePrimitive* sphere, PlanePrimitive* plane, CollisionData* data);
//	unsigned SpherePlaneCollision(XMVECTOR spherePos, float sphereRadius, XMVECTOR planeNormal, XMVECTOR pointOnPlane);
	unsigned SphereAABBCollision(XMVECTOR spherePos, float sphereRadius, PropObject* object);
	unsigned SphereOBBCollision(XMVECTOR spherePos, float sphereRadius);
	unsigned AABBAABBCollision(PropObject* a, PropObject* b);
	unsigned AABBOBBCollision(PropObject* a, PropObject* b);
	unsigned OBBOBBCollision(PropObject* a, PropObject* b);

	bool RaySphereIntersect(XMVECTOR spherePos, float sphereRadius, XMVECTOR rayOrigin, XMVECTOR rayDir);
	bool RayBoxIntersect(XMVECTOR min, XMVECTOR max, XMVECTOR pos, XMVECTOR rayOrigin, XMVECTOR dirfrac);
	bool RayAABBIntersect(float scale, Model::AABB aabb, XMVECTOR aabbPos, XMVECTOR rayOrigin, XMVECTOR rayDir);
	bool RayOBBIntersect(float scale, Model::OBB obb, XMVECTOR obbPos, XMVECTOR rayOrigin, XMVECTOR rayDir, XMMATRIX rotationMatrix);
	bool RayTriangleIntersect(XMMATRIX worldMatrix, XMVECTOR rayOrigin, XMVECTOR rayDir, XMVECTOR vertex0, XMVECTOR vertex1, XMVECTOR vertex2, float& intersectDistance, XMVECTOR& pointOfIntersect);

	XMVECTOR VectorReflection(XMVECTOR vector, XMVECTOR planeNormal);

	void ResolveCollision(RigidBody* obj1, RigidBody* obj12);

	bool RayPropIntersect(PropObject* object, XMVECTOR rayOrigin, XMVECTOR rayDir, float& nearestIntersect, XMVECTOR& intersectLocation);

	bool RayModelIntersect(Model* model, XMMATRIX worldMatrix, XMVECTOR position, float scale, XMVECTOR rayOrigin, XMVECTOR rayDir, float& nearestIntersect, XMVECTOR& intersectLocation);

	bool RayMeshIntersect(Mesh* mesh, XMMATRIX worldMatrix, XMVECTOR rayOrigin, XMVECTOR rayDir, float& intersectDistance, XMVECTOR& intersectLocation);

private:
	static CollisionHandler* instance;
	CollisionHandler() {}
};

