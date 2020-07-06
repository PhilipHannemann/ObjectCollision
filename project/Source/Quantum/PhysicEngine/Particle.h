#pragma once

namespace physics
{
	class Particle
	{
	public:
		enum NarrowPhaseTestState
		{
			TESTED,
			UNTESTED
		};

		enum ParticleSleepState
		{
			ACTIVE,
			SLEEPING
		};

		glm::vec3 pos;
		glm::vec3 goal_pos;
		glm::vec3 init_pos;
		glm::vec3 intermediate_pos;
		glm::vec3 pos_prev_step;
		glm::vec3 velocity;
		glm::vec3 intermediate_velocity;
		glm::vec3 applied_force;

		float mass;

		NarrowPhaseTestState narrowphase_test_state;
		ParticleSleepState sleep_state;
		int sleep_counter;

	public:
		Particle(float x, float y, float z);
		~Particle();
	};
}