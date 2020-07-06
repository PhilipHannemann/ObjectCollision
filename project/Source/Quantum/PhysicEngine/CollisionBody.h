#pragma once

#include "BodyIndex.h"
#include "BroadphaseAABB.h"
#include "CollisionHull.h"
#include "CollisionData.h"

namespace physics
{
	class CollisionBody
	{
		friend class CollisionWorld;

	private:
		unsigned int storageIndexID;

	public: 
		// body type
		BodyIndex type;
		// collisions with this body
		//std::vector<BaseCollisionData> collisions;
		
		// narrphase collision hull
		CollisionHull* collisionHull;

		//last position or geometry change
		int64_t last_change;

		// activition state
		// friction
		void * userptr;
		int userindex;

	public:
		CollisionBody();
		virtual ~CollisionBody();
		virtual BroadphaseAABB recalculateAABB() = 0;
	};
}