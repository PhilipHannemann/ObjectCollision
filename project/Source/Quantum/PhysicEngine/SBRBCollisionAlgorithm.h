#pragma once

#include "AlgorithmBase.h"
#include "SoftBody.h"
#include "RigidBody.h"
#include "AABBCollisionHull.h"

namespace physics
{
	class SBRBCollisionAlgorithm : public ACollisionAlgorithm
	{
	public:

		SBRBCollisionAlgorithm();
		virtual ~SBRBCollisionAlgorithm();

		void testPair(CollisionBody*& bodyA, CollisionBody*& bodyB, const glm::vec3 &gravity, const float &air_drag, const float &time_delta);
	
	private:
		void prepareTraversal(SoftBody*& body);

		void updateNode(SoftBody*& body, SphereTreeNode*& node);

		float calcANormLinear(SoftBody*& body);

		float calcDeviationBasic(SoftBody*& body);

		float calcDeviationLinear(SoftBody*& body);

		void testNodeBox(SoftBody*& bodyA, SphereTreeNode*& nodeA, const AABBCollisionHull* box, const float &time_delta);

		bool sphereBoxIntersects(SphereTreeNode*& node, const AABBCollisionHull* box);

		bool pointBoxIntersects(const glm::vec3 &point, const AABBCollisionHull* box);
	
		void calcContactPointAndNormal(const glm::vec3 &particle_ray, const glm::vec3 &particle_ray_origin, const AABBCollisionHull* box, glm::vec3 &contact_point, glm::vec3 &normal);

		glm::vec3 calcReflectedVelocity(const glm::vec3 &entry_velocity, const glm::vec3 &normal, const float &restitution);
	};
}