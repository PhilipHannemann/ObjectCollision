#include "stdafx.h"
#include "RigidBodyAABBRayIntersectAlgorithm.h"


namespace physics
{

	bool physics::RigidBodyAABBRayIntersectionAlgorithm::testRay(Ray ray, PickResult & data)
	{
		return calcContactPointAndNormal(ray, static_cast<AABBCollisionHull*>(data.body->collisionHull), data);
	}

	bool RigidBodyAABBRayIntersectionAlgorithm::calcContactPointAndNormal(Ray &ray, const AABBCollisionHull* box, PickResult & data)
	{
		glm::vec3 inv = 1.0f / ray.dir;

		glm::vec3 t1 = (box->min - ray.start)*inv;
		glm::vec3 t2 = (box->max - ray.start)*inv;

		float tmin = glm::min(t1.x, t2.x);
		float tmax = glm::max(t1.x, t2.x);

		tmin = glm::max(tmin, glm::min(t1.y, t2.y));
		tmax = glm::min(tmax, glm::max(t1.y, t2.y));

		tmin = glm::max(tmin, glm::min(t1.z, t2.z));
		tmax = glm::min(tmax, glm::max(t1.z, t2.z));

		if (tmax >= tmin)
		{
			data.pos = ray.start + (tmin * ray.dir);
			data.distance = tmin;
		}
		else
		{
			return false;
		}

		float min_dist = FLT_MAX;

		glm::vec3 aabb_center = (box->min + box->max) * 0.5f;
		glm::vec3 contact_point_local = data.pos - aabb_center;
		float distance;

		distance = glm::abs(box->dimensions[0] - glm::abs(contact_point_local.x));
		if (distance < min_dist)
		{
			min_dist = distance;
			data.normal = glm::sign(contact_point_local.x) * glm::vec3(1.f, 0.f, 0.f);
		}

		distance = glm::abs(box->dimensions[1] - glm::abs(contact_point_local.y));
		if (distance < min_dist)
		{
			min_dist = distance;
			data.normal = glm::sign(contact_point_local.y) * glm::vec3(0.f, 1.f, 0.f);
		}

		distance = glm::abs(box->dimensions[2] - glm::abs(contact_point_local.z));
		if (distance < min_dist)
		{
			min_dist = distance;
			data.normal = glm::sign(contact_point_local.z) * glm::vec3(0.f, 0.f, 1.f);
		}

		return true;
	}

}