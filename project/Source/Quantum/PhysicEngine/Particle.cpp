#include "stdafx.h"
#include "Particle.h"

namespace physics
{

	Particle::Particle(float x, float y, float z)
	{
		init_pos = glm::vec3(x, y, z);
		pos = init_pos;
		goal_pos = init_pos;
		intermediate_pos = init_pos;
		pos_prev_step = pos;
		velocity = glm::vec3(0.f);
		intermediate_velocity = velocity;

		narrowphase_test_state = UNTESTED;
		sleep_state = ACTIVE;
		sleep_counter = 0;
	}

	Particle::~Particle()
	{

	}
}