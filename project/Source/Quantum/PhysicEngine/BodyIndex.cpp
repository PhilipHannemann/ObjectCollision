#include "stdafx.h"
#include "BodyIndex.h"

physics::BodyIndex next = 0;

physics::BodyIndex physics::body_index::nextBodyIndex()
{
	return next++;
}

physics::BodyIndex physics::body_index::getBodyIndexCount()
{
	return next;
}

namespace physics
{
	namespace body_index
	{
		BodyIndex SOFTBODY = nextBodyIndex();
		BodyIndex RIGIDBODY_DYNAMIC = nextBodyIndex();
		BodyIndex RIGIDBODY_KINEMATIC = nextBodyIndex();
	}
}