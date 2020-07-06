#include "stdafx.h"
#include "RigidBody.h"
#include "CollisionHull.h"
#include "PhysicsWorld.h"

namespace physics
{

	RigidBody::RigidBody(float mass, CollisionHull * collision_hull)
	{
		this->mass = mass;
		this->collisionHull = collision_hull;

		if (mass == 0.0f)
			type = body_index::RIGIDBODY_KINEMATIC;
		else
			type = body_index::RIGIDBODY_DYNAMIC;

		last_change = DynamicsWorld::getCurrentTick();
	}

	RigidBody::~RigidBody()
	{
		delete collisionHull;
	}

	BroadphaseAABB RigidBody::recalculateAABB()
	{
		return BroadphaseAABB(this->collisionHull->getAABB());
	}

}