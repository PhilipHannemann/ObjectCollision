#pragma once
#include "ARayIntersectionAlgorithm.h"
#include "AABBCollisionHull.h"

namespace physics
{
	class RigidBodyAABBRayIntersectionAlgorithm : public ARayIntersectionAlgorithm
	{
	public:
		

		RigidBodyAABBRayIntersectionAlgorithm() {}
		virtual ~RigidBodyAABBRayIntersectionAlgorithm() {}
		virtual bool testRay(Ray ray, PickResult &data) override;
		bool calcContactPointAndNormal(Ray & ray, const AABBCollisionHull * box, PickResult & data);
	};
}