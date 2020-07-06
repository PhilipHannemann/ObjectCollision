#pragma once
#include <EngineCore\AABB.h>
//#include "BVHNode.h"

namespace physics
{
	//class BVHNode;

	class BroadphaseAABB : public util::AABB
	{
	public:
		//BVHNode* owner;
		
		BroadphaseAABB() : util::AABB() { /*owner = NULL;*/ }
		BroadphaseAABB(util::AABB &aabb) : util::AABB(aabb) { /*owner = NULL;*/ }
		BroadphaseAABB(const glm::vec3& min, const glm::vec3&  max) : util::AABB(min, max) { /*owner = NULL;*/ }
		virtual ~BroadphaseAABB() {/* owner = NULL;*/ }

		void recalculateAABB(const glm::vec3& min, const glm::vec3& max) override;

		bool overlaps(const BroadphaseAABB& other) const;
		bool contains(const BroadphaseAABB& other) const;
		float getGrowth(const BroadphaseAABB& other);

	};
}