#pragma once

namespace physics
{
	class CollisionBody;
	class RigidBody;
	class SoftBody;
	class Particle;

	/*struct BaseCollisionData
	{

	};*/

	struct RigidBodyCollisionData /*: public BaseCollisionData*/
	{
		RigidBody* own;
		CollisionBody* other;
		glm::vec3 contactOnA, contactOnB;
		glm::vec3 normalOnA, normalOnB;
		// ...
	};

	struct SoftBodyParticleCollisionData /*: public BaseCollisionData*/
	{
		unsigned int particle_id;
		glm::vec3 contact_point;
		glm::vec3 plane_normal;
		glm::vec3 reflected_velocity;
	};
}
