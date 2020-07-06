#pragma once

namespace physics
{	
	typedef unsigned int BodyIndex;

	namespace body_index
	{
		BodyIndex nextBodyIndex();
		BodyIndex getBodyIndexCount();

		extern BodyIndex SOFTBODY;
		extern BodyIndex RIGIDBODY_DYNAMIC;
		extern BodyIndex RIGIDBODY_KINEMATIC;
	}
}