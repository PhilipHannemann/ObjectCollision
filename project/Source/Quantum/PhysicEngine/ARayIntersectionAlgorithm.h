#pragma once
#include "CollisionBody.h"
#include "CollisionData.h"
#include "RayHelper.h"

namespace physics
{
	class ARayIntersectionAlgorithm
	{
	public:
		//ACollisionAlgorithm() {}
		virtual ~ARayIntersectionAlgorithm() {}
		virtual bool testRay(Ray ray, PickResult &data) = 0;
	};
}