#pragma once
#include <EngineCore\AABB.h>
#include <glm\glm.hpp>

namespace physics
{

	class CollisionHull
	{
	public:
		virtual CollisionHull* copy() const = 0;
		virtual CollisionHull* copyAndTransform(glm::mat4x4 trans) const = 0;
		virtual ~CollisionHull() {}
		virtual util::AABB getAABB() = 0;
	};
}