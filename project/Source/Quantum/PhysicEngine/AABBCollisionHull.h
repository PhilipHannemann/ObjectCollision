#pragma once
#include <glm\glm.hpp>
#include <EngineCore\AABB.h>
#include "CollisionHull.h"

namespace physics
{
	class AABBCollisionHull : public CollisionHull, public util::AABB
	{
	public:

		AABBCollisionHull(const glm::vec3& min, const glm::vec3&  max) : AABB(min, max) {}
		virtual ~AABBCollisionHull(){}

		virtual CollisionHull* copy() const override
		{
			return new AABBCollisionHull(min, max);
		}

		virtual CollisionHull* copyAndTransform(glm::mat4x4 trans) const override
		{
			return new AABBCollisionHull(glm::vec3(trans * glm::vec4(min, 1.0)), glm::vec3(trans * glm::vec4(max, 1.0)));
		}

		virtual util::AABB getAABB() 
		{
			return AABB(min, max);
		}
	
	};
}