#pragma once

#include "CollisionBody.h"

namespace physics
{
	class RigidBody : public CollisionBody
	{
	public:
		//std::vector<glm::vec3> vertices;

		float mass;

		glm::vec3 COM;

		std::vector<RigidBodyCollisionData> collisions;

		//
		// Constructor / Destructor
		//
		RigidBody(float mass, CollisionHull *collision_hull);

		~RigidBody();

		virtual BroadphaseAABB recalculateAABB() override;
	};
}