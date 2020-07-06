#pragma once

#include "CollisionBody.h"

namespace physics
{
	struct PotentialContact
	{
		CollisionBody* body[2];
	};
}