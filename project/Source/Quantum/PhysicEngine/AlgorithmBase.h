#pragma once

#include "CollisionBody.h"
#include "CollisionData.h"

namespace physics
{
	class ACollisionAlgorithm
	{
	public:
		//ACollisionAlgorithm() {}
		virtual ~ACollisionAlgorithm(){}

		virtual void testPair(CollisionBody*& bodyA, CollisionBody*& bodyB, const glm::vec3 &gravity, const float &air_drag, const float &time_delta)  = 0;
	};
}